#!/bin/bash

# import setings from config-default
. ./config_default.sh

# overwrite settings from config-default
RCPATTERN="resource.rc"
RCDIR="res"
DISTROOT="build/msw"
BINFILE="$DISTROOT/bin/$PKGNAME.exe"

CFLAGS="
    -c
    -Wall
    -O2
    -D__GNUWIN32__
    -D__WXMSW__
    -DwxUSE_UNICODE
    -DCURL_STATICLIB
    -Iinclude/msw
    -Ilib/msw/wx/mswu
    "

RC="windres.exe"
RCFLAGS="
    -J rc
    -O coff
    -F pe-i386
    -Iinclude/msw
    "

LDFLAGS="
    -s
    -static-libgcc
    -static-libstdc++
    -mwindows
    "
LDLIBS="
    -Llib/msw/wx
    -lwx_mswu-2.8
    -lwx_mswu_gl-2.8
    -lwxexpat-2.8
    -lwxregexu-2.8
    -lwxpng-2.8
    -lwxjpeg-2.8
    -lwxtiff-2.8
    -lwxzlib-2.8
    -Llib/msw/curl
    -lcurl -lrtmp -lwinmm -lssh2 -lssl -lcrypto -lgdi32 -lcrypt32 -lz -lidn -lwldap32 -lws2_32
    -Llib/msw/openssl
    -lcrypto
    -L/mingw/lib
    -lwinspool
    -lole32
    -loleaut32
    -luuid
    -lcomctl32
    -lwsock32
    "
