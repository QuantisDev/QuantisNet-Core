#!/bin/sh
#
# Copyright (c) 2017-2019 The QuantisNet Core developers
#

srcdir="$(dirname $0)"

if [ -z ${LIBTOOLIZE} ] && GLIBTOOLIZE="`which glibtoolize 2>/dev/null`"; then
  LIBTOOLIZE="${GLIBTOOLIZE}"
  export LIBTOOLIZE
fi

if [ "${SKIP_AUTO_DEPS}" = "true" ]; then
    :
elif which apt-get >/dev/null 2>&1; then
    deb_list=""
    deb_list="${deb_list} python3-setuptools python3-dev"
    deb_list="${deb_list} build-essential g++ libtool autotools-dev automake bsdmainutils pkg-config"
    deb_list="${deb_list} autoconf autoconf2.13 autoconf2.64"

    if ! which pip >/dev/null; then
        rpm_list="${rpm_list} python3-pip"
    fi

    if [ "$HOST" = "x86_64-linux-musl" ]; then
        deb_list="${deb_list} musl-dev musl-tools"
    elif [ "$HOST" = "x86_64-w64-mingw32" ] || [ "$HOST" = "i686-w64-mingw32" ]; then
        deb_list="${deb_list} mingw-w64 nsis"

        if [ "$(lsb_release -is)" = "Ubuntu" ]; then
            # old CI node
            deb_list="${deb_list} wine-development wine64-development wine-binfmt"
        else
            deb_list="${deb_list} wine wine64 wine-binfmt"
        fi
    elif [ -z "$HOST" ]; then
        deb_list="${deb_list} libssl-dev libevent-dev"
        deb_list="${deb_list} libboost-system-dev libboost-filesystem-dev libboost-chrono-dev"
        deb_list="${deb_list} libboost-program-options-dev libboost-test-dev libboost-thread-dev"
        deb_list="${deb_list} libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools"
        deb_list="${deb_list} libprotobuf-dev protobuf-compiler"
        deb_list="${deb_list} libqrencode-dev"
        deb_list="${deb_list} libminiupnpc-dev libzmq3-dev"
        deb_list="${deb_list} libdb4.8-dev libdb4.8++-dev"
        deb_list="${deb_list} lcov default-jre-headless"
        deb_list="${deb_list} ccache"
        deb_list="${deb_list} clang-5.0"
    fi

    deb_to_install=""


    if [ -n "${deb_to_install}" ]; then
        echo "Auto-trying to install Debian/Ubuntu deps"
        set -x
        sudo -n /usr/bin/apt-get install --no-install-recommends -y ${deb_to_install}
        set +x
    fi

    pip_install() {
        ( which futoin-cid && cd $srcdir && CC=gcc CXX=g++ cte pip install "$@" )

        for pip in /usr/local/bin/pip3 /usr/bin/pip3 /usr/local/bin/pip3 /usr/bin/pip; do
            if [ -e $pip ]; then
                CC=gcc CXX=g++ $pip install --user "$@";
                break
            fi
        done
    }
elif which yum >/dev/null 2>&1; then
    rpm_list=""
    rpm_list="${rpm_list} python-setuptools python-devel"
    rpm_list="${rpm_list} boost-devel ccache"
    rpm_list="${rpm_list} automake autoconf autoconf213 autoconf-archive"
    rpm_list="${rpm_list} qt5-qtbase-devel qt5-qtbase-gui qt5-qttools-devel"
    rpm_list="${rpm_list} protobuf-devel"
    rpm_list="${rpm_list} qrencode-devel"
    rpm_list="${rpm_list} libevent-devel"
    rpm_list="${rpm_list} miniupnpc-devel czmq-devel"
    rpm_list="${rpm_list} libdb4-devel libdb4-cxx-devel"
    rpm_list="${rpm_list} openssl-devel"

    if ! which pip >/dev/null; then
        rpm_list="${rpm_list} python2-pip"
    fi

    rpm_to_install=""
    for d in $rpm_list; do
        if ! rpm -q $d >/dev/null 2>&1; then
            rpm_to_install="${rpm_to_install} ${d}"
        fi
    done

    if [ -n "${rpm_to_install}" ]; then
        echo "Auto-trying to install RPM deps"
        set -x
        sudo -n /usr/bin/yum install -y ${rpm_to_install}
        set +x
    fi

    pip_install() {
        ( which futoin-cid && cd $srcdir && CC=gcc CXX=g++ cte pip install "$@" )

        CC=gcc CXX=g++ /bin/pip install --user "$@"
    }
elif which brew >/dev/null 2>&1; then
    brew_list=""
    brew_list="${brew_list} ccache"

    for f in $brew_list; do
        brew install $f || true
    done

    pip_install() {
        ( which futoin-cid && cd $srcdir && cte pip install "$@" )

        /usr/local/bin/pip install --user "$@"
    }
else
    pip_install() {
        :
    }
fi

#---
# pip_install pyzmq
# pip_install -U nrghash

#---
which autoreconf >/dev/null || \
  (echo "configuration failed, please install autoconf first" && exit 1)
autoreconf --install --force --warnings=all $srcdir

if [ -n "$HOST" ]; then
    case "$HOST" in
    *-w64-mingw32)
        # Need to update alternatives, ignore failure
        sudo -n update-alternatives --set $HOST-gcc /usr/bin/$HOST-gcc-posix || true
        sudo -n update-alternatives --set $HOST-g++ /usr/bin/$HOST-g++-posix || true
        ;;
    esac

    # TODO: create a separate QuantisNet SDK
    echo "Preparing dependencies"
    # # Ensure sysroot is clean of older versions which are unpacked from built folder
    rm -rf $srcdir/depends/$HOST
    make -C $srcdir/depends HOST=$HOST -j${MAKEJOBS:-$(nproc)}

    install_dir=$srcdir/build/${QUANTISNET_VER:-quantisnet}
    mkdir -p $install_dir

    if [ "$(uname)" = "Darwin" ]; then
        cp -Rpf $srcdir/depends/$HOST/* $install_dir/
    else
        cp -rauf $srcdir/depends/$HOST/* $install_dir/
    fi
fi
