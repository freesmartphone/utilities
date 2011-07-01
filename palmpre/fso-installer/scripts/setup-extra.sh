#!/bin/sh

if [ ! -d build/unpacked-doctor/rootfs ] ; then
	echo "webOS doctor is not unpacked!"
	exit 1
fi

WEBOS_FILES="\
	etc/camd/smia_ed.cfg \
	etc/camd/vx6852-load-firmware \
	etc/camd/vx6852-once-0.00.bin \
	etc/camd/vx6852-once-0.10.bin \
	etc/camd/vx6852-once-1.20.bin \
	etc/camd/vx6852-once-2.21.bin \
	usr/lib/libEGL.so
	usr/lib/libsrv_um.so
	usr/lib/libIMGegl.so
	usr/lib/libpvrPVR2D_LINUXFBWSEGL.so
	usr/lib/libpvrPVR2D_BLITWSEGL.so
	usr/lib/libOpenVGU.so
	usr/lib/libsrv_init.so
	usr/lib/libPVRScopeServices.so
	usr/lib/libpvrPVR2D_FRONTWSEGL.so
	usr/lib/libOpenVG.so
	usr/lib/libGLES_CM.so
	usr/lib/libpvrPVR2D_FLIPWSEGL.so
	usr/lib/libpvr2d.so
	usr/lib/libGLESv2.so
	usr/lib/libews.so
	usr/lib/libglslcompiler.so
	usr/lib/libpvrEWS_WSEGL.so
	usr/bin/pvrsrvinit
"

for file in $WEBOS_FILES ; do
	mkdir -p extra/`dirname $file`
	cp build/unpacked-doctor/rootfs/$file extra/$file
done
