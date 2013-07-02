#!/bin/bash

PKGNAME=hakuneko
PKGVERSION=1.0.5
DEBVERSION=1
PKGAUTHOR="Ronny Wegener"
PKGHOMEPAGE=http://hakuneko.googlecode.com

DEBHELPERCOMPAT=9

##########################################
### create the original source package ###
##########################################

OGZPKG="$PKGNAME"_"$PKGVERSION".orig.tar.gz
echo $OGZPKG

###################################
### create the debian directory ###
###################################

mkdir -p debian/source 2> /dev/null

#############################
### create changelog file ###
#############################

echo "3.0 (quilt)" > debian/source/format
echo "$PKGNAME $PKGVERSION unstable; urgency=low" > debian/changelog
echo "" >> debian/changelog
echo "  [ $PKGAUTHOR ]" >> debian/changelog
echo "  * $PKGNAME update, details on URL: $PKGHOMEPAGE" >> debian/changelog
echo "" >> debian/changelog
echo " -- $PKGAUTHOR  $(date -R -u)" >> debian/changelog

##########################
### create compat file ###
##########################

echo "$DEBHELPERCOMPAT" > debian/compat

###########################
### create control file ###
###########################

echo "Source: $PKGNAME" > debian/control
echo "Maintainer: $PKGAUTHOR" >> debian/control
echo "Section: $PKGSECTION" >> debian/control
echo "Priority: optional" >> debian/control
echo "Standards-Version: 3.9.4" >> debian/control
# TODO: resolve packages
echo "Build-Depends: debhelper (>= $DEBHELPERCOMPAT), RESOLVE_DEV_PKG_DEPS" | sed -e 's|,\\s*\$\$||g' >> debian/control
echo "" >> debian/control
echo "Package: $PKGNAME" >> debian/control
echo "Architecture: any" >> debian/control
echo "Depends: \${shlibs:Depends}, \${misc:Depends}" >> debian/control
echo "Homepage: $PKGHOMEPAGE" >> debian/control
echo "Description: $PKGDESCRIPTION" >> debian/control
if [ -f COPYRIGHT ]
then
    cp COPYRIGHT debian/copyright
elif [ -f LICENSE ]
then
    cp LICENSE debian/copyright
else
    echo "UNKNOWN LICENSE" > debian/copyright
    echo "" >> debian/copyright
    echo "Copyright (c) $(date +%Y) $PKGAUTHOR" >> debian/copyright
    echo "" >> debian/copyright
    echo "This Software was build without any license." >> debian/copyright
    echo "Please contact the author for further information." >> debian/copyright
fi

#########################
### create rules file ###
#########################

echo "#!/usr/bin/make -f" > debian/rules
echo "" >> debian/rules
echo "%:" >> debian/rules
# TODO: tabs
echo "\tdh \$\$@" >> debian/rules
echo "" >> debian/rules
echo "override_dh_auto_install:" >> debian/rules
# TODO: tabs, $$$ signs
echo "\t\$\$(MAKE) DESTDIR=\$\$\$\$(pwd)/debian/$PKGNAME PREFIX=/usr install" >> debian/rules
chmod 0755 debian/rules

#################################
### create postinstall script ###
#################################

echo "#!/bin/sh" > debian/postinst
echo "" >> debian/postinst
echo "set -e" >> debian/postinst
# TODO: menu and mime update no longer needed?
# echo "if [ -x /usr/bin/update-menus ] ; then update-menus ; fi" >> debian/postinst
# echo "if [ -x /usr/bin/update-mime ] ; then update-mime ; fi" >> debian/postinst
echo "" >> debian/postinst
echo "#DEBHELPER#" >> debian/postinst
chmod 0755 debian/postinst

################################
### create postremove script ###
################################

echo "#!/bin/sh" > debian/postrm
echo "" >> debian/postrm
echo "set -e" >> debian/postrm
# TODO: menu and mime update no longer needed?
# echo "if [ -x /usr/bin/update-menus ] ; then update-menus ; fi" >> debian/postrm
# echo "if [ -x /usr/bin/update-mime ] ; then update-mime ; fi" >> debian/postrm
echo "" >> debian/postrm
echo "#DEBHELPER#" >> debian/postrm
chmod 0755 debian/postrm

########################################
### create the debian source package ###
########################################

DGZPKG="$PKGNAME"_"$PKGVERSION".debian.tar.gz
echo $DGZPKG

exit

# TODO: check if PKGVERSION is obsolete...
# TODO: skip empty dirs/files, skip include, skip libs
$ECHO_ESC "\t@tar -czvf \$(DGZPKG) --transform 's,^,\$(PKGNAME)-\$(PKGVERSION)/,' --exclude='make*' --exclude='Make*' --exclude='*.tar.gz' --exclude='*.dsc' --exclude='*.deb' --exclude='*.lib' --exclude='*.a' --exclude='\$(BINFILE)' --exclude='\$(OBJDIR)/*' *" >> $MAKEFILE
$ECHO_ESC "\t@rm -f -r debian" >> $MAKEFILE

#######################
### create dsc file ###
#######################

###########################
### create changes file ###
###########################





write_makefile_rule_dist_dsc()
{
    $ECHO_ESC "DSCFILE := \$(PKGNAME)_\$(PKGVERSION).dsc" >> $MAKEFILE
    $ECHO_ESC "" >> $MAKEFILE
    $ECHO_ESC "dsc: dgz" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Format: 3.0 (native)' > \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Source: \$(PKGNAME)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Binary: \$(PKGNAME)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Architecture: any' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Version: \$(PKGVERSION)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Maintainer: \$(PKGAUTHOR)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Homepage: \$(PKGHOMEPAGE)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Standards-Version: 3.9.3' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Build-Depends: debhelper (>= 9), \$(RESOLVE_DEV_PKG_DEPS)' | sed -e 's|,\\s*\$\$||g' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Package-List:' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(PKGNAME) deb \$(PKGSECTION) optional' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Checksums-Sha1:' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha1sum \$(DGZPKG) | cut -b-40) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(DGZPKG)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Checksums-Sha256:' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha256sum \$(DGZPKG) | cut -b-64) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(DGZPKG)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Files:' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell md5sum \$(DGZPKG) | cut -b-32) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(DGZPKG)' >> \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo '' >> \$(DSCFILE)" >> $MAKEFILE
# sign file with gpg (requires a gpg key already generated)
# entering passphrase is required
$ECHO_ESC "\t@gpg -a --clearsign \$(DSCFILE)" >> $MAKEFILE
$ECHO_ESC "\t@mv -f \$(DSCFILE).asc \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "\t-@lintian --profile debian \$(DSCFILE)" >> $MAKEFILE
    $ECHO_ESC "" >> $MAKEFILE
}

write_makefile_rule_dist_chg()
{
    $ECHO_ESC "CHGFILE := \$(PKGNAME)_\$(PKGVERSION).changes" >> $MAKEFILE
    $ECHO_ESC "" >> $MAKEFILE
    $ECHO_ESC "chg: dsc" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Format: 1.8' > \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Date: \$(shell date -R -u)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Source: \$(PKGNAME)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Binary: \$(PKGNAME)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Architecture: source' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Version: \$(PKGVERSION)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Distribution: unstable' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Urgency: low' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Maintainer: \$(PKGAUTHOR)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Changed-By: \$(PKGAUTHOR)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Description: ' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(PKGNAME) - $($ECHO_ESC "$PKGDESCRIPTION" | head -n 1)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Changes: ' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(PKGNAME) (\$(PKGVERSION)) unstable; urgency=low' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' .' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo '   [ \$(PKGAUTHOR) ]' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo '   * \$(PKGNAME) update, details on URL: \$(PKGHOMEPAGE)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Checksums-Sha1:' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha1sum \$(DSCFILE) | cut -b-40) \$(shell du -b \$(DSCFILE) | sed -e 's|\s*\$(DSCFILE)||g') \$(DSCFILE)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha1sum \$(DGZPKG) | cut -b-40) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(DGZPKG)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Checksums-Sha256:' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha256sum \$(DSCFILE) | cut -b-64) \$(shell du -b \$(DSCFILE) | sed -e 's|\s*\$(DSCFILE)||g') \$(DSCFILE)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell sha256sum \$(DGZPKG) | cut -b-64) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(DGZPKG)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo 'Files:' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell md5sum \$(DSCFILE) | cut -b-32) \$(shell du -b \$(DSCFILE) | sed -e 's|\s*\$(DSCFILE)||g') \$(PKGSECTION) optional \$(DSCFILE)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo ' \$(shell md5sum \$(DGZPKG) | cut -b-32) \$(shell du -b \$(DGZPKG) | sed -e 's|\s*\$(DGZPKG)||g') \$(PKGSECTION) optional \$(DGZPKG)' >> \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t@echo '' >> \$(CHGFILE)" >> $MAKEFILE
# sign file with gpg (requires a gpg key already generated)
# entering passphrase is required
$ECHO_ESC "\t@gpg -a --clearsign \$(CHGFILE)" >> $MAKEFILE
$ECHO_ESC "\t@mv -f \$(CHGFILE).asc \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "\t-@lintian --profile debian \$(CHGFILE)" >> $MAKEFILE
    $ECHO_ESC "" >> $MAKEFILE
}
