# Use --with generic to build a generic package
%define is_generic  %{?_with_generic:     1} %{?!_with_generic:     0}

%define alsa_version 1.0.23

%if !%{is_generic}
%define version 1.0.23.1_k%{kversion_underscore}
%else
%define version 1.0.23.1
%endif

# We really want to be in that directory and not /usr/lib64 on 64-bit distributions
%define _libdir /usr/lib

Summary:   ALSA driver enhanced for Conexant HDA modem support
Name:      alsa-driver-linuxant
Version:   %{version}
%if %{is_generic}
BuildArch: noarch
Release:   1
%else
Release:   %{rel}
%endif
Source:    ftp://ftp.alsa-project.org/pub/driver/alsa-driver-%{alsa_version}.tar.bz2
URL:       http://www.alsa-project.org
%if %{is_generic}
Patch0:    alsa-driver-%{alsa_version}-install-1.patch
Patch1:    alsa-driver-1.0.23-1.patch
Patch2:    alsa-driver-1.0.23-smartaudio.patch
Patch3:    alsa-driver-1.0.23-hermosa.patch
Patch4:    alsa-driver-1.0.23-bdl_pos_adj.patch
Patch5:    alsa-driver-1.0.23-thinkpad.patch
Patch6:    alsa-driver-1.0.23-adriver.patch
Patch7:    alsa-driver-1.0.23-adriver_fmode.patch
Patch8:    alsa-driver-1.0.23-bool.patch
Patch9:    alsa-driver-1.0.23-hrtimer.patch
Patch10:    alsa-driver-1.0.23-tea575x.patch
Patch11:    alsa-driver-1.0.23-video_ioctl2.patch
Patch12:    alsa-driver-1.0.23-hrtimer_forward_now.patch
Patch13:    alsa-driver-1.0.23-usb_endpoint.patch
Patch14:    alsa-driver-1.0.23-crownbeach.patch
Patch15:    alsa-driver-1.0.23-acer_zr6.patch
Patch16:    alsa-driver-1.0.23-ppch-1.patch
Patch17:    alsa-driver-1.0.23-input_mux.patch
%endif
License:   GPL
Group:     Base/Kernel
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires:  gcc make perl

%description
Advanced Linux Sound Architecture driver. Driver is fully compatible
with OSS/Lite, but contains many enhanced features. This version is
enhanced with Conexant HDA modem support and this package is required
to use Conexant HDA modems with the HSF driver.

%prep
%if %{is_generic}

rm -fr alsa-driver-%{alsa_version}
tar -xjf ../SOURCES/alsa-driver-%{alsa_version}.tar.bz2
cd alsa-driver-%{alsa_version}

# Bug in alsa-driver-1.0.15, a pre-build binary for x86_64 was included in the package
rm -f utils/mod-deps

%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1
%patch8 -p1
%patch9 -p1
%patch10 -p1
%patch11 -p1
%patch12 -p1
%patch13 -p1
%patch14 -p1
%patch15 -p1
%patch16 -p1
%patch17 -p0

# Regen autotools, needed for adrver_fmode patch
autoconf

./configure

# It's important to run 'make dep' before installing the "install" patch and after
# installing patches that do change to the source code. We do that to drop the
# "patch" dependency of the package
make dep

%patch0 -p1
chmod +x symvers-update

# Use mrproper instead of mrextraproper since we want alsasound init script to be installed
make mrproper

%else # %{is_generic}

rm -fr alsa-driver-linuxant-%{version}
tar -xjf ../SOURCES/alsa-driver-linuxant.tar.bz2
mv alsa-driver-linuxant alsa-driver-linuxant-%{version}

%build

cd alsa-driver-linuxant-%{version}
./configure --with-moddir=%{_libdir}/%{name}/binaries/%{kversion}/modules --with-kernel=%{kbuild}
make

%endif # %{is_generic}


%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%if %{is_generic}

cd alsa-driver-%{alsa_version}
make DESTDIR=$RPM_BUILD_ROOT install

make mrextraproper

mkdir -p $RPM_BUILD_ROOT/%{_libdir}/%{name}
cp -pr * $RPM_BUILD_ROOT/%{_libdir}/%{name}
cp -p ../../SPECS/alsa-driver-linuxant.spec $RPM_BUILD_ROOT/%{_libdir}/%{name}

%else

LIBDIR="$RPM_BUILD_ROOT/%{_libdir}/%{name}"
MODDIR="${LIBDIR}/binaries/%{kversion}"

mkdir -p "${MODDIR}/modules"
mkdir -p "${MODDIR}/config"

cd alsa-driver-linuxant-%{version}
make DESTDIR=${RPM_BUILD_ROOT} install install-modules

if [ -r "%{kbuild}/kernelcompiler" ]; then
	KERNCOMPVER="`sed -n -e '/gcc version /s/gcc version //p' %{kbuild}/kernelcompiler 2>/dev/null`"

	if [ -n "${KERNCOMPVER}" ]; then
		for m in `find ${RPM_BUILD_ROOT}/%{_libdir}/%{name}/binaries/%{kversion}/modules -name "*.ko"`; do
			[ -r $m ] || continue
			sed "s@\(vermagic=.* \)gcc-[0-9]\.[0-9]@\1gcc-${KERNCOMPVER}@g" < $m > $m.tmp && mv $m.tmp $m
			strip --strip-debug $m
		done
	fi
fi

cp -p symvers-update Makefile Makefile.conf toplevel.config "${LIBDIR}"

for f in Module.symvers include/linux/autoconf.h; do
	if [ -e "${f}" ]; then
		cp -p "${f}" "${MODDIR}/config"
	fi
done

# Mimic the source tree a little to make it easier to run the Makefile
mkdir -p "${LIBDIR}/alsa-kernel"

mkdir -p "${LIBDIR}/include/sound"
cp -p include/sound/*.h "${LIBDIR}/include/sound"

%endif

# Use %{name} instead of alsasound for the init. script to prevent conflict in SUSE 11.0
mv $RPM_BUILD_ROOT/etc/init.d/alsasound $RPM_BUILD_ROOT/etc/init.d/%{name}

# Prevent the user to install multiple versions of the package at the same time
# This file will be different in each version that will create a conflict
echo "%{version}" > $RPM_BUILD_ROOT/%{_libdir}/%{name}/rpm_version


%post
if [ -x /usr/sbin/hsfconfig ]; then
	/usr/sbin/hsfconfig --remove
fi

if [ -x /sbin/chkconfig ]; then
	# The grep here is used to remove a warning in Mandriva 2009.0
	/sbin/chkconfig --add %{name} 2>&1 | grep "netfs"
	/sbin/chkconfig %{name} on
fi

%if %{is_generic}

buildlog=%{_tmppath}/%{name}.$$.log
echo -n "Building modules for the `uname -r` kernel, please wait..."
( cd /usr/lib/%{name} && ./configure && make && make install-kernel ) > "${buildlog}" 2>&1
if [ ! -e /usr/lib/%{name}/noclean ]; then
	( cd /usr/lib/%{name} && make mrextraproper ) > "${buildlog}" 2>&1
fi
ret=$?
echo " done."

if [ ${ret} -ne 0 ]; then
	echo 2>&1 "ERROR: Build failed. Please review the build log at ${buildlog}"
else
	rm -f "${buildlog}"
fi

%else # %{is_generic}

MODDIR="%{_libdir}/%{name}/binaries/`uname -r`"

if [ -d "${MODDIR}" ]; then
	echo "Pre-built driver modules that seem compatible with your system were found under:"
	echo "${MODDIR}"
	cd "%{_libdir}/%{name}"
	make PREBUILT_DIR=${MODDIR} install-kernel > /dev/null
	ret=$?
else
	echo "No pre-built modules for the `uname -r` kernel. Please use a generic package"
	echo "or the right pre-compiled package for your kernel."
	ret=1
fi

%endif # %{is_generic}

if [ -x /usr/sbin/hsfconfig ]; then
	/usr/sbin/hsfconfig --auto
fi

exit ${ret}


%preun
if [ -x /usr/sbin/hsfconfig ]; then
	/usr/sbin/hsfconfig --remove
fi

if [ -x /sbin/chkconfig ]; then
	# The grep here is used to remove a warning in Mandriva 2009.0
	/sbin/chkconfig %{name} off | grep "netfs"
	/sbin/chkconfig --del %{name}
fi

%if %{is_generic}
( cd %{_libdir}/%{name} && make uninstall > /dev/null )
%else
MODDIR="%{_libdir}/%{name}/binaries/`uname -r`"
( cd %{_libdir}/%{name} && make PREBUILT_DIR=${MODDIR} uninstall > /dev/null )
%endif

if [ -x /usr/sbin/hsfconfig ]; then
	/usr/sbin/hsfconfig --auto || true
fi


%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/%{name}
/usr/sbin/reload-alsa-driver
/etc/init.d/%{name}

%changelog
* Sun Nov 28 2010 -
	* Added input_mux patch
* Thu May 17 2010 -
	* Upgraded to 1.0.23 final
	* Make sure that reload-alsa-driver will run depmod even if it fails to unload the modules
	* Added the hrtimer_forward_now patch to fix a build problem with the 2.6.24 kernel
* Thu Apr 15 2010 -
	* Upgraded to git20100411
	* Removed the adriver_fmode patch, alsa-driver now has a simiar fix built-in
* Tue Sep 01 2009 -
	* Upgraded to 1.0.21 final
* Thu Jun 11 2009 -
	* Added the video_ioctl2 patch to fix a build problem with the 2.6.30 kernel
* Mon Jun 08 2009 -
	* Renamed the hermosaheadphone patch to simply hermosa and added "Port D" related fixes to the patch
* Wed May 27 2009 -
	* Fixed several merge problems
	* Added the proc_print patch to be able to read bios_profile from /proc
	* Added support to build pre-compiled x86_64 (amd64) packages
* Thu May 09 2009 -
	* Installing a pre-compiled pakages will no longer fail if the kernel headers are not installed
	* Improved error messages when something goes wrong with the "make debprecomp" and "make rpmprecomp" features
	* Added tea575x patch to solve compilation issue under kernels < 2.6.27
	* Improved the usb_endpoint patch to fix compilation issue
	* Added hrtimer path to fix yet another compilation issue
	* Added support to make pre-compiled packages
	* Rebased on 1.0.20
* Thu Mar 05 2009 -
	* Improved support for codec reconfiguration via "sys" interface
	* Improved uninstallation
	* Added some options to the reload-alsa-driver script
	* Update the kernel configuration (CONFIG_*) options at the same time we update the headers and symbol versions
* Wed Feb 11 2009 -
	* Fixed multiple build issues with Red Hat based 2.6.18 kernels
	* Improved main HDA patch to fix build issue when CONFIG_SND_JACK is not defined
* Thu Feb 05 2009 -
	* Fixed deb_version file (DEB package specific)
	* Zipped RPM files now unpack their files in the current directory
* Tue Jan 20 2009 -
	* Upgraded to alsa-driver 1.0.19
	* rpm -V alsa-driver-linuxant shouldn't find differences anymore with a typical use of the package
	* Added more precise detection for the fmode type
	* Updated main HDA patch as well as the smartaudio one
* Tue Dec 04 2008 -
	* Upgraded to git20081204
	* Fixed a build issue with the 2.6.25.9 kernel
* Tue Nov 28 2008 -
	* Upgraded to git20081128
* Tue Oct 29 2008 -
	* Upgraded to 1.0.18 final
* Tue Oct 21 2008 -
	* Upgraded to git20081020
	* Fixed modem enumeration problem on Thinkpad/Lenovo x6x machines
	* Removed the 'patch' dependency
	* Prevent unwanted warning messages on Mandriva 2009.0 while installing and removing the package
* Tue Sep 15 2008 -
	* Upgraded to git20080915
	* Added the hermosaheadphone slave_put and bdl_pos_adj patches
* Tue Sep 10 2008 -
	* Upgraded to alsa-driver-1.0.18rc3
* Tue Sep 08 2008 -
	* Fixed another issue with the smartaudio patch
* Tue Aug 29 2008 -
	* Fixed some issues with the smartaudio patch
* Tue Aug 26 2008 -
	* Added smartaudio patch
* Wed Aug 20 2008 -
	* Updated to alsa-driver 1.0.18rc1
* Thu Aug 12 2008 -
	* Fix multiple build issue on 2.6.23 and earlier kernels
* Thu Jul 23 2008 -
	* Prevent a possible build problem related to a file copy issue
* Thu Jul 18 2008 -
	* Prevent the user to install multiple versions of the package at the same time
	* %preun will not fail if hsfconfig --auto fails
	* Added the "patch" dependency
* Thu Jul 17 2008 -
	* Upgraded to ALSA 1.0.17 final
* Tue Jul 01 2008 -
	* Upgraded to  ALSA 1.0.17rc3
* Tue Jun 24 2008 -
	* Upgraded to ALSA 1.0.17rc2 to fix multiple build problems on SUSE 11.0
	* Improved kernel headers location detection to workaround fake kernel build directory on SUSE 11.0
	* Renamed the init. script to alsa-driver-linuxant to prevent a conflict on SUSE 11.0
* Mon Apr 7 2008 -
	* Improvements dealing with the HSF driver and detecting existing ALSA modules
	* Fixed an enumeration issue with the HDA bus driver
* Tue Feb 19 2008 -
	* Upgraded to ALSA 1.0.16
* Fri Nov 16 2007 -
	* Initial version

