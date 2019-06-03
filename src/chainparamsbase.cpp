// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2017-2019 The QuantisNet Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparamsbase.h"

#include "tinyformat.h"
#include "util.h"

#include <assert.h>

const std::string CBaseChainParams::MAIN = "main";
const std::string CBaseChainParams::TESTNET = "test";
#ifdef QUANTISNET_ENABLE_TESTNET_60X
const std::string CBaseChainParams::TESTNET60X = "test60";
#endif
const std::string CBaseChainParams::DEVNET = "dev";
const std::string CBaseChainParams::REGTEST = "regtest";

void AppendParamsHelpMessages(std::string& strUsage, bool debugHelp)
{
    strUsage += HelpMessageGroup(_("Chain selection options:"));
    strUsage += HelpMessageOpt("-testnet", _("Use the test chain"));
    #ifdef QUANTISNET_ENABLE_TESTNET_60X
    strUsage += HelpMessageOpt("-testnet60x", _("Use the 60x test chain, which is essentially 60 times faster, in terms of emission and governance"));
    #endif
    strUsage += HelpMessageOpt("-devnet=<name>", _("Use devnet chain with provided name"));
    if (debugHelp) {
        strUsage += HelpMessageOpt("-regtest", "Enter regression test mode, which uses a special chain in which blocks can be solved instantly. "
                                   "This is intended for regression testing tools and app development.");
    }
}

/**
 * Main network
 */
class CBaseMainParams : public CBaseChainParams
{
public:
    CBaseMainParams()
    {
        nRPCPort = 9796;
    }
};
static CBaseMainParams mainParams;

/**
 * Testnet (v1)
 */
class CBaseTestNetParams : public CBaseChainParams
{
public:
    CBaseTestNetParams()
    {
        nRPCPort = 19796;
        strDataDir = "testnet1";
    }
};
static CBaseTestNetParams testNetParams;

#ifdef QUANTISNET_ENABLE_TESTNET_60X
/**
 * Testnet (60x)
 * Represents the 60x faster testnet, in terms of emission and governance testing
 */
class CBaseTestNet60xParams : public CBaseChainParams
{
public:
    CBaseTestNet60xParams()
    {
        nRPCPort = 29796;
        strDataDir = "testnet60x1";
    }
};
static CBaseTestNet60xParams testNet60xParams;
#endif

/**
 * Devnet
 */
class CBaseDevNetParams : public CBaseChainParams
{
public:
    CBaseDevNetParams(const std::string &dataDir)
    {
        nRPCPort = 19998;
        strDataDir = dataDir;
    }
};
static CBaseDevNetParams *devNetParams;


/*
 * Regression test
 */
class CBaseRegTestParams : public CBaseChainParams
{
public:
    CBaseRegTestParams()
    {
        nRPCPort = 39796;
        strDataDir = "regtest";
    }
};
static CBaseRegTestParams regTestParams;

static CBaseChainParams* pCurrentBaseParams = 0;

const CBaseChainParams& BaseParams()
{
    assert(pCurrentBaseParams);
    return *pCurrentBaseParams;
}

CBaseChainParams& BaseParams(const std::string& chain)
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
    } else if (chain == CBaseChainParams::REGTEST)
        return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectBaseParams(const std::string& chain)
{
    if (chain == CBaseChainParams::DEVNET) {
        std::string devNetName = GetDevNetName();
        assert(!devNetName.empty());
        devNetParams = new CBaseDevNetParams(devNetName);
    }

    pCurrentBaseParams = &BaseParams(chain);
}

std::string ChainNameFromCommandLine()
{
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fDevNet = IsArgSet("-devnet");
    bool fTestNet = GetBoolArg("-testnet", false);
    
    int nameParamsCount = (fRegTest ? 1 : 0) + (fDevNet ? 1 : 0) + (fTestNet ? 1 : 0);
    if (nameParamsCount > 1)
        throw std::runtime_error("Only one of -regtest, -testnet or -devnet can be used.");

    #ifdef QUANTISNET_ENABLE_TESTNET_60X
    bool fTestNet60x = GetBoolArg("-testnet60x", false);

    if ((fTestNet && fRegTest && fTestNet60x) || (fTestNet && fRegTest) || (fTestNet60x && fRegTest) || (fTestNet && fTestNet60x))
        throw std::runtime_error("Invalid combination of -regtest, -testnet and/or -testnet60x. Can't be used together.");

    if (fTestNet60x)
        return CBaseChainParams::TESTNET60X;
    #endif
    
    
    if (fDevNet)
        return CBaseChainParams::DEVNET;


    if (fRegTest)
        return CBaseChainParams::REGTEST;
    if (fTestNet)
        return CBaseChainParams::TESTNET;

    return CBaseChainParams::MAIN;
}

std::string GetDevNetName()
{
    // This function should never be called for non-devnets
    assert(IsArgSet("-devnet"));
    std::string devNetName = GetArg("-devnet", "");
    return "devnet" + (devNetName.empty() ? "" : "-" + devNetName);
}

bool AreBaseParamsConfigured()
{
    return pCurrentBaseParams != NULL;
}
