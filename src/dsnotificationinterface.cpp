// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "dsnotificationinterface.h"
#include "instantx.h"
#include "governance.h"
#include "masternodeman.h"
#include "masternode-payments.h"
#include "masternode-sync.h"
#include "privatesend.h"
#ifdef ENABLE_WALLET
#include "privatesend-client.h"
#endif // ENABLE_WALLET

void CDSNotificationInterface::InitializeCurrentBlockTip()
{
    LOCK(cs_main);
    UpdatedBlockTip(chainActive.Tip(), NULL, IsInitialBlockDownload());
}

void CDSNotificationInterface::AcceptedBlockHeader(const CBlockIndex *pindexNew)
{
    masternodeSync.AcceptedBlockHeader(pindexNew);
}

void CDSNotificationInterface::NotifyHeaderTip(const CBlockIndex *pindexNew, bool fInitialDownload)
{
    masternodeSync.NotifyHeaderTip(pindexNew, fInitialDownload, connman);
}

void CDSNotificationInterface::UpdatedBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload)
{
    if (pindexNew == pindexFork) // blocks were disconnected without any new ones
        return;

    masternodeSync.UpdatedBlockTip(pindexNew, fInitialDownload, connman);

    // Update global DIP0001 activation status
    fDIP0001ActiveAtTip = pindexNew->nHeight >= Params().GetConsensus().DIP0001Height;

    if (fInitialDownload)
        return;

    if (fLiteMode)
        return;

    mnodeman.UpdatedBlockTip(pindexNew);
    CPrivateSend::UpdatedBlockTip(pindexNew);
#ifdef ENABLE_WALLET
    privateSendClient.UpdatedBlockTip(pindexNew);
#endif // ENABLE_WALLET
    instantsend.UpdatedBlockTip(pindexNew);
    mnpayments.UpdatedBlockTip(pindexNew, connman);
    governance.UpdatedBlockTip(pindexNew, connman);
}

void CDSNotificationInterface::SyncTransaction(const CTransaction &tx, const CBlockIndex *pindex, int posInBlock)
{
    instantsend.SyncTransaction(tx, pindex, posInBlock);
    CPrivateSend::SyncTransaction(tx, pindex, posInBlock);
}
