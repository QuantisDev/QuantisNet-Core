# Copyright (c) 2013-2016 The Bitcoin Core developers
# Copyright (c) 2014-2018 The Dash Core developers
# Copyright (c) 2017-2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CFLAGS = $(HARDENED_CXXFLAGS) $(WARN_CFLAGS)
AM_CXXFLAGS = $(HARDENED_CXXFLAGS) $(WARN_CXXFLAGS)
AM_CPPFLAGS = $(HARDENED_CPPFLAGS)
EXTRA_LIBRARIES =


# Quick precompiled header support, TODO: revise
export CCACHE_SLOPPINESS = pch_defines,time_macros

if ENABLE_PCH

if IS_GCC
QUANTISNET_PCH_DIR = $(builddir)/quantisnet_all.hpp.gch
QUANTISNET_PCH = $(QUANTISNET_PCH_DIR).stamp
QUANTISNET_PCH_CPPFLAGS = -fpch-preprocess -include $(builddir)/quantisnet_all.hpp
export CCACHE_IGNOREHEADERS = $(QUANTISNET_PCH_DIR)

$(QUANTISNET_PCH): $(srcdir)/quantisnet_all.hpp
	$(MKDIR_P) $(QUANTISNET_PCH_DIR)
	-rm -f $(QUANTISNET_PCH_DIR)/*.gch
	$(CXXCOMPILE) -x c++-header $(BITCOIN_INCLUDES) $(BITCOIN_QT_INCLUDES) \
	  $(QT_INCLUDES) $(QT_DBUS_INCLUDES) $(PROTOBUF_CFLAGS) $(QR_CFLAGS) \
	  $(QT_PIE_FLAGS) $(PIC_FLAGS) \
	  $(srcdir)/quantisnet_all.hpp -o $(QUANTISNET_PCH_DIR)/qt.gch
	$(CXXCOMPILE) -x c++-header $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(PIE_FLAGS) \
	  $(srcdir)/quantisnet_all.hpp -o $(QUANTISNET_PCH_DIR)/main.gch
	test "$$CCACHE_SLOPPINESS" = "$(CCACHE_SLOPPINESS)"
	touch $(QUANTISNET_PCH)
endif

if IS_CLANG
QUANTISNET_PCH = $(builddir)/quantisnet_all.hpp.pch
QUANTISNET_PCH_CPPFLAGS = -include-pch $(QUANTISNET_PCH)
export CCACHE_IGNOREHEADERS = $(QUANTISNET_PCH)

$(QUANTISNET_PCH): $(srcdir)/quantisnet_all.hpp
	$(CXXCOMPILE) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(PIE_FLAGS) \
	  -emit-pch $(srcdir)/quantisnet_all.hpp -o $(QUANTISNET_PCH)
	test "$$CCACHE_SLOPPINESS" = "$(CCACHE_SLOPPINESS)"
endif

else
QUANTISNET_PCH =
QUANTISNET_PCH_CPPFLAGS =
endif

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

BITCOIN_CONFIG_INCLUDES=-I$(builddir)/config
BITCOIN_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

BITCOIN_INCLUDES += -I$(srcdir)/secp256k1/include
BITCOIN_INCLUDES += $(UNIVALUE_CFLAGS)

LIBBITCOIN_SERVER=libquantisnet_server.a
LIBBITCOIN_COMMON=libquantisnet_common.a
LIBBITCOIN_CONSENSUS=libquantisnet_consensus.a
LIBBITCOIN_CLI=libquantisnet_cli.a
LIBBITCOIN_UTIL=libquantisnet_util.a
LIBBITCOIN_CRYPTO=crypto/libquantisnet_crypto.a
LIBBITCOINQT=qt/libquantisnetqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBBITCOIN_ZMQ=libquantisnet_zmq.a
endif
if BUILD_BITCOIN_LIBS
LIBBITCOINCONSENSUS=libquantisnetconsensus.la
endif
if ENABLE_WALLET
LIBBITCOIN_WALLET=libquantisnet_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_CONSENSUS) \
  $(LIBBITCOIN_SERVER) \
  $(LIBBITCOIN_CLI) \
  $(LIBBITCOIN_WALLET) \
  $(LIBBITCOIN_ZMQ)

lib_LTLIBRARIES = $(LIBBITCOINCONSENSUS)

bin_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_BITCOIND
  bin_PROGRAMS += quantisnetd
endif

if BUILD_BITCOIN_UTILS
  bin_PROGRAMS += quantisnet-cli quantisnet-tx
endif

.PHONY: FORCE check-symbols check-security
# quantisnet core #
BITCOIN_CORE_H = \
  addrdb.h \
  activemasternode.h \
  addressindex.h \
  spentindex.h \
  addrman.h \
  alert.h \
  base58.h \
  boost_workaround.hpp \
  bip39.h \
  bip39_english.h \
  blockencodings.h \
  bloom.h \
  cachemap.h \
  cachemultimap.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  core_io.h \
  core_memusage.h \
  cuckoocache.h \
  quantisnet_all.hpp \
  quantisnet_deps.hpp \
  privatesend.h \
  privatesend-client.h \
  privatesend-server.h \
  privatesend-util.h \
  dsnotificationinterface.h \
  governance.h \
  governance-classes.h \
  governance-exceptions.h \
  governance-object.h \
  governance-validators.h \
  governance-vote.h \
  governance-votedb.h \
  flat-database.h \
  hdchain.h \
  httprpc.h \
  httpserver.h \
  indirectmap.h \
  init.h \
  instantx.h \
  key.h \
  keepass.h \
  keystore.h \
  dbwrapper.h \
  limitedmap.h \
  masternode.h \
  masternode-payments.h \
  masternode-sync.h \
  masternodeman.h \
  masternodeconfig.h \
  memusage.h \
  merkleblock.h \
  messagesigner.h \
  miner.h \
  net.h \
  net_processing.h \
  netaddress.h \
  netbase.h \
  netfulfilledman.h \
  netmessagemaker.h \
  noui.h \
  policy/fees.h \
  policy/policy.h \
  policy/rbf.h \
  pos_kernel.h \
  pow.h \
  protocol.h \
  random.h \
  reverselock.h \
  rpc/client.h \
  rpc/protocol.h \
  rpc/server.h \
  rpc/register.h \
  scheduler.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/ismine.h \
  spork.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/events.h \
  support/lockedpool.h \
  sync.h \
  threadsafety.h \
  threadinterrupt.h \
  timedata.h \
  torcontrol.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utiltime.h \
  validation.h \
  validationinterface.h \
  versionbits.h \
  wallet/coincontrol.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/rpcwallet.h \
  wallet/wallet.h \
  wallet/walletdb.h \
  warnings.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h\
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h


obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libquantisnet_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between quantisnetd and quantisnet-qt
libquantisnet_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) \
  $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_server_a_SOURCES = \
  activemasternode.cpp \
  addrman.cpp \
  addrdb.cpp \
  alert.cpp \
  bloom.cpp \
  blockencodings.cpp \
  chain.cpp \
  checkpoints.cpp \
  dsnotificationinterface.cpp \
  httprpc.cpp \
  httpserver.cpp \
  init.cpp \
  instantx.cpp \
  dbwrapper.cpp \
  governance.cpp \
  governance-classes.cpp \
  governance-object.cpp \
  governance-validators.cpp \
  governance-vote.cpp \
  governance-votedb.cpp \
  masternode.cpp \
  masternode-payments.cpp \
  masternode-sync.cpp \
  masternodeconfig.cpp \
  masternodeman.cpp \
  merkleblock.cpp \
  messagesigner.cpp \
  miner.cpp \
  net.cpp \
  netfulfilledman.cpp \
  net_processing.cpp \
  noui.cpp \
  policy/fees.cpp \
  policy/policy.cpp \
  pow.cpp \
  privatesend.cpp \
  privatesend-server.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/masternode.cpp \
  rpc/governance.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  script/ismine.cpp \
  sendalert.cpp \
  spork.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  txmempool.cpp \
  ui_interface.cpp \
  validation.cpp \
  validationinterface.cpp \
  versionbits.cpp \
  $(BITCOIN_CORE_H)
  
$(libquantisnet_server_a_SOURCES): $(QUANTISNET_PCH)

if ENABLE_ZMQ
libquantisnet_zmq_a_CPPFLAGS = $(BITCOIN_INCLUDES) $(ZMQ_CFLAGS) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif

$(libquantisnet_zmq_a_SOURCES): $(QUANTISNET_PCH)


# wallet: shared between quantisnetd and quantisnet-qt, but only linked
# when wallet enabled
libquantisnet_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_wallet_a_SOURCES = \
  keepass.cpp \
  pos_kernel.cpp \
  privatesend-client.cpp \
  privatesend-util.cpp \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/walletdb.cpp \
  policy/rbf.cpp \
  $(BITCOIN_CORE_H)
  
$(libquantisnet_wallet_a_SOURCES): $(QUANTISNET_PCH)

# crypto primitives library
crypto_libquantisnet_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_CONFIG_INCLUDES) $(PIC_FLAGS) -D__STDC_WANT_LIB_EXT1__=1 -DUSE_SECURE_MEMZERO
crypto_libquantisnet_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
if IS_GCC
   crypto_libquantisnet_crypto_a_CFLAGS = -std=c99
endif
crypto_libquantisnet_crypto_a_SOURCES = \
  crypto/aes.cpp \
  crypto/aes_helper.h \
  crypto/secure_memzero.h \
  crypto/aes.h \
  crypto/common.h \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

# x11
crypto_libquantisnet_crypto_a_SOURCES += \
  crypto/blake.c \
  crypto/bmw.c \
  crypto/cubehash.c \
  crypto/echo.c \
  crypto/groestl.c \
  crypto/jh.c \
  crypto/keccak.c \
  crypto/luffa.c \
  crypto/shavite.c \
  crypto/simd.c \
  crypto/skein.c \
  crypto/sph_blake.h \
  crypto/sph_bmw.h \
  crypto/sph_cubehash.h \
  crypto/sph_echo.h \
  crypto/sph_groestl.h \
  crypto/sph_jh.h \
  crypto/sph_keccak.h \
  crypto/sph_luffa.h \
  crypto/sph_shavite.h \
  crypto/sph_simd.h \
  crypto/sph_skein.h \
  crypto/sph_types.h

# consensus: shared between all executables that validate any consensus rules.
libquantisnet_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES)
libquantisnet_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_consensus_a_SOURCES = \
  amount.h \
  arith_uint256.cpp \
  arith_uint256.h \
  consensus/merkle.cpp \
  consensus/merkle.h \
  consensus/params.h \
  consensus/validation.h \
  hash.cpp \
  hash.h \
  prevector.h \
  primitives/block.cpp \
  primitives/block.h \
  primitives/transaction.cpp \
  primitives/transaction.h \
  pubkey.cpp \
  pubkey.h \
  script/quantisnetconsensus.cpp \
  script/interpreter.cpp \
  script/interpreter.h \
  script/script.cpp \
  script/script.h \
  script/script_error.cpp \
  script/script_error.h \
  serialize.h \
  tinyformat.h \
  uint256.cpp \
  uint256.h \
  utilstrencodings.cpp \
  utilstrencodings.h \
  version.h

# common: shared between quantisnetd, and quantisnet-qt and non-server tools
libquantisnet_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_common_a_SOURCES = \
  amount.cpp \
  base58.cpp \
  bip39.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  hdchain.cpp \
  key.cpp \
  keystore.cpp \
  netaddress.cpp \
  netbase.cpp \
  protocol.cpp \
  scheduler.cpp \
  script/sign.cpp \
  script/standard.cpp \
  warnings.cpp \
  $(BITCOIN_CORE_H)
  
$(libquantisnet_common_a_SOURCES): $(QUANTISNET_PCH)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libquantisnet_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_util_a_SOURCES = \
  support/lockedpool.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpc/protocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  threadinterrupt.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(BITCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libquantisnet_util_a_SOURCES += compat/glibc_compat.cpp
endif

$(libquantisnet_util_a_SOURCES): $(QUANTISNET_PCH)

# cli: shared between quantisnet-cli and quantisnet-qt
libquantisnet_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
libquantisnet_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libquantisnet_cli_a_SOURCES = \
  rpc/client.cpp \
  $(BITCOIN_CORE_H)
  
$(libquantisnet_cli_a_SOURCES): $(QUANTISNET_PCH)

nodist_libquantisnet_util_a_SOURCES = $(srcdir)/obj/build.h
#

# bitcoind binary #
quantisnetd_SOURCES = quantisnetd.cpp
quantisnetd_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
quantisnetd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
quantisnetd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
quantisnetd_SOURCES += quantisnetd-res.rc
endif

$(quantisnetd_SOURCES): $(QUANTISNET_PCH)

quantisnetd_LDADD = \
  $(LIBBITCOIN_SERVER) \
  $(LIBBITCOIN_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_WALLET) \
  $(LIBBITCOIN_ZMQ) \
  $(LIBBITCOIN_CONSENSUS) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

quantisnetd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS)

# quantisnet-cli binary #
quantisnet_cli_SOURCES = quantisnet-cli.cpp
quantisnet_cli_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(EVENT_CFLAGS) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
quantisnet_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
quantisnet_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
quantisnet_cli_SOURCES += quantisnet-cli-res.rc
endif

$(quantisnet_cli_SOURCES): $(QUANTISNET_PCH)

quantisnet_cli_LDADD = \
  $(LIBBITCOIN_CLI) \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO)
quantisnet_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# quantisnet-tx binary #
quantisnet_tx_SOURCES = quantisnet-tx.cpp
quantisnet_tx_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_INCLUDES) $(PIC_FLAGS) $(QUANTISNET_PCH_CPPFLAGS)
quantisnet_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
quantisnet_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
quantisnet_tx_SOURCES += quantisnet-tx-res.rc
endif

$(quantisnet_tx_SOURCES): $(QUANTISNET_PCH)

quantisnet_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CONSENSUS) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBSECP256K1)

quantisnet_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# quantisnetconsensus library #
if BUILD_BITCOIN_LIBS
include_HEADERS = script/quantisnetconsensus.h
libquantisnetconsensus_la_SOURCES = $(crypto_libquantisnet_crypto_a_SOURCES) $(libquantisnet_consensus_a_SOURCES)

if GLIBC_BACK_COMPAT
  libquantisnetconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

$(libquantisnetconsensus_la_SOURCES): $(QUANTISNET_PCH)

libquantisnetconsensus_la_LDFLAGS = $(AM_LDFLAGS) $(RELDFLAGS)
libquantisnetconsensus_la_LIBADD = $(LIBSECP256K1)
libquantisnetconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_BITCOIN_INTERNAL -D__STDC_WANT_LIB_EXT1__=1 -DUSE_SECURE_MEMZERO
libquantisnetconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CTAES_DIST =  crypto/ctaes/bench.c
CTAES_DIST += crypto/ctaes/ctaes.c
CTAES_DIST += crypto/ctaes/ctaes.h
CTAES_DIST += crypto/ctaes/README.md
CTAES_DIST += crypto/ctaes/test.c

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += consensus/*.gcda consensus/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += policy/*.gcda policy/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = $(CTAES_DIST)


config/quantisnet-config.h: config/stamp-h1
	@$(MAKE) -C $(top_builddir) $(subdir)/$(@)
config/stamp-h1: $(top_srcdir)/$(subdir)/config/quantisnet-config.h.in $(top_builddir)/config.status
	$(AM_V_at)$(MAKE) -C $(top_builddir) $(subdir)/$(@)
$(top_srcdir)/$(subdir)/config/quantisnet-config.h.in:  $(am__configure_deps)
	$(AM_V_at)$(MAKE) -C $(top_srcdir) $(subdir)/config/quantisnet-config.h.in

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

.mm.o:
	$(AM_V_CXX) $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(QT_INCLUDES) $(AM_CXXFLAGS) $(PIE_FLAGS) $(CXXFLAGS) -c -o $@ $<

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_BENCH
include Makefile.bench.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
