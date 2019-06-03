//-----------------------------------------------------------------------------
// Copyright (c) 2018-2019 The QuantisNet Core developers
//
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
//-----------------------------------------------------------------------------
// This file is assumed to be used as precompiled header to compensate its
// final size and unnecessary load on every compilation unit.
//
// Primary intention is to deal with -Werror issues in dependencies.
//-----------------------------------------------------------------------------
// NOTE: Excessive size may also hurt performance and ccache.
//-----------------------------------------------------------------------------

#ifndef QUANTISNET_DEPS_HPP
#define QUANTISNET_DEPS_HPP

// Standard 
//-----------------------------
//#include <algorithm>
//#include <atomic>
//#include <condition_variable>
//#include <deque>
//#include <exception>
//#include <fstream>
//#include <iomanip>
//#include <list>
#include <map>
//#include <memory>
//#include <queue>
//#include <set>
//#include <sstream>
//#include <stdexcept>
#include <string>
//#include <thread>
//#include <utility>
//#include <vector>

//#include <cassert>
//#include <cmath>
//#include <cstdarg>
//#include <cstdint>
//#include <cstdio>
//#include <cstdlib>


// Boost
//-----------------------------
#pragma GCC diagnostic push

#ifdef WIN32
// Won't work: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53431
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#	pragma GCC diagnostic ignored "-Wshift-negative-value"
// TODO: fix boost errors

//#   define BOOST_USE_WINDOWS_H
//#	include "winsock2.h"
//#	include "windows.h"
#endif

//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/algorithm/string/classification.hpp>
//#include <boost/algorithm/string/predicate.hpp>
//#include <boost/algorithm/string/replace.hpp>
//#include <boost/algorithm/string/split.hpp>
//#include <boost/assert.hpp>
//#include <boost/assign/list_of.hpp>
//#include <boost/bind.hpp> // TODO: change to c++11
//#include <boost/chrono/chrono.hpp> // TODO: change to c++11
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/dynamic_bitset.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/filesystem/fstream.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/foreach.hpp> // TODO: change to c++11
//#include <boost/function.hpp> // TODO: change to c++11
//#include <boost/interprocess/sync/file_lock.hpp>
//#include <boost/iostreams/concepts.hpp>
//#include <boost/iostreams/stream.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/math/distributions/poisson.hpp>
//#include <boost/preprocessor/cat.hpp>
//#include <boost/preprocessor/stringize.hpp>
//#include <boost/program_options/detail/config_file.hpp>
//#include <boost/program_options/parsers.hpp>
//#include <boost/scoped_array.hpp>
//#include <boost/scoped_ptr.hpp>
//#include <boost/shared_ptr.hpp>
#ifndef WIN32
// Produces warning
//#include <boost/signals2/last_value.hpp>
#endif
//#include <boost/signals2/signal.hpp>
#ifndef WIN32
// Produces warning
//#include <boost/thread.hpp> // TODO: change to c++11
#endif
//#include <boost/thread/condition_variable.hpp> // TODO: change to c++11
//#include <boost/thread/exceptions.hpp>
//#include <boost/thread/locks.hpp> // TODO: change to c++11
//#include <boost/thread/mutex.hpp> // TODO: change to c++11
//#include <boost/thread/once.hpp>
//#include <boost/thread/recursive_mutex.hpp>
//#include <boost/tuple/tuple.hpp>
//#include <boost/unordered_map.hpp> // TODO: change to c++11
//#include <boost/unordered_set.hpp> // TODO: change to c++11
//#include <boost/variant.hpp>
//#include <boost/variant/apply_visitor.hpp>
//#include <boost/variant/static_visitor.hpp>



#if BOOST_FILESYSTEM_VERSION >= 3
// TODO: c++11 ??
//#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#endif

#pragma GCC diagnostic pop

// Other
//-----------------------------
//#include <univalue.h>

#endif // QUANTISNET_DEPS_HPP
