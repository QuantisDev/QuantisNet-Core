// Copyright (c) 2018-2019 The QuantisNet Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <univalue.h>
#include "dag_singleton.h"
#include "validation.h"
#include "crypto/egihash.h"
#include "rpc/server.h"
#include "utilstrencodings.h"

using namespace egihash;

UniValue getepoch(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 0) {
        throw std::runtime_error("getepoch\n"
                                 "\nReturns current epoch number");
    }
    return static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
}

UniValue getseedhash(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 1) {
        throw std::runtime_error("getseedhash\n \"epoch\" "
                                 "\nReturns the hex encoded seedhash for specified epoch n or current epoch if n is not specified"
                                 "\nArguments"
                                 "\n\"epoch\" the epoch number");
    }
    int epoch = 0;
    if (params[0].isNull()) {
        epoch = static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
    } else {
        try {
            epoch = std::stoi(params[0].get_str());
        } catch (const std::invalid_argument& ex) {
            throw std::runtime_error("provided argument \'" + params[0].get_str() + "\' is not an integer");
        }
    }
    return cache_t::get_seedhash(epoch * constants::EPOCH_LENGTH).to_hex();
}

UniValue getdagsize(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 1) {
        throw std::runtime_error("getdagsize\n \"epoch\" "
                                 "\nReturns the size of the DAG in bytes for the specified epoch n or the current epoch if n is not specified"
                                 "\nArguments"
                                 "\n\"epoch\" the epoch number");
    }
    int epoch = 0;
    if (params[0].isNull()) {
        epoch = static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
    } else {
        try {
            epoch = std::stoi(params[0].get_str());
        } catch (const std::invalid_argument& ex) {
            throw std::runtime_error("provided argument \'" + params[0].get_str() + "\' is not an integer");
        }
    }
    return static_cast<uint64_t>(dag_t::get_full_size(epoch * constants::EPOCH_LENGTH));
}

UniValue getdagcachesize(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 1) {
        throw std::runtime_error("getdagcachesize\n \"epoch\" "
                                 "\nReturns the size of the DAG chache in bytes for the specified epoch n or the current epoch if n is not specified"
                                 "\nArguments"
                                 "\n\"epoch\" the epoch number");
    }
    int epoch = 0;
    if (params[0].isNull()) {
        epoch = static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
    } else {
        try {
            epoch = std::stoi(params[0].get_str());
        } catch (const std::invalid_argument& ex) {
            throw std::runtime_error("provided argument \'" + params[0].get_str() + "\' is not an integer");
        }
    }
    return cache_t::get_cache_size(epoch * constants::EPOCH_LENGTH);
}

UniValue getdag(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 1) {
        throw std::runtime_error("getdag\n \"epoch\" "
                                 "\nReturns a JSON object specifying DAG information for the specified epoch n or the current epoch if n is not specified"
                                 "\nArguments"
                                 "\n\"epoch\" the epoch number");
    }
    UniValue result(UniValue::VOBJ);
    int epoch = 0;
    if (params[0].isNull()) {
        epoch = static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
    } else {
        try {
            epoch = std::stoi(params[0].get_str());
        } catch (const std::invalid_argument& ex) {
            throw std::runtime_error("provided argument \'" + params[0].get_str() + "\' is not an integer");
        }
    }
    auto block_num = epoch * constants::EPOCH_LENGTH;
    result.push_back(Pair("epoch", epoch));
    result.push_back(Pair("seedhash", cache_t::get_seedhash(block_num).to_hex()));
    result.push_back(Pair("size", static_cast<uint64_t>(dag_t::get_full_size(block_num))));
    result.push_back(Pair("cache_size", cache_t::get_cache_size(block_num)));
    return result;
}

UniValue getcache(const JSONRPCRequest& request)
{
    auto& params = request.params;

    if (request.fHelp || params.size() > 1) {
        throw std::runtime_error("getcache\n \"epoch\" "
                                 "\nReturns a JSON object specifying DAG cache information for the specified epoch n or the current epoch if n is not specified"
                                 "\nArguments"
                                 "\n\"epoch\" the epoch number");
    }
    UniValue result(UniValue::VOBJ);
    int epoch = 0;
    if (params[0].isNull()) {
        epoch = static_cast<int>(chainActive.Height() / constants::EPOCH_LENGTH);
    } else {
        try {
            epoch = std::stoi(params[0].get_str());
        } catch (const std::invalid_argument& ex) {
            throw std::runtime_error("provided argument \'" + params[0].get_str() + "\' is not an integer");
        }
    }
    auto block_num = epoch * constants::EPOCH_LENGTH;
    result.push_back(Pair("epoch", epoch));
    result.push_back(Pair("seedhash", cache_t::get_seedhash(block_num).to_hex()));
    result.push_back(Pair("size", cache_t::get_cache_size(block_num)));
    return result;
}

UniValue getactivedag(const JSONRPCRequest& request)
{
    if (request.fHelp || request.params.size() > 1) {
        throw std::runtime_error("getactivedag\n"
                                 "\nReturns a JSON list specifying loaded DAG");
    }
    const auto& dag = ActiveDAG();
    if (dag == nullptr) {
        throw std::runtime_error("there is no active dag");
    }
    using namespace egihash;
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("epoch", dag->epoch()));
    result.push_back(Pair("seedhash", cache_t::get_seedhash(dag->epoch() * constants::EPOCH_LENGTH).to_hex()));
    result.push_back(Pair("size", static_cast<uint64_t>(dag->size())));
    return result;
}


static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         okSafeMode
  //  --------------------- ------------------------  -----------------------  ----------
    { "egihash",            "getepoch",               &getepoch,               true,  {}  },
    { "egihash",            "getseedhash",            &getseedhash,            true,  {}  },
    { "egihash",            "getdagsize",             &getdagsize,             true,  {}  },
    { "egihash",            "getdagcachesize",        &getdagcachesize,        true,  {}  },
    { "egihash",            "getdag",                 &getdag,                 true,  {}  },
    { "egihash",            "getcache",               &getcache,               true,  {}  },
    { "egihash",            "getactivedag",           &getactivedag,           true,  {}  },
};

void RegisterEGIHashRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}

