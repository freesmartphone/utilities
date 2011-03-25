#!/bin/sh

if [ ! -d build/unpacked-doctor/rootfs ] ; then
	echo "webOS doctor is not unpacked!"
	exit 1
fi

WEBOS_FILES="\
	lib/firmware/helper_sd.bin \
	lib/firmware/sd8686.bin \
	lib/firmware/mfg_sd8686.bin \
	lib/firmware/sd8686_ap.bin \
	lib/modules/2.6.24-palm-joplin-3430/kernel/net/wifi/uap8xxx.ko \
	lib/modules/2.6.24-palm-joplin-3430/kernel/net/wifi/sd8xxx.ko \
	etc/camd/smia_ed.cfg \
	etc/camd/vx6852-load-firmware \
	etc/camd/vx6852-once-0.00.bin \
	etc/camd/vx6852-once-0.10.bin \
	etc/camd/vx6852-once-1.20.bin \
	etc/camd/vx6852-once-2.21.bin \
"

for file in $WEBOS_FILES ; do
	if [ $file == "lib/firmware/sd8686.bin" ] ; then
		# Simple quirk for WiFi firmware as we already have a firmware for sd8686
		# installed with the same name
		mkdir -p extra/lib/firmware
		cp build/unpacked-doctor/rootfs/$file extra/lib/firmware/sd8686-palm.bin
	else
		mkdir -p extra/`dirname $file`
		cp build/unpacked-doctor/rootfs/$file extra/$file
	fi
done
