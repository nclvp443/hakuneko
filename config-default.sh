#! /bin/sh

PKGNAME="hakuneko"
PKGVERSION="1.0.2"
PKGSECTION="net"
PKGAUTHOR="Ronny Wegener <wegener.ronny@gmail.com>"
PKGHOMEAGE="http://hakuneko.googlecode.com"
PKGDEPENDS="ffmpeg, libc6 (>=0.1.2-3), libgtk2.0-0 (>=1.2.3-4)"
PKGDESCRIPTION="A GTK based Manga Downloader
 HakuNeko allows you to download manga images from
 some selected online manga reader websites
 .
 Currently Supported:
 .
 + MangaReader
 + MangaHere
 + MangaFox
 + Batoto
 + Submanga"

SRCPATTERN="*.cpp"
SRCDIR="src"
OBJDIR="obj"
DISTROOT="dist/linux"
BINFILE="$DISTROOT/usr/bin/$PKGNAME"

CC="g++"
CFLAGS="
	-Wall
	-O2
	$(wx-config --static=yes --debug=no --cflags)
	"

LD="g++"
LDFLAGS="
	-s
	$(wx-config --static=yes --debug=no --libs)
	"
