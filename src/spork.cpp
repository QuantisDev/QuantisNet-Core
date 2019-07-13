// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "spork.h"

#include "base58.h"
#include "chainparams.h"
#include "validation.h"
#include "messagesigner.h"
#include "net_processing.h"
#include "netmessagemaker.h"
#include "checkpoints.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>

CSporkManager sporkManager;

std::map<uint256, CSporkMessage> mapSporks;
std::map<int, int64_t> mapSporkDefaults = {
    {SPORK_2_INSTANTSEND_ENABLED,            0},             // ON
    {SPORK_3_INSTANTSEND_BLOCK_FILTERING,    0},             // ON
    {SPORK_5_INSTANTSEND_MAX_VALUE,          1000},          // 1000 QUAN
    {SPORK_6_NEW_SIGS,                       1551207600ULL}, // ON @mainnet
    {SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT, 1541375412ULL}, // ON
    {SPORK_9_SUPERBLOCKS_ENABLED,            0},             // ON
    {SPORK_10_MASTERNODE_PAY_UPDATED_NODES,  1551200400ULL}, // ON @mainnet
    {SPORK_12_RECONSIDER_BLOCKS,             0},             // 0 BLOCKS
    {SPORK_14_REQUIRE_SENTINEL_FLAG,         1545415606ULL}, // ON
    {SPORK_15_FIRST_POS_BLOCK,               315ULL},     // ON @mainnet
    {SPORK_16_MASTERNODE_MIN_PROTOCOL,       MIN_PEER_PROTO_VERSION_BEFORE_ENFORCEMENT }, // Actual
    {SPORK_17_NEWPROTO_ENFORCE,                   1563195813ULL}, // July 15th
};
SporkCheckpointMap mapSporkCheckpoints GUARDED_BY(cs_main);
SporkBlacklistMap mapSporkBlacklist GUARDED_BY(cs_main);

void CSporkManager::ProcessSpork(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman)
{
    if(fLiteMode) return; // disable all QuantisNet specific functionality

    if (strCommand == NetMsgType::SPORK) {

        CSporkMessage spork;
        vRecv >> spork;

        uint256 hash = spork.GetHash();

        std::string strLogMsg;
        {
            LOCK(cs_main);
            pfrom->setAskFor.erase(hash);
            if(!chainActive.Tip()) return;
            strLogMsg = strprintf("SPORK -- hash: %s id: %d value: %10d bestHeight: %d peer=%d", hash.ToString(), spork.nSporkID, spork.nValue, chainActive.Height(), pfrom->id);
        }

        if(mapSporksActive.count(spork.nSporkID)) {
            if (mapSporksActive[spork.nSporkID].nTimeSigned >= spork.nTimeSigned) {
                LogPrint("spork", "%s seen\n", strLogMsg);
                return;
            } else {
                LogPrintf("%s updated\n", strLogMsg);
            }
        } else {
            LogPrintf("%s new\n", strLogMsg);
        }

        if(!spork.CheckSignature(sporkPubKeyID)) {
            LOCK(cs_main);
            LogPrintf("CSporkManager::ProcessSpork -- ERROR: invalid signature\n");
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        mapSporks[hash] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        spork.Relay(connman);

        //does a task if needed
        ExecuteSpork(spork.nSporkID, spork.nValue);

    } else if (strCommand == NetMsgType::GETSPORKS) {

        std::map<int, CSporkMessage>::iterator it = mapSporksActive.begin();

        while(it != mapSporksActive.end()) {
            connman.PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::SPORK, it->second));
            it++;
        }

        // Dynamic checkpoints are closely related to sporks functionality
        if (pfrom->nVersion >= SPORK_CHECKPOINT_VERSION) {
            LOCK(cs_main);

            auto min_time = GetAdjustedTime() - CSporkCheckpoint::MAX_AGE;

            for (auto iter = mapSporkCheckpoints.begin(); iter != mapSporkCheckpoints.end();) {
                auto& cp = iter->second;

                // Avoid polluting network with old checkpoints
                if (cp.nTimeSigned < min_time) {
                    // Cleanup active as side-effect
                    auto active_iter = mapCheckpointsActive.find(cp.nHeight);

                    if ((active_iter != mapCheckpointsActive.end()) &&
                        (active_iter->second == cp)
                    ) {
                        mapCheckpointsActive.erase(active_iter);
                    }

                    auto todel = iter++;
                    mapSporkCheckpoints.erase(todel);
                    continue;
                }

                connman.PushMessage(
                    pfrom,
                    CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::CHECKPOINT, cp));
                ++iter;
            }
        }

        // Dynamic blacklists are closely related to sporks functionality
        if (pfrom->nVersion >= SPORK_BLACKLIST_VERSION) {
            LOCK(cs_main);

            auto min_time = GetAdjustedTime() - CSporkBlacklist::MAX_AGE;

            for (auto iter = mapSporkBlacklist.begin(); iter != mapSporkBlacklist.end();) {
                auto& bl = iter->second;

                // Avoid polluting network with old checkpoints
                if (bl.nTimeSigned < min_time) {
                    // Cleanup active as side-effect
                    auto active_iter = mapBlacklistActive.find(bl.scriptPubKey);

                    if ((active_iter != mapBlacklistActive.end()) &&
                        (active_iter->second == bl)
                    ) {
                        mapBlacklistActive.erase(active_iter);
                    }

                    auto todel = iter++;
                    mapSporkBlacklist.erase(todel);
                    continue;
                }

                connman.PushMessage(
                    pfrom,
                    CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::BLACKLIST, bl));
                ++iter;
            }
        }
    } else if (strCommand == NetMsgType::CHECKPOINT) {

        CSporkCheckpoint checkpoint;
        vRecv >> checkpoint;

        {
            auto height = checkpoint.nHeight;
            auto hash = checkpoint.GetHash();

            LOCK(cs_main);

            pfrom->setAskFor.erase(hash);

            std::string strLogMsg = strprintf("DYNCHECKPOINT -- hash: %s height: %d block: %s peer=%d", hash.ToString(), height, checkpoint.hashBlock.ToString(), pfrom->id);

            auto iter = mapCheckpointsActive.find(height);

            if(iter != mapCheckpointsActive.end()) {
                if (iter->second.nTimeSigned >= checkpoint.nTimeSigned) {
                    LogPrint("spork", "%s seen\n", strLogMsg);
                    return;
                } else {
                    LogPrintf("%s updated\n", strLogMsg);
                }
            } else {
                LogPrintf("%s new\n", strLogMsg);
            }

            if(!checkpoint.CheckSignature(sporkPubKeyID)) {
                LogPrintf("CSporkManager::ProcessSpork checkpoint -- ERROR: invalid signature\n");
                Misbehaving(pfrom->GetId(), 100);
                return;
            }

            mapSporkCheckpoints[hash] = checkpoint;
            mapCheckpointsActive[height] = checkpoint;
            ExecuteCheckpoint(height, checkpoint.hashBlock);
        }

        checkpoint.Relay(connman);
    } else if (strCommand == NetMsgType::BLACKLIST) {

        CSporkBlacklist blacklist;
        vRecv >> blacklist;

        {
            auto scriptPubKey = blacklist.scriptPubKey;
            auto hash = blacklist.GetHash();

            LOCK(cs_main);

            pfrom->setAskFor.erase(hash);

            std::string strLogMsg = strprintf("DYNBLACKLIST -- hash: %s script: %d since: %s peer=%d",
                                              hash.ToString(),
                                              HexStr(scriptPubKey).c_str(),
                                              blacklist.nTimeSince, pfrom->id);

            auto iter = mapBlacklistActive.find(scriptPubKey);

            if(iter != mapBlacklistActive.end()) {
                if (iter->second.nTimeSigned >= blacklist.nTimeSigned) {
                    LogPrint("spork", "%s seen\n", strLogMsg);
                    return;
                } else {
                    LogPrintf("%s updated\n", strLogMsg);
                }
            } else {
                LogPrintf("%s new\n", strLogMsg);
            }

            if(!blacklist.CheckSignature(sporkPubKeyID)) {
                LogPrintf("CSporkManager::ProcessSpork blacklist -- ERROR: invalid signature\n");
                Misbehaving(pfrom->GetId(), 100);
                return;
            }

            mapSporkBlacklist[hash] = blacklist;
            mapBlacklistActive[scriptPubKey] = blacklist;
            ExecuteBlacklist(scriptPubKey, blacklist.nTimeSince);
        }

        blacklist.Relay(connman);
    }
}

void CSporkManager::ExecuteSpork(int nSporkID, int nValue)
{
    //correct fork via spork technology
    if(nSporkID == SPORK_12_RECONSIDER_BLOCKS && nValue > 0) {
        // allow to reprocess 24h of blocks max, which should be enough to resolve any issues
        int64_t nMaxBlocks = 1440;
        // this potentially can be a heavy operation, so only allow this to be executed once per 10 minutes
        int64_t nTimeout = 10 * 60;

        static int64_t nTimeExecuted = 0; // i.e. it was never executed before

        if(GetTime() - nTimeExecuted < nTimeout) {
            LogPrint("spork", "CSporkManager::ExecuteSpork -- ERROR: Trying to reconsider blocks, too soon - %d/%d\n", GetTime() - nTimeExecuted, nTimeout);
            return;
        }

        if(nValue > nMaxBlocks) {
            LogPrintf("CSporkManager::ExecuteSpork -- ERROR: Trying to reconsider too many blocks %d/%d\n", nValue, nMaxBlocks);
            return;
        }


        LogPrintf("CSporkManager::ExecuteSpork -- Reconsider Last %d Blocks\n", nValue);

        ReprocessBlocks(nValue);
        nTimeExecuted = GetTime();
    }

    // if (nSporkID == SPORK_15_FIRST_POS_BLOCK) {
    //     LOCK(cs_main);

    //     if ((nValue < int(nFirstPoSBlock)) &&
    //         (nValue > chainActive.Tip()->nHeight)) {
    //         nFirstPoSBlock = nValue;
    //     } else if (nValue != int(nFirstPoSBlock)) {
    //         error("SPORK15 conflicts with current chain %d vs. %d", nValue, nFirstPoSBlock);
    //     }
    // }
}

bool CSporkManager::UpdateSpork(int nSporkID, int64_t nValue, CConnman& connman)
{

    CSporkMessage spork = CSporkMessage(nSporkID, nValue, GetAdjustedTime());

    if(spork.Sign(sporkPrivKey)) {
        spork.Relay(connman);
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[nSporkID] = spork;
        ExecuteSpork(nSporkID, nValue);
        return true;
    }

    return false;
}

void CSporkManager::ExecuteCheckpoint(int height, const uint256& block_hash)
{
    LOCK(cs_main);

    LogPrintf("Adding dynamic checkpoint at height %d with hash %s\n",
                height, block_hash.ToString().c_str());

    auto& chainparams = Params();
    Params(chainparams.NetworkIDString()).AddCheckpoint(height, block_hash);
    CheckpointValidateBlockIndex(chainparams);
}

bool CSporkManager::UpdateCheckpoint(int height, const uint256& block_hash, CConnman& connman)
{
    auto checkpoint = CSporkCheckpoint(height, block_hash, GetAdjustedTime());

    if(checkpoint.Sign(sporkPrivKey)) {
        checkpoint.Relay(connman);

        {
            LOCK(cs_main);
            mapSporkCheckpoints[checkpoint.GetHash()] = checkpoint;
            mapCheckpointsActive[height] = checkpoint;
            ExecuteCheckpoint(height, block_hash);
        }

        return true;
    }

    return false;
}

void CSporkManager::ExecuteBlacklist(const CScript &scriptPubKey, int64_t nTimeSince)
{
    LOCK(cs_main);

    LogPrintf("Adding dynamic blacklist for %s since %lld\n",
              HexStr(scriptPubKey).c_str(), nTimeSince);

    auto& chainparams = Params();
    Params(chainparams.NetworkIDString()).SetBlacklist(scriptPubKey, nTimeSince);
    ProcessScriptBlacklist(scriptPubKey, nTimeSince);
}

bool CSporkManager::UpdateBlacklist(const CScript &scriptPubKey, int64_t nTimeSince, CConnman& connman)
{
    auto blacklist = CSporkBlacklist(scriptPubKey, nTimeSince, GetAdjustedTime());

    if(blacklist.Sign(sporkPrivKey)) {
        blacklist.Relay(connman);

        {
            LOCK(cs_main);
            mapSporkBlacklist[blacklist.GetHash()] = blacklist;
            mapBlacklistActive[scriptPubKey] = blacklist;
            ExecuteBlacklist(scriptPubKey, nTimeSince);
        }

        return true;
    }

    return false;
}

// grab the spork, otherwise say it's off
bool CSporkManager::IsSporkActive(int nSporkID)
{
    int64_t r = -1;

    if(mapSporksActive.count(nSporkID)){
        r = mapSporksActive[nSporkID].nValue;
    } else if (mapSporkDefaults.count(nSporkID)) {
        r = mapSporkDefaults[nSporkID];
    } else {
        LogPrint("spork", "CSporkManager::IsSporkActive -- Unknown Spork ID %d\n", nSporkID);
        r = 4070908800ULL; // 2099-1-1 i.e. off by default
    }

    return r < GetAdjustedTime();
}

// grab the value of the spork on the network, or the default
int64_t CSporkManager::GetSporkValue(int nSporkID)
{
    if (mapSporksActive.count(nSporkID))
        return mapSporksActive[nSporkID].nValue;

    if (mapSporkDefaults.count(nSporkID)) {
        return mapSporkDefaults[nSporkID];
    }

    LogPrint("spork", "CSporkManager::GetSporkValue -- Unknown Spork ID %d\n", nSporkID);
    return -1;
}

int CSporkManager::GetSporkIDByName(const std::string& strName)
{
    if (strName == "SPORK_2_INSTANTSEND_ENABLED")               return SPORK_2_INSTANTSEND_ENABLED;
    if (strName == "SPORK_3_INSTANTSEND_BLOCK_FILTERING")       return SPORK_3_INSTANTSEND_BLOCK_FILTERING;
    if (strName == "SPORK_5_INSTANTSEND_MAX_VALUE")             return SPORK_5_INSTANTSEND_MAX_VALUE;
    if (strName == "SPORK_6_NEW_SIGS")                          return SPORK_6_NEW_SIGS;
    if (strName == "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT")    return SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT;
    if (strName == "SPORK_9_SUPERBLOCKS_ENABLED")               return SPORK_9_SUPERBLOCKS_ENABLED;
    if (strName == "SPORK_10_MASTERNODE_PAY_UPDATED_NODES")     return SPORK_10_MASTERNODE_PAY_UPDATED_NODES;
    if (strName == "SPORK_12_RECONSIDER_BLOCKS")                return SPORK_12_RECONSIDER_BLOCKS;
    if (strName == "SPORK_14_REQUIRE_SENTINEL_FLAG")            return SPORK_14_REQUIRE_SENTINEL_FLAG;
    if (strName == "SPORK_15_FIRST_POS_BLOCK")                  return SPORK_15_FIRST_POS_BLOCK;
    if (strName == "SPORK_16_MASTERNODE_MIN_PROTOCOL")          return SPORK_16_MASTERNODE_MIN_PROTOCOL;
    if (strName == "SPORK_17_NEWPROTO_ENFORCE")          return SPORK_17_NEWPROTO_ENFORCE;

    LogPrint("spork", "CSporkManager::GetSporkIDByName -- Unknown Spork name '%s'\n", strName);
    return -1;
}

std::string CSporkManager::GetSporkNameByID(int nSporkID)
{
    switch (nSporkID) {
        case SPORK_2_INSTANTSEND_ENABLED:               return "SPORK_2_INSTANTSEND_ENABLED";
        case SPORK_3_INSTANTSEND_BLOCK_FILTERING:       return "SPORK_3_INSTANTSEND_BLOCK_FILTERING";
        case SPORK_5_INSTANTSEND_MAX_VALUE:             return "SPORK_5_INSTANTSEND_MAX_VALUE";
        case SPORK_6_NEW_SIGS:                          return "SPORK_6_NEW_SIGS";
        case SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT:    return "SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT";
        case SPORK_9_SUPERBLOCKS_ENABLED:               return "SPORK_9_SUPERBLOCKS_ENABLED";
        case SPORK_10_MASTERNODE_PAY_UPDATED_NODES:     return "SPORK_10_MASTERNODE_PAY_UPDATED_NODES";
        case SPORK_12_RECONSIDER_BLOCKS:                return "SPORK_12_RECONSIDER_BLOCKS";
        case SPORK_14_REQUIRE_SENTINEL_FLAG:            return "SPORK_14_REQUIRE_SENTINEL_FLAG";
        case SPORK_15_FIRST_POS_BLOCK:                  return "SPORK_15_FIRST_POS_BLOCK";
        case SPORK_16_MASTERNODE_MIN_PROTOCOL:          return "SPORK_16_MASTERNODE_MIN_PROTOCOL";
        case SPORK_17_NEWPROTO_ENFORCE:          return "SPORK_17_NEWPROTO_ENFORCE";
        default:
            LogPrint("spork", "CSporkManager::GetSporkNameByID -- Unknown Spork ID %d\n", nSporkID);
            return "Unknown";
    }
}

bool CSporkManager::SetSporkAddress(const std::string& strAddress) {
    CBitcoinAddress address(strAddress);
    if (!address.IsValid() || !address.GetKeyID(sporkPubKeyID)) {
        LogPrintf("CSporkManager::SetSporkAddress -- Failed to parse spork address\n");
        return false;
    }
    return true;
}

bool CSporkManager::SetPrivKey(const std::string& strPrivKey)
{
    CKey key;
    CPubKey pubKey;
    if(!CMessageSigner::GetKeysFromSecret(strPrivKey, key, pubKey)) {
        LogPrintf("CSporkManager::SetPrivKey -- Failed to parse private key\n");
        return false;
    }

    if (pubKey.GetID() != sporkPubKeyID) {
        LogPrintf("CSporkManager::SetPrivKey -- New private key does not belong to spork address\n");
        return false;
    }

    CSporkMessage spork;
    if (spork.Sign(key)) {
        // Test signing successful, proceed
        LogPrintf("CSporkManager::SetPrivKey -- Successfully initialized as spork signer\n");

        sporkPrivKey = key;
        return true;
    } else {
        LogPrintf("CSporkManager::SetPrivKey -- Test signing failed\n");
        return false;
    }
}

CSporkManager::ActiveCheckpointMap CSporkManager::GetActiveCheckpoints() const {
    LOCK(cs_main);
    auto ret = mapCheckpointsActive;
    return ret;
}

CSporkManager::ActiveBlacklistMap CSporkManager::GetActiveBlacklists() const {
    LOCK(cs_main);
    auto ret = mapBlacklistActive;
    return ret;
}

//---

template<typename SporkType, int MsgType, int MinProtocol>
uint256 CSporkBase<SporkType, MsgType, MinProtocol>::GetHash() const
{
    return SerializeHash(*this);
}

template<typename SporkType, int MsgType, int MinProtocol>
uint256 CSporkBase<SporkType, MsgType, MinProtocol>::GetSignatureHash() const
{
    return GetHash();
}

template<typename SporkType, int MsgType, int MinProtocol>
bool CSporkBase<SporkType, MsgType, MinProtocol>::Sign(const CKey& key)
{
    if (!key.IsValid()) {
        LogPrintf("CSporkBase::Sign -- signing key is not valid\n");
        return false;
    }

    CKeyID pubKeyId = key.GetPubKey().GetID();
    std::string strError = "";

    uint256 hash = GetSignatureHash();

    if(!CHashSigner::SignHash(hash, key, vchSig)) {
        LogPrintf("CSporkBase::Sign -- SignHash() failed\n");
        return false;
    }

    if (!CHashSigner::VerifyHash(hash, pubKeyId, vchSig, strError)) {
        LogPrintf("CSporkBase::Sign -- VerifyHash() failed, error: %s\n", strError);
        return false;
    }

    return true;
}

// Backward compatibility support
bool CSporkMessage::Sign(const CKey& key)
{
    if (!key.IsValid()) {
        LogPrintf("CSporkMessage::Sign -- signing key is not valid\n");
        return false;
    }

    CKeyID pubKeyId = key.GetPubKey().GetID();
    std::string strError = "";

    if (sporkManager.IsSporkActive(SPORK_6_NEW_SIGS)) {
        uint256 hash = GetSignatureHash();

        if(!CHashSigner::SignHash(hash, key, vchSig)) {
            LogPrintf("CSporkMessage::Sign -- SignHash() failed\n");
            return false;
        }

        if (!CHashSigner::VerifyHash(hash, pubKeyId, vchSig, strError)) {
            LogPrintf("CSporkMessage::Sign -- VerifyHash() failed, error: %s\n", strError);
            return false;
        }
    } else {
        std::string strMessage = boost::lexical_cast<std::string>(nSporkID) + boost::lexical_cast<std::string>(nValue) + boost::lexical_cast<std::string>(nTimeSigned);

        if(!CMessageSigner::SignMessage(strMessage, vchSig, key)) {
            LogPrintf("CSporkMessage::Sign -- SignMessage() failed\n");
            return false;
        }

        if(!CMessageSigner::VerifyMessage(pubKeyId, vchSig, strMessage, strError)) {
            LogPrintf("CSporkMessage::Sign -- VerifyMessage() failed, error: %s\n", strError);
            return false;
        }
    }

    return true;
}


template<typename SporkType, int MsgType, int MinProtocol>
bool CSporkBase<SporkType, MsgType, MinProtocol>::CheckSignature(const CKeyID& pubKeyId) const
{
    std::string strError = "";

    uint256 hash = GetSignatureHash();

    if (!CHashSigner::VerifyHash(hash, pubKeyId, vchSig, strError)) {
        // Note: unlike for many other messages when SPORK_6_NEW_SIGS is ON sporks with sigs in old format
        // and newer timestamps should not be accepted, so if we failed here - that's it
        LogPrintf("CSporkBase::CheckSignature -- VerifyHash() failed, error: %s\n", strError);
        return false;
    }

    return true;
}


bool CSporkMessage::CheckSignature(const CKeyID& pubKeyId) const
{
    std::string strError = "";

    if (sporkManager.IsSporkActive(SPORK_6_NEW_SIGS) &&
        (nTimeSigned >= sporkManager.GetSporkValue(SPORK_6_NEW_SIGS))
    ) {
        uint256 hash = GetSignatureHash();

        if (!CHashSigner::VerifyHash(hash, pubKeyId, vchSig, strError)) {
            // Note: unlike for many other messages when SPORK_6_NEW_SIGS is ON sporks with sigs in old format
            // and newer timestamps should not be accepted, so if we failed here - that's it
            LogPrintf("CSporkMessage::CheckSignature -- VerifyHash() failed, error: %s\n", strError);
            return false;
        }
    } else {
        std::string strMessage = boost::lexical_cast<std::string>(nSporkID) + boost::lexical_cast<std::string>(nValue) + boost::lexical_cast<std::string>(nTimeSigned);

        if (!CMessageSigner::VerifyMessage(pubKeyId, vchSig, strMessage, strError)){
            // Note: unlike for other messages we have to check for new format even with SPORK_6_NEW_SIGS
            // inactive because SPORK_6_NEW_SIGS default is OFF and it is not the first spork to sync
            // (and even if it would, spork order can't be guaranteed anyway).
            uint256 hash = GetSignatureHash();
            if (!CHashSigner::VerifyHash(hash, pubKeyId, vchSig, strError)) {
                LogPrintf("CSporkMessage::CheckSignature -- VerifyHash() failed, error: %s\n", strError);
                return false;
            }
        }
    }

    return true;
}

template<typename SporkType, int MsgType, int MinProtocol>
void CSporkBase<SporkType, MsgType, MinProtocol>::Relay(CConnman& connman)
{
    CInv inv(MsgType, GetHash());
    connman.RelayInv(inv, MinProtocol);
}


