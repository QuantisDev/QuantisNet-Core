// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include "arith_uint256.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"
#include "hdchain.h"

int CChainParams::ExtCoinType(int version) const {
    switch(version) {
    case HDVersion::LEGACY:
        return nLegacyExtCoinType;
    case HDVersion::CURRENT:
    default:
        return nExtCoinType;
    }
}

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint64_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << nBits << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nHeight  = 0;
    genesis.hashMix.SetNull();
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint64_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Quantis Network  www.QuantisNetwork.com 4 / 2019";
    const CScript genesisOutputScript = CScript() << ParseHex("0479619b3615fc9f03aace413b9064dc97d4b6f892ad541e5a2d8a3181517443840a79517fb1a308e834ac3c53da86de69a9bcce27ae01cf77d9b2b9d7588d122a") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.DevFeeAddr = "QQjxNmKFMaZZAhRtKwuENQM8mXnHs1QGmB";
        // 40% of the total annual emission of ~12M goes to the treasury
        // which is around 4.8M / 26.07 ~ 184,000, where 26.07 are the
        // number of super blocks per year according to the 20160 block cycle
        consensus.nSuperblockCycle = 28800; //per 30 days
        consensus.nMasternodePaymentsStartBlock = 2500; // should be about 2.5k blocks after genesis
        consensus.nInstantSendKeepLock = 24;
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nGovernanceMinQuorum = 30; //30% acceptance per proposal to approve
        consensus.nGovernanceFilterElements = 20000;

        consensus.nMasternodeMinimumConfirmations = 15;
        consensus.BIP34Height = 1; // since genesis
        consensus.BIP65Height = 1; // since genesis
        consensus.BIP66Height = 1; // since genesis
        consensus.DIP0001Height = 1; // since genesis
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24

        consensus.nPowTargetTimespan = 24 * 60 * 60; // QuantisNet: 1 day
        consensus.nPowTargetSpacing = 90; 
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 27;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1486252800; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1556013600; // Apr 23th, 2019

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1508025600; // Oct 15th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1556013600; // Apr 23th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 3226; // 80% of 4032

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1546300800; // Jan 1st, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1556013600; // Apr 23th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 3226; // 80% of 4032

        // The best chain should have at least this much work.This helps new and existing users not connect to a forked chain
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000086c17a618e2657e79");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xbbf1e935d4999e83b0f52b154e418119bdc71666fa3f58700a0a825b8b33eddd");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbc;
        pchMessageStart[2] = 0xba;
        pchMessageStart[3] = 0xd4;
        vAlertPubKey = ParseHex("048cd9adbefe1ca8435de5372e2725027e56f959fb979f5252c7d2a51de2f5251c10d55ad632e8c217d086b7b517ccfa934d5af693f354a0ab58bce23c963df5fc");
        nDefaultPort = 9801;
        nPruneAfterHeight = 100000;

	// genesis params

	    uint32_t nGenesisTime = 1559435552;
        uint32_t nNonce = 1121235;
        genesis = CreateGenesisBlock(nGenesisTime, nNonce, 0x1e0ffff0, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        vSeeds.push_back(CDNSSeedData("qSeed1", "144.202.47.177"));
        vSeeds.push_back(CDNSSeedData("qSeed2", "45.63.104.138"));
        vSeeds.push_back(CDNSSeedData("qSeed3", "149.28.110.229"));
        // QuantisNet addresses start with 'Q'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,58);
        // QuantisNet script addresses start with 'N'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,53);
        // QuantisNet private keys start with 'G'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,106);
        // QuantisNet BIP32 pubkeys start with 'npub'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x03)(0xB8)(0xC8)(0x56).convert_to_container<std::vector<unsigned char> >();
        // QuantisNet BIP32 prvkeys start with 'nprv'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0xD7)(0xDC)(0x6E)(0x9F).convert_to_container<std::vector<unsigned char> >();

        // QuantisNet BIP44/SLIP44 coin type is '9797'
        nExtCoinType = 9797;
        // Legacy inherited from Dash
        nLegacyExtCoinType = 5;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        // See https://github.com/dashpay/dash/pull/1969
        strSporkAddress = "Qgfqm9eXVCDwPFuTRsJ7qm6qb21zW23cST";

        nStakeMinAge = 5 * 60; //Set as 5 mins during testing,make it 24 hours when mainnet
//Add checkpoints constantly on each new update,to keep all nodes on proper chain later
        checkpointData = {
          {
            {  0, uint256(genesis.GetHash()) },
          }
        };

        chainTxData = ChainTxData{
            1558042760,     // * UNIX timestamp of last known number of transactions
            1157185,         // * total number of transactions between genesis and that timestamp
                            //   (the tx=... number in the SetBestChain debug.log lines)
            500          // * estimated number of transactions per second after that timestamp
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v1)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";

    
        // 40% of the total annual emission of ~12M goes to the treasury
        // which is around 4.8M / 26.07 ~ 184,000, where 26.07 are the
        // number of super blocks per year according to the 180 block cycle
        consensus.nSuperblockCycle = 180;

        consensus.nMasternodePaymentsStartBlock = 17630; // should be about 15 days after genesis
        consensus.nInstantSendKeepLock = 6;
        consensus.nInstantSendConfirmationsRequired = 2;

        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 4
        consensus.nPowTargetTimespan = 24 * 60 * 60; // in seconds -> QuantisNet: 1 day
        consensus.nPowTargetSpacing = 60; // in seconds QuantisNet: 1 minute
        consensus.BIP34Height = 1; //
        consensus.BIP65Height = 1; //
        consensus.BIP66Height = 1; //
        consensus.DIP0001Height = 1; //
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 27;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1486252800; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1549328400; // Feb 5th, 2019

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1505692800; // Sep 18th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1549328400; // Feb 5th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1546300800; // Jan 1st, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1549328400; // Feb 5th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000073661d6780eafb");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x9b5e54cd8b8634127bee5f57038a77d887947a11934427fea28ae38d74502508");

        pchMessageStart[0] = 0xd9;
        pchMessageStart[1] = 0x2a;
        pchMessageStart[2] = 0xab;
        pchMessageStart[3] = 0x6e;
        vAlertPubKey = ParseHex("04da7109a0215bf7bb19ecaf9e4295104142b4e03579473c1083ad44e8195a13394a8a7e51ca223fdbc5439420fd08963e491007beab68ac65c5b1c842c8635b37");
        nDefaultPort = 19797;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1524344801, 16880322, 0x207fffff, 1,0);
        consensus.hashGenesisBlock = genesis.GetHash();

     //   assert(consensus.hashGenesisBlock == expectedGenesisHash);
     //   assert(genesis.hashMerkleRoot == expectedGenesisMerkleRoot);

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("test.quantisnet.network", "dnsseed.test.quantisnet.network"));

        // Testnet QuantisNet addresses start with 't'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        // Testnet QuantisNet script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet QuantisNet BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet QuantisNet BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Dedicated QUAN testnet for BIP44
        nExtCoinType = 19797;
        // BIP44 test coin type is '1' (All coin's testnet default)
        nLegacyExtCoinType = 1;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        // See https://github.com/dashpay/dash/pull/1969
        // QuantisNet prefix: Base58 't' = 127 = 0x7F
        strSporkAddress = "tCLzFoAUkWyrDJmU3qvcKpSA41aD6AckwL";

        nStakeMinAge = 180;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  1000, uint256S("0x48357913ab6aeff3ac5d8a7120cdf991ca7b598f40c30efbc66b32ce343c8596"))
            (  5000, uint256S("0x50d6318ae28e2d46d3aa5ecb4a7566ec3e9f8b9542e9a84a744d3c8eb815f405"))
            (  9000, uint256S("0x263bb5d663abbbff11318d82c93249c63523f6b48535f81acf194e45e353be59"))
            (  17606, uint256S("0xa3a707f57db9100fcc949609394c2ab164e51892f1b3810bbedad8f9cfb87f91"))
            (  20000, uint256S("0x8085fb36bd44f4e238b10f948f9c944f2185f93164f567e43f477097dad59dda"))
            (  25000, uint256S("0x59049b920a2cbf4f61fce77ef3341ff9a393e0b70b734cc5812902dd105f4de8"))
            (  31000, uint256S("0xf2b400d4d516ac50cfb806d365e26e55c4e216e21f678f3e60405942bf266409"))
            (  45000, uint256S("0xb4f8601acbca2073fde7691c58145886534ae4c6806ccdf3bfff77d3fa6acbaf"))
            (  47000, uint256S("0x83c326375f94cfa64dfd9f6250259f3bff8e744d8f8a0c53546abe205f6faa45"))
            (  47170, uint256S("0xb7ac4454f458096ab3510e408d6c8c7cb2214ecbda019a3019482922f8c083e2"))
            (  57000, uint256S("0x9b5e54cd8b8634127bee5f57038a77d887947a11934427fea28ae38d74502508"))
            (  74741, uint256S("0xead94a1aa85edc4318c0e1947dd88fa64935c86be540c5810023d8f0b86dadac"))
            (  86000, uint256S("0x75951ec8094fc01ab2be643eb48cc10638690470b8550d7dd8bf849eaafff7bc"))

            // Blacklist
            ,
            {}
        };

        chainTxData = ChainTxData{
            1551396162,     // * UNIX timestamp of last checkpoint block
            127679,         // * total number of transactions between genesis and last checkpoint
                            //   (the tx=... number in the SetBestChain debug.log lines)
            0.017           // * estimated number of transactions per second after that timestamp
        };

    }
};
static CTestNetParams testNetParams;

#ifdef QUANTISNET_ENABLE_TESTNET_60X
/**
 * Testnet (60x)
 */
class CTestNet60xParams : public CChainParams {
public:
    CTestNet60xParams() {
        strNetworkID = "test60";

        // QuantisNet distribution parameters
        // Seeing as there are 526,000 blocks per year, and there is a 12M annual emission
        // masternodes get 40% of all coins or 4.8M / 526,000 ~ 9.14
        // miners get 10% of all coins or 1.2M / 526,000 ~ 2.28
        // backbone gets 10% of all coins or 1.2M / 526,000 ~ 2.28
        // which adds up to 13.7 as block subsidy

        // 40% of the total annual emission of ~12M goes to the treasury
        // which is around 4.8M / 26.07 ~ 184,000, where 26.07 are the
        // number of super blocks per year according to the 20160 block cycle
        consensus.nSuperblockCycle = 60;

        consensus.nMasternodePaymentsStartBlock = 216000 / 60;
        consensus.nInstantSendKeepLock = 6;

        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPowTargetTimespan = 24 * 60 * 60; // in seconds -> QuantisNet: 1 day
        consensus.nPowTargetSpacing = 60; // in seconds QuantisNet: 1 minute
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 27;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1486252800; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1517788800; // Feb 5th, 2018

        pchMessageStart[0] = 0xd9;
        pchMessageStart[1] = 0x2a;
        pchMessageStart[2] = 0xab;
        pchMessageStart[3] = 0x60; // Changed the last byte just in case, even though the port is different too, so shouldn't mess with the general testnet
        vAlertPubKey = ParseHex("04da7109a0215bf7bb19ecaf9e4295104142b4e03579473c1083ad44e8195a13394a8a7e51ca223fdbc5439420fd08963e491007beab68ac65c5b1c842c8635b37");
        nDefaultPort = 29797;
        nMaxTipAge = 0x7fffffff; // allow mining on top of old blocks for testnet
        nDelayGetHeadersTime = 24 * 60 * 60;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(nGenesisTime, nNonce, 0x1e0ffff0, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("test60x.quantisnet.network",  "dnsseed.test60x.quantisnet.network"));

        // Testnet QuantisNet addresses start with 't'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        // Testnet QuantisNet script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet QuantisNet BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet QuantisNet BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Dedicated QUAN testnet for BIP44
        nExtCoinType = 19797;
        // BIP44 test coin type is '1' (All coin's testnet default)
        nLegacyExtCoinType = 1;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test60x, pnSeed6_test60x + ARRAYLEN(pnSeed6_test60x));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        // See https://github.com/dashpay/dash/pull/1969
        // QuantisNet prefix: Base58 't' = 127 = 0x7F
        strSporkAddress = "tCLzFoAUkWyrDJmU3qvcKpSA41aD6AckwL";

        nStakeMinAge = 180;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x440cbbe939adba25e9e41b976d3daf8fb46b5f6ac0967b0a9ed06a749e7cf1e2")),
            0, // * UNIX timestamp of last checkpoint block
            0,     // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0         // * estimated number of transactions per day after checkpoint
        };

    }
};
static CTestNet60xParams testNet60xParams;
#endif

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        strNetworkID = "dev";

        // QuantisNet distribution parameters


        // 40% of the total annual emission of ~12M goes to the treasury
        // which is around 4.8M / 26.07 ~ 184,000, where 26.07 are the
        // number of super blocks per year according to the 20160 block cycle
        consensus.nSuperblockCycle = 60;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on devnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 1; // BIP34 activated immediately on devnet
        consensus.BIP65Height = 1; // BIP65 activated immediately on devnet
        consensus.BIP66Height = 1; // BIP66 activated immediately on devnet
        consensus.DIP0001Height = 2; // DIP0001 activated immediately on devnet
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.posLimit = uint256S("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 4
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Dash: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Important for PoW test
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 27;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1506556800; // September 28th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1549328400; // Feb 5th, 2019

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 1505692800; // Sep 18th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 1549328400; // Feb 5th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nThreshold = 50; // 50% of 100

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 1517792400; // Feb 5th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 1549328400; // Feb 5th, 2019
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nWindowSize = 100;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nThreshold = 50; // 50% of 100

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        pchMessageStart[0] = 0xe2;
        pchMessageStart[1] = 0xca;
        pchMessageStart[2] = 0xff;
        pchMessageStart[3] = 0xce;
        vAlertPubKey = ParseHex("04517d8a699cb43d3938d7b24faaff7cda448ca4ea267723ba614784de661949bf632d6304316b244646dea079735b9a6fc4af804efb4752075b9fe2245e14e412");
        nDefaultPort = 19797;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1417713337, 1096447, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();

        // Testnet QuantisNet addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
        // Testnet QuantisNet script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet QuantisNet BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet QuantisNet BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Testnet QuantisNet BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        // See for instructions https://github.com/dashpay/dash/pull/1969
        // privKey: cP4EKFyJsHT39LDqgdcB43Y3YXjNyjb5Fuas1GQSeAtjnZWmZEQK
        strSporkAddress = "yj949n1UH6fDhw6HtVE5VMj2iSTaSWBMcW";

        nStakeMinAge = 0;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (      0, uint256S("0x000008ca1832a4baf228eb1553c03d3a2c8e02399550dd6ea8d65cec3ef23d2e"))

            // Blacklist
            ,
            {}
        };

        chainTxData = ChainTxData{
            1417713337,                   // * UNIX timestamp of devnet genesis block
            2,                            // * we only have 2 coinbase transactions when a devnet is started up
            0.01                          // * estimated number of transactions per second
        };
    }
};
static CDevNetParams *devNetParams;


/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSuperblockCycle = 60;
        consensus.nMasternodePaymentsStartBlock = 240;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.DIP0001Height = 2000;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.posLimit = uint256S("0fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 4
        consensus.nPowTargetTimespan = 24 * 60 * 60; // QuantisNet: 1 day
        consensus.nPowTargetSpacing = 60; // QuantisNet: 1 minute
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 27;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xef;
        pchMessageStart[1] = 0x89;
        pchMessageStart[2] = 0x6c;
        pchMessageStart[3] = 0x7f;
        nDefaultPort = 39797;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1524279488, 12, 0x207fffff, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        // See https://github.com/dashpay/dash/pull/1969
        // QuantisNet prefix: Base58 't' = 127 = 0x7F
        // privKey: cP6fF1kWCPWWWDjKpFzkCPJL6rUWVhErc2uBxBMivLv7fRRDcDBK
        strSporkAddress = "tCri6YknwQ4wnUQGxqDxSb6hdMaU7rPR3z";

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x378abe3d42888769177494063edd42e6c3925e938ff8f73c71a6b6ad5b293ea7"))

            // Blacklist
            ,
            {}
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };
        // Testnet QuantisNet addresses start with 't'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127);
        // Testnet QuantisNet script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet QuantisNet BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet QuantisNet BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Dedicated QUAN testnet for BIP44
        nExtCoinType = 19797;
        // BIP44 test coin type is '1' (All coin's testnet default)
        nLegacyExtCoinType = 1;

        nStakeMinAge = 0;
   }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
#ifdef QUANTISNET_ENABLE_TESTNET_60X
    else if (chain == CBaseChainParams::TESTNET60X)
            return testNet60xParams;
#endif
    else if (chain == CBaseChainParams::DEVNET) {
            assert(devNetParams);
            return *devNetParams;
    }
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    if (network == CBaseChainParams::DEVNET) {
        devNetParams = new CDevNetParams();
    }

    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
