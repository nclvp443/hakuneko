#############
### LINUX ###
#############

	-----------------------------------------------------
	- requirements to compile hakuneko on debian/wheezy -
	-----------------------------------------------------

	+ build-essentials (gcc, g++, make, lib*,...)
	+ libgtk2.0-dev
	+ libgtkgl2.0-dev
	+ libglu1-mesa-dev
	+ libpng12-dev
	+ libjpeg8-dev
	+ libtiff5-dev
	+ libexpat1-dev
	+ libpcre3-dev
	+ zlib1g-dev

	+ wxGTK 2.8.12 configure parameters:
		--enable-unicode
		--disable-shared
		--enable-monolithic
		--with-opengl
		--with-libpng=sys
		--with-libjpeg=sys
		--with-libtiff=sys
		--with-expat=sys
		--with-regex=sys
		--with-zlib=sys
		--disable-debug

	----------------------
	- compiling hakuneko -
	----------------------

	# open a terminal
	# change into hakuneko directory
	# run the following commands as root user
	./configure
	make
	make install

	# remove hakuneko
	# run the following command as root user
	make uninstall

#############
### WINNT ###
#############

	---------------------------------------------
	- Requirements to compile HakuNeko on win32 -
	---------------------------------------------

	+ MinGW32 (GCC >= 4.5)
	+ wxWidgets MSW >= 2.8.12 build parameters:
		MONOLITHIC=1
		SHARED=0
		UNICODE=1
		BUILD=release
		USE_OPENGL=1

	----------------------
	- Compiling HakuNeko -
	----------------------

	open a command prompt (cmd)
	change into hakuneko directory
	run configure.cmd
	run make
	run make install

	+ open a command prompt (cmd.exe)
	+ add your MinGW to the system environment path (SET PATH=C:\MinGW\bin)
	+ set environment variable for your wxWidgets (SET WXWIN=C:\wxMSW)
	+ change into HakuNeko's build directory (CD .\build)
	+ clean previous portable builds (mingw32-make.exe -f win32.make BUILD=portable clean)
	+ compile HakuNeko as portable* application (mingw32-make.exe -f win32.make BUILD=portable)
	  *portable: all settings are stored inside the applications root directory
	+ HakuNeko_Portable.exe can be found in the .\bin\win32 directory
