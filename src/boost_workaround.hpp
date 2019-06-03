//-----------------------------------------------------------------------------
// Copyright (c) 2018-2019 The QuantisNet Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//-----------------------------------------------------------------------------
// Current boost dep produces warnings on MinGW builds.
//-----------------------------------------------------------------------------

#ifndef QUANTISNET_BOOST_WIN32_HPP
#define QUANTISNET_BOOST_WIN32_HPP

#pragma GCC diagnostic push

#ifndef __clang__
    // General GCC+Boost issue
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#ifdef WIN32
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#	pragma GCC diagnostic ignored "-Wshift-negative-value"
#	pragma GCC diagnostic ignored "-Wstack-protector"
#endif

#include <boost/integer/integer_log2.hpp>
#include <boost/thread.hpp> // TODO: change to c++11
#include <boost/signals2/last_value.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem.hpp>

#pragma GCC diagnostic pop

#endif // QUANTISNET_BOOST_WIN32_HPP
