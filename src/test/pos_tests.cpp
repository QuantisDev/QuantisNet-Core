//
// Copyright (c) 2019 The QuantisNet Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "spork.h"
#include "utilstrencodings.h"
#include "validation.h"
#include "wallet/wallet.h"
#include "pos_kernel.h"
#include "consensus/consensus.h"
#include "consensus/validation.h"
#include "miner.h"
#include "masternode-sync.h"

#include "test/test_quantisnet.h"

#include <boost/test/unit_test.hpp>

constexpr auto TEST_FIRST_POS_BLOCK = 103U;

struct PoSTestSetup : TestChain100Setup {
    CWallet wallet;
    int64_t mock_time{0};
    int block_shift{0};

    void UpdateMockTime(int block_count = 1) {
        mock_time += block_count * block_shift;
        SetMockTime(mock_time);
    }

    PoSTestSetup() {
#if 0
        const char* args[] = {
            "",
            "-debug=stake",
        };
        ParseParameters(ARRAYLEN(args), args);
        fDebug = true;
        fPrintToConsole = true;
#endif

        CScript scriptPubKey = CScript() <<  ToByteVector(coinbaseKey.GetPubKey()) << OP_CHECKSIG;

        pwalletMain = &wallet;
        RegisterValidationInterface(pwalletMain);

        pwalletMain->AddKeyPubKey(coinbaseKey, coinbaseKey.GetPubKey());
        pwalletMain->ScanForWalletTransactions(chainActive.Genesis(), true);
        pwalletMain->ReacceptWalletTransactions();
        pwalletMain->nStakeSplitThreshold = 1;

        CBitcoinAddress spork_address;
        spork_address.Set(coinbaseKey.GetPubKey().GetID());
        BOOST_CHECK(spork_address.IsValid());

        BOOST_CHECK(sporkManager.SetSporkAddress(spork_address.ToString()));
        BOOST_CHECK(sporkManager.SetPrivKey(CBitcoinSecret(coinbaseKey).ToString()));
        BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, TEST_FIRST_POS_BLOCK, *connman));
        BOOST_CHECK_EQUAL(nFirstPoSBlock, TEST_FIRST_POS_BLOCK);
        //int last_pow_height;

        mock_time = chainActive.Tip()->GetBlockTimeMax() + 5;
        block_shift = pwalletMain->nHashDrift;
        UpdateMockTime(0);

        // PoW mode
        //---
        for (auto i = 2; i > 0; --i) {
            auto blk = CreateAndProcessBlock(CMutableTransactionList(), scriptPubKey);
            BOOST_CHECK(blk.IsProofOfWork());
            UpdateMockTime();
        }

        // PoS mode by spork
        //---
        for (auto i = 30; i > 0; --i) {
            auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());
            BOOST_CHECK(blk.IsProofOfStake());
            BOOST_CHECK(blk.HasStake());
            UpdateMockTime();
        }
    }

    ~PoSTestSetup() {
        UnregisterValidationInterface(pwalletMain);
        pwalletMain = nullptr;
        nFirstPoSBlock = 999999;
        BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, 999999ULL, *connman));
    }
};

BOOST_FIXTURE_TEST_SUITE(PoS_tests, PoSTestSetup)

BOOST_AUTO_TEST_CASE(PoS_transition_test)
{
    // Still, it must continue PoS even after Spork change
    //---
    auto value_bak = sporkManager.GetSporkValue(SPORK_15_FIRST_POS_BLOCK);
    BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, 999999ULL, *connman));
    BOOST_CHECK_EQUAL(nFirstPoSBlock, TEST_FIRST_POS_BLOCK);

    {
        auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());
        BOOST_CHECK(blk.IsProofOfStake());
        BOOST_CHECK(blk.HasStake());
        UpdateMockTime();
    }

    BOOST_CHECK(sporkManager.UpdateSpork(SPORK_15_FIRST_POS_BLOCK, value_bak, *connman));
}

BOOST_AUTO_TEST_CASE(PoS_check_signature)
{
    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    CKey key;
    key.MakeNewKey(true);
    BOOST_CHECK(key.SignCompact(blk.GetHash(), blk.posBlockSig));
    BOOST_CHECK(!CheckProofOfStake(state, blk, Params().GetConsensus()));
    state = CValidationState();
    BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));
}

BOOST_AUTO_TEST_CASE(PoS_check_stake_tx)
{
    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    blk.vtx.erase(blk.vtx.begin() + 1);

    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus())); // Yes, it's TRUE
    BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));
}
    
BOOST_AUTO_TEST_CASE(PoS_check_coinbase) {
    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    CMutableTransaction cb{*(blk.CoinBase())};
    cb.vout[0].scriptPubKey = cb.vout[1].scriptPubKey;
    blk.CoinBase() = MakeTransactionRef(std::move(cb));

    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus())); // Yes, it's TRUE
    BOOST_CHECK(!TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));
}

BOOST_AUTO_TEST_CASE(PoS_unknown_stake) {
    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    blk.posStakeHash = uint256();

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, Params().GetConsensus()));

        int dos = 0;
        BOOST_CHECK(state_fail.IsInvalid(dos));
        BOOST_CHECK(!state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(dos, 100);
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "bad-unkown-stake");
    }

    blk.hashPrevBlock = uint256();

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, Params().GetConsensus()));

        int dos = 0;
        BOOST_CHECK(!state_fail.IsInvalid(dos));
        BOOST_CHECK_EQUAL(dos, 0);
        BOOST_CHECK(state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "tmp-bad-unkown-stake");
    }
}

BOOST_AUTO_TEST_CASE(PoS_mempool_stake) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    CMutableTransaction mempoool_tx;
    blk.posStakeHash = mempoool_tx.GetHash();
    TestMemPoolEntryHelper entry;
    BOOST_CHECK(mempool.addUnchecked(blk.posStakeHash, entry.FromTx(mempoool_tx)));

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, consensus));

        int dos = 0;
        BOOST_CHECK(state_fail.IsInvalid(dos));
        BOOST_CHECK(!state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(dos, 100);
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "bad-stake-mempool");
    }

    blk.hashPrevBlock = uint256();
    
    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, consensus));

        int dos = 0;
        BOOST_CHECK(!state_fail.IsInvalid(dos));
        BOOST_CHECK_EQUAL(dos, 0);
        BOOST_CHECK(state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "tmp-bad-stake-mempool");
    }
}

BOOST_AUTO_TEST_CASE(PoS_beyond_fork_point) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    blk.hashPrevBlock = uint256();;

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, consensus));

        int dos = 0;
        BOOST_CHECK(state_fail.IsInvalid(dos));
        BOOST_CHECK(!state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(dos, 100);
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "bad-prev-header");
    }

    blk.hashPrevBlock = chainActive[0]->GetBlockHash();

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, consensus));

        int dos = 0;
        BOOST_CHECK(state_fail.IsInvalid(dos));
        BOOST_CHECK(!state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(dos, 100);
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "bad-stake-after-fork");
    }
}

BOOST_AUTO_TEST_CASE(PoS_coinbase_maturity) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    UpdateMockTime();

    auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
    auto &blk = *pblk;

    CValidationState state;
    BOOST_CHECK(CheckProofOfStake(state, blk, Params().GetConsensus()));
    BOOST_CHECK(TestBlockValidity(state, Params(), blk, chainActive.Tip(), true, false));

    CBlock maturity_edge;
    auto pindex_maturity_edge = chainActive[chainActive.Height() - COINBASE_MATURITY + 1];
    ReadBlockFromDisk(maturity_edge, pindex_maturity_edge, consensus);
    blk.posStakeHash = maturity_edge.vtx[0]->GetHash();

    {
        CValidationState state_fail;
        BOOST_CHECK(!CheckProofOfStake(state_fail, blk, consensus));

        int dos = 0;
        BOOST_CHECK(state_fail.IsInvalid(dos));
        BOOST_CHECK(!state_fail.IsTransientError());
        BOOST_CHECK_EQUAL(dos, 100);
        BOOST_CHECK_EQUAL(state_fail.GetRejectReason(), "bad-stake-coinbase-maturity");
    }
}

BOOST_AUTO_TEST_CASE(PoS_header_throttle) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    auto out123_0 = COutPoint(uint256S("123"), 0);
    auto out123_1 = COutPoint(uint256S("123"), 1);
    auto out234_0 = COutPoint(uint256S("234"), 0);

    // Should not throttle
    BOOST_CHECK(!IsThottledStakeInput(out123_0));
    BOOST_CHECK(!IsThottledStakeInput(out123_1));
    BOOST_CHECK(!IsThottledStakeInput(out234_0));
    BOOST_CHECK(!IsThottledStakeInput(out123_0));
    BOOST_CHECK(!IsThottledStakeInput(out123_1));
    BOOST_CHECK(!IsThottledStakeInput(out234_0));

    // Throttle #1
    CValidationState state;
    BOOST_CHECK(PassStakeInputThrottle(state, out123_0));
    BOOST_CHECK(PassStakeInputThrottle(state, out123_1));
    BOOST_CHECK(IsThottledStakeInput(out123_0));
    BOOST_CHECK(IsThottledStakeInput(out123_1));
    BOOST_CHECK(!IsThottledStakeInput(out234_0));

    {
        CValidationState fail_state;
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_0));

        int dos = 0;
        BOOST_CHECK(fail_state.IsInvalid(dos));
        BOOST_CHECK_EQUAL(dos, 10);
        BOOST_CHECK_EQUAL(fail_state.GetRejectReason(), "throttle-stake-input");

        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_1));
    }

    // Half-period still throttle
    mock_time += STAKE_INPUT_THROTTLE_PERIOD / 2;
    SetMockTime(mock_time);

    {
        CValidationState fail_state;
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_0));
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_1));
        BOOST_CHECK(PassStakeInputThrottle(fail_state, out234_0));
    }

    mock_time += STAKE_INPUT_THROTTLE_PERIOD / 2;
    SetMockTime(mock_time);
    BOOST_CHECK(PassStakeInputThrottle(state, out123_0));
    BOOST_CHECK(PassStakeInputThrottle(state, out123_1));
    BOOST_CHECK(!PassStakeInputThrottle(state, out234_0));
    {
        CValidationState fail_state;
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_0));
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out123_1));
        BOOST_CHECK(!PassStakeInputThrottle(fail_state, out234_0));
    }

    {
        auto pblk = BlockAssembler(Params()).CreateNewBlock(CScript(), pwalletMain)->block;
        auto &blk = *pblk;

        CValidationState fail_state;
        std::deque<CBlockHeader> headers;
        headers.push_back(blk);
        BOOST_CHECK(ProcessNewBlockHeaders(headers, fail_state, params, NULL));

        blk.hashMerkleRoot = uint256S("1234");
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));
        headers.push_back(blk);
        BOOST_CHECK(!ProcessNewBlockHeaders(headers, fail_state, params, NULL));

        int dos = 0;
        BOOST_CHECK(fail_state.IsInvalid(dos));
        BOOST_CHECK_EQUAL(dos, 10);
        BOOST_CHECK_EQUAL(fail_state.GetRejectReason(), "throttle-stake-input");
    }
}

BOOST_AUTO_TEST_CASE(PoS_old_fork) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    UpdateMockTime();

    auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());

    // Recent fork
    {
        blk.hashPrevBlock = chainActive.Tip()->pprev->GetBlockHash();
        blk.nHeight = chainActive.Height();
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        CValidationState state;
        std::deque<CBlockHeader> headers;
        headers.push_back(blk);
        BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
        BOOST_CHECK_EQUAL(state.GetRejectReason(), "");
    }

    // Advance in time
    mock_time += OLD_POS_BLOCK_AGE_FOR_FORK;
    SetMockTime(mock_time);

    for (auto i = 0; i < CBlockIndex::nMedianTimeSpan; ++i) {
        CreateAndProcessBlock(CMutableTransactionList(), CScript());
    }

    // old fork
    {
        mock_time = chainActive.Tip()->GetMedianTimePast() + MIN_POS_TIP_AGE_FOR_OLD_FORK - 1;
        SetMockTime(mock_time);

        // 1. OK - not synced
        blk.hashMerkleRoot = uint256S("1");
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));
        masternodeSync.Reset();

        {
            CValidationState state;
            std::deque<CBlockHeader> headers;
            headers.push_back(blk);
            BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
            BOOST_CHECK_EQUAL(state.GetRejectReason(), "");

            // Just repeat
            BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
            BOOST_CHECK_EQUAL(state.GetRejectReason(), "");
        }

        // Reset stake seen status
        {
            SetMockTime(mock_time + STAKE_INPUT_THROTTLE_PERIOD + 1);
            CValidationState state;
            PassStakeInputThrottle(state, COutPoint());
            SetMockTime(mock_time);
        }

        // 2. Fail when synced
        blk.hashMerkleRoot = uint256S("2");
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        while (!masternodeSync.IsSynced()) {
            masternodeSync.SwitchToNextAsset(*connman);
        }

        {
            CValidationState state;
            std::deque<CBlockHeader> headers;
            headers.push_back(blk);
            BOOST_CHECK(!ProcessNewBlockHeaders(headers, state, params, NULL));

            int dos = -1;
            BOOST_CHECK(state.IsInvalid(dos));
            BOOST_CHECK_EQUAL(dos, 0);
            BOOST_CHECK_EQUAL(state.GetRejectReason(), "too-old-pos-fork");
        }

        // Reset stake seen status
        {
            SetMockTime(mock_time + (STAKE_INPUT_THROTTLE_PERIOD + 1) * 2);
            CValidationState state;
            PassStakeInputThrottle(state, COutPoint());
            SetMockTime(mock_time);
        }

        // 3. OK - when after threshold
        mock_time += 1;
        SetMockTime(mock_time);

        {
            CValidationState state;
            std::deque<CBlockHeader> headers;
            headers.push_back(blk);
            BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
            BOOST_CHECK_EQUAL(state.GetRejectReason(), "");

            // Just repeat
            BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
            BOOST_CHECK_EQUAL(state.GetRejectReason(), "");
        }

        masternodeSync.Reset();
    }
}

BOOST_AUTO_TEST_CASE(PoS_header_double_spent) {
    auto params = Params();
    auto consensus = params.GetConsensus();

    UpdateMockTime();

    auto blk = CreateAndProcessBlock(CMutableTransactionList(), CScript());
    auto tip = chainActive.Tip();

    UpdateMockTime();
    CreateAndProcessBlock(CMutableTransactionList(), CScript());
    UpdateMockTime();
    CreateAndProcessBlock(CMutableTransactionList(), CScript());

    // Fork case
    {
        mock_time += STAKE_INPUT_THROTTLE_PERIOD;
        SetMockTime(mock_time);

        // First block
        blk.hashPrevBlock = tip->GetBlockHash();
        blk.nHeight = tip->nHeight + 1;
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        CValidationState state;
        std::deque<CBlockHeader> headers;
        headers.push_back(blk);
        BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
        BOOST_CHECK_EQUAL(state.GetRejectReason(), "");

        mock_time += STAKE_INPUT_THROTTLE_PERIOD;
        SetMockTime(mock_time);

        // Second block
        blk.hashPrevBlock = blk.GetHash();
        blk.nHeight++;
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        headers.push_back(blk);
        BOOST_CHECK(!ProcessNewBlockHeaders(headers, state, params, NULL));
        BOOST_CHECK_EQUAL(state.GetRejectReason(), "bad-header-double-spent");
    }

    // Active chain case
    {
        tip = chainActive.Tip();
        mock_time += STAKE_INPUT_THROTTLE_PERIOD;
        SetMockTime(mock_time);

        // First block
        blk.hashPrevBlock = tip->GetBlockHash();
        blk.nHeight = tip->nHeight + 1;
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        CValidationState state;
        std::deque<CBlockHeader> headers;
        headers.push_back(blk);
        BOOST_CHECK(ProcessNewBlockHeaders(headers, state, params, NULL));
        BOOST_CHECK_EQUAL(state.GetRejectReason(), "");

        mock_time += STAKE_INPUT_THROTTLE_PERIOD;
        SetMockTime(mock_time);

        // Second block
        blk.hashPrevBlock = blk.GetHash();
        blk.nHeight++;
        BOOST_CHECK(coinbaseKey.SignCompact(blk.GetHash(), blk.posBlockSig));

        headers.push_back(blk);
        BOOST_CHECK(!ProcessNewBlockHeaders(headers, state, params, NULL));
        BOOST_CHECK_EQUAL(state.GetRejectReason(), "bad-header-double-spent");
    }
}


BOOST_AUTO_TEST_CASE(PoS_abandon_stake) {
    auto params = Params();
    //auto consensus = params.GetConsensus();

    UpdateMockTime();

    // Lock all outputs
    std::vector<COutput> vCoins;
    pwalletMain->AvailableCoins(vCoins);

    for (auto& c : vCoins) {
        pwalletMain->LockCoin(COutPoint(c.tx->GetHash(), c.i));
    }

    pwalletMain->AvailableCoins(vCoins);
    BOOST_CHECK(vCoins.empty());

    {
        CValidationState state;
        InvalidateBlock(state, params, chainActive.Tip());
        BOOST_CHECK(state.IsValid());
    }

    pwalletMain->AvailableCoins(vCoins);
    BOOST_CHECK_EQUAL(vCoins.size(), 1U);

    if (!vCoins.empty()) {
        CreateAndProcessBlock(CMutableTransactionList(), CScript());
    }
}

BOOST_AUTO_TEST_SUITE_END()
