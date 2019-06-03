// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Utility functions shared by unit tests
 */
#ifndef BITCOIN_TEST_TESTUTIL_H
#define BITCOIN_TEST_TESTUTIL_H

#pragma GCC diagnostic push

#ifdef _WIN32
#   pragma GCC diagnostic ignored "-Wstack-protector"
#endif

#include <boost/filesystem/path.hpp>

#pragma GCC diagnostic pop

boost::filesystem::path GetTempPath();

#endif // BITCOIN_TEST_TESTUTIL_H
