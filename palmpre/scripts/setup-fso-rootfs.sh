#!/bin/sh

FSO_ROOTFS=/media/fso
FSO_DOWNLOAD_URL=

error () {
	echo "[ERROR] $1"
}

info () { 
	echo "[INFO] $1"
}

download_and_install_fso_rootfs() {
	rootfs=$1
	
	# create and format image
	dd if=/dev/zero of=/media/internal/fso-rootfs.img bs=1MB count=128
	mkfs.ext3 /media/internal/fso-rootfs.img
	
	# create mount point
	mount -o remount,rw /
	mkdir -p /media/fso
	echo "/media/internal/fso-rootfs.img $rootfs auto defaults 0 0" >> /etc/fstab
	mount -o remount,ro /
	
	# mount image
	mount $rootfs
	
	# download and extract rootfs
	wget -O /media/internal/fso-rootfs.tar.bz2 $FSO_DOWNLOAD_URL
	tar xjf /media/internal/fso-rootfs.tar.bz2 -C $rootfs
	
	# make rootfs valid
	mkdir -p $rootfs/.config
	touch $rootfs/.config/valid
}

install_webos_libs() {
	rootfs=$1
	
	# FIXME install webos-libs
	
	touch $rootfs/.config/webos_libs_installed
}

# Is there already a FSO rootfs?
if [ ! -d $FSO_ROOTFS ] ; then
	error "There is no FSO rootfs directory!"
	download_and_install_fso_rootfs
fi

# Check if the FSO rootfs is valid
if [ ! -f $FSO_ROOTFS/.config/valid ] ; then
	error "You do not have a valid FSO rootfs!"
	error "Please remove the FSO rootfs on our own and restart the script"
	exit 1
fi

if [ ! -f $FSO_ROOTFS/.config/webos_libs_installed ] ; then
	error "webos-libs are not installed!"
	install_webos_libs $FSO_ROOTFS
fi

# Mount system directories
mount -t proc none $FSO_ROOTFS/proc
moint -t sysfs none $FSO_ROOTFS/sys
mount -o bind /dev $FSO_ROOTFS/dev

# Chroot!
chroot $(FSO_ROOTFS) /bin/sh



