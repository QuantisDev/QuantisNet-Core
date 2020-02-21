// Copyright (c) 2017-2019 The QuantisNet Core developers
// Distributed under the MIT software license, see the accompanying
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORK_H
#define SPORK_H

#include "hash.h"
#include "net.h"
#include "utilstrencodings.h"
#include "key.h"
#include "script/script.h"

class CSporkMessage;
class CSporkManager;
class CSporkCheckpoint;
class CSporkBlacklist;
using SporkCheckpointMap = std::map<uint256, CSporkCheckpoint>;
using SporkBlacklistMap = std::map<uint256, CSporkBlacklist>;


/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/
static const int SPORK_2_INSTANTSEND_ENABLED                            = 10001;
static const int SPORK_3_INSTANTSEND_BLOCK_FILTERING                    = 10002;
static const int SPORK_5_INSTANTSEND_MAX_VALUE                          = 10004;
static const int SPORK_6_NEW_SIGS                                       = 10005;
static const int SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT                 = 10007;
static const int SPORK_9_SUPERBLOCKS_ENABLED                            = 10008;
static const int SPORK_10_MASTERNODE_PAY_UPDATED_NODES                  = 10009;
static const int SPORK_12_RECONSIDER_BLOCKS                             = 10011;
static const int SPORK_14_REQUIRE_SENTINEL_FLAG                         = 10013;
static const int SPORK_15_FIRST_POS_BLOCK                               = 10014;
static const int SPORK_16_MASTERNODE_MIN_PROTOCOL                       = 10015;
static const int SPORK_17_NEWPROTO_ENFORCE                              = 10016;
static const int SPORK_18_DISABLE_IPV6_MNS                              = 10017;
static const int SPORK_19_BLACKLIST_ENABLED                             = 10018;
static const int SPORK_20_STAKEMINAGEV2                                 = 10019;

static const int SPORK_START                                            = SPORK_2_INSTANTSEND_ENABLED;
static const int SPORK_END                                              = SPORK_20_STAKEMINAGEV2;

extern std::map<int, int64_t> mapSporkDefaults;
extern std::map<uint256, CSporkMessage> mapSporks;
extern SporkCheckpointMap mapSporkCheckpoints;
extern SporkBlacklistMap mapSporkBlacklist;
extern CSporkManager sporkManager;

//
// Spork classes
// Keep track of all of the network spork settings
//

template<typename SporkType, int MsgType, int MinProtocol=MIN_PEER_PROTO_VERSION_BEFORE_ENFORCEMENT>
class CSporkBase {
protected:
    std::vector<unsigned char> vchSig;

public:
    int64_t nTimeSigned{0};

    CSporkBase(int64_t nTimeSigned) : nTimeSigned(nTimeSigned) {}
    CSporkBase() = default;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    void SerializationOp(Stream& s, Operation ser_action);

    bool operator==(const SporkType& other) const {
        return vchSig == other.vchSig;
    }

    uint256 GetHash() const;
    uint256 GetSignatureHash() const;

    bool Sign(const CKey& key);
    bool CheckSignature(const CKeyID& pubKeyId) const;
    void Relay(CConnman& connman);
};

class CSporkMessage :
    public CSporkBase<CSporkMessage, MSG_SPORK>
{
public:
    int nSporkID{0};
    int64_t nValue{0};

    CSporkMessage(int nSporkID, int64_t nValue, int64_t nTimeSigned) :
        CSporkBase(nTimeSigned),
        nSporkID(nSporkID),
        nValue(nValue)
        {}

    CSporkMessage() = default;

    template <typename Stream, typename Operation>
    inline void DataSerialization(Stream& s, Operation ser_action) {
        READWRITE(nSporkID);
        READWRITE(nValue);    
    }

    bool Sign(const CKey& key);
    bool CheckSignature(const CKeyID& pubKeyId) const;
};

class CSporkCheckpoint :
    public CSporkBase<CSporkCheckpoint, MSG_CHECKPOINT, SPORK_CHECKPOINT_VERSION>
{
public:
    static constexpr auto MAX_AGE = 30 * 24 * 60 * 60; // one month

    int nHeight{0};
    uint256 hashBlock;

    CSporkCheckpoint(int nHeight, const uint256& hashBlock, int64_t nTimeSigned) :
        CSporkBase(nTimeSigned),
        nHeight(nHeight),
        hashBlock(hashBlock)
        {}

    CSporkCheckpoint() = default;

    template <typename Stream, typename Operation>
    inline void DataSerialization(Stream& s, Operation ser_action) {
        READWRITE(nHeight);
        READWRITE(hashBlock);
    }
};


class CSporkBlacklist :
    public CSporkBase<CSporkBlacklist, MSG_BLACKLIST, SPORK_BLACKLIST_VERSION>
{
public:
    static constexpr auto MAX_AGE = 3 * 30 * 24 * 60 * 60; // three months
    static constexpr auto DISABLED_SINCE = -1;

    CScript scriptPubKey;
    int64_t nTimeSince{0};

    CSporkBlacklist(const CScript &scriptPubKey, int64_t nTimeSince, int64_t nTimeSigned) :
        CSporkBase(nTimeSigned),
        scriptPubKey(scriptPubKey),
        nTimeSince(nTimeSince)
        {}

    CSporkBlacklist() = default;

    template <typename Stream, typename Operation>
    inline void DataSerialization(Stream& s, Operation ser_action) {
        READWRITE(static_cast<CScriptBase&>(scriptPubKey));
        READWRITE(nTimeSince);
    }
};

template<typename SporkType, int MsgType, int MinProtocol>
template<typename Stream, typename Operation>
void CSporkBase<SporkType, MsgType, MinProtocol>::SerializationOp(Stream& s, Operation ser_action) {
    auto impl = static_cast<SporkType*>(this);
    impl->template DataSerialization<Stream, Operation>(s, ser_action);

    READWRITE(nTimeSigned);

    if (!(s.GetType() & SER_GETHASH)) {
        READWRITE(vchSig);
    }
}

class CSporkManager
{
public:
    using ActiveCheckpointMap = std::map<int, CSporkCheckpoint>;
    using ActiveBlacklistMap = std::map<CScript, CSporkBlacklist>;

private:
    std::vector<unsigned char> vchSig;
    std::map<int, CSporkMessage> mapSporksActive;
    ActiveCheckpointMap mapCheckpointsActive;
    ActiveBlacklistMap mapBlacklistActive;

    CKeyID sporkPubKeyID;
    CKey sporkPrivKey;

public:

    CSporkManager() {}

    void ProcessSpork(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman);
    void ExecuteSpork(int nSporkID, int nValue);
    void ExecuteCheckpoint(int height, const uint256& block_hash);
    void ExecuteBlacklist(const CScript &scriptPubKey, int64_t nTimeSince);
    bool UpdateSpork(int nSporkID, int64_t nValue, CConnman& connman);
    bool UpdateCheckpoint(int height, const uint256& block_hash, CConnman& connman);
    bool UpdateBlacklist(const CScript &scriptPubKey, int64_t nTimeSince, CConnman& connman);

    bool IsSporkActive(int nSporkID);
    int64_t GetSporkValue(int nSporkID);
    int GetSporkIDByName(const std::string& strName);
    std::string GetSporkNameByID(int nSporkID);

    bool SetSporkAddress(const std::string& strAddress);
    bool SetPrivKey(const std::string& strPrivKey);

    ActiveCheckpointMap GetActiveCheckpoints() const;
    ActiveBlacklistMap GetActiveBlacklists() const;
};

#endif
