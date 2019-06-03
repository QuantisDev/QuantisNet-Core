// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2017 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "checkpoints.h"

#include "chain.h"
#include "chainparams.h"
#include "validation.h"
#include "uint256.h"

#include <stdint.h>

#include <boost/foreach.hpp>


namespace Checkpoints {

    CBlockIndex* GetLastSeenCheckpoint(const CCheckpointData& data)
    {
        const MapCheckpoints& checkpoints = data.mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            BlockMap::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    //! Validate block hash against checkpoint, if any
    bool ValidateCheckpoint(const CCheckpointData& data, int height, const uint256 &hash)
    {
        auto& checkpoints = data.mapCheckpoints;
        auto checkp = checkpoints.find(height);

        // Not a checkoint or hash matches
        return ((checkp == checkpoints.end()) || (checkp->second == hash));
    }
} // namespace Checkpoints
