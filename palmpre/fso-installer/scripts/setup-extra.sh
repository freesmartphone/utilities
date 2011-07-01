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
"

for file in $WEBOS_FILES ; do
	mkdir -p extra/`dirname $file`
	cp build/unpacked-doctor/rootfs/$file extra/$file
done
