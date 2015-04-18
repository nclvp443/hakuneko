#!/bin/bash

PKGNAME="hakuneko"
PKGVERSION="1.3.7"
PKGSECTION="net"
PKGAUTHOR="Ronny Wegener <wegener.ronny@gmail.com>"
PKGHOMEPAGE="http://hakuneko.sourceforge.net"
PKGDEPENDS=""
PKGDESCRIPTION="Manga Downloader based on wxGTK
 HakuNeko allows you to download manga images from
 some selected online manga reader websites
 .
 Currently Supported:
 .
 * MangaReader
 * MangaHere
 * MangeHere (es)
 * MangaFox
 * Batoto
 * MangaHead
 * KissAnime"

SRCPATTERN="*.cpp"
SRCDIR="src"
RCPATTERN=""
RCDIR=""
OBJDIR="obj"
DISTROOT="build/linux"
BINFILE="$DISTROOT/bin/$PKGNAME"

CC="g++"
CFLAGS="
    -c
    -Wall
    -O2
    $(wx-config --static=no --debug=no --cflags)
    "

RC=""
RCFLAGS=""

LD="g++"
LDFLAGS="-s"
LDLIBS="
    $(wx-config --static=no --debug=no --libs)
    -lcurl
    -lcrypto
    "

echo "#define VERSION wxT(\"$PKGVERSION\")" > "src/version.h"
