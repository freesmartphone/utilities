#
# Copyright (C) 2011 by Sylvain Paré <sylvain.pare@gmail.com>
#                       Simon Busch <morphis@gravedo.de>
#                       Lukas 'Slyon' Märdian <lukasmaerdian@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#


###############################################################################
# Configuration
###############################################################################

# Select "pre", "preplus", "pre2" or "hpveer".
TARGET = unknown
# Define the size you want to let on your media partition of WebOS: 00.5G < X < 07.5G (15.5G for a pre2)
# It NEEDS to be formated as XX.XG
# The remaining space will be used for SHR
CONFIG_WEBOS_MEDIA_TARGET_SIZE = 05.5G

# Base URL for downloading kernels and images
ORIGIN_URL = http://build.shr-project.org/shr-unstable/images/
# Name of your webos doctor file
# DOCTOR = webosdoctor*.jar

# Select "wr", "sprint", "verizonwireless", "bellmo", "telcel", "att" or "wifi".
CARRIER = wr

###############################################################################
# !!! DO NOT MODIFY ANY OF THE FOLLOWING LINES !!!
###############################################################################
DOWNLOADDIRECTORY = downloads
SUPPORTEDTARGETS = pre preplus pre2 hpveer
ifeq ($(filter ${TARGET},${SUPPORTEDTARGETS}),)
$(error You did not enter a proper TARGET)
endif

TIME_TO_WAIT_FOR_DEVICE = 10
BOOT_PARTITION = /dev/mmcblk0p2

ifneq ($(shell echo ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}|wc -c),6)
$(error Wrong CONFIG_WEBOS_MEDIA_TARGET_SIZE format!!)
endif

ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '<' '00.5G'),1)
$(error CONFIG_WEBOS_MEDIA_TARGET_SIZE needs to be bigger than 00.5G)
endif

ifeq (${TARGET},pre)
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '07.5G'),1)
$(error $(shell echo |awk '{print 8.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
CODENAME = castle
ifeq (${CARRIER},wr)
MODEL = p100ueu
VERSION = 1.4.5
endif
ifeq (${CARRIER},sprint)
MODEL = p100eww
VERSION = 1.4.5
endif
ifeq (${CARRIER},bellmo)
MODEL = p100eww
VERSION = 1.4.5
endif
ifeq (${CARRIER},telcel)
MODEL = p100eww
VERSION = 1.4.0
CARRIER_TARBALL = wr.tar
endif
endif

ifeq (${TARGET},preplus)
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '15.5G'),1)
$(error $(shell echo |awk '{print 8.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
CODENAME = castle
ifeq (${CARRIER},wr)
MODEL = p101ueu
VERSION = 2.1.0
CARRIER_TARBALL = wr-castle-plus.tar
endif
ifeq (${CARRIER},verizonwireless)
MODEL = p101eww
VERSION = 1.4.5.1
CARRIER_TARBALL = verizon.tar
endif
ifeq (${CARRIER},att)
MODEL = p101eww
VERSION = 1.4.5
endif

endif

#common stuff to both pre and preplus devices redefined if it is a pre2
IMAGEFILENAME = full-palmpre.tar.gz
KERNELFILENAME = uImage-palmpre.bin
DOWNLOAD_BASE_URL = "${ORIGIN_URL}/palmpre"

ifeq (${TARGET},pre2)
IMAGEFILENAME = full-palmpre2.tar.gz
KERNELFILENAME = uImage-palmpre2.bin
DOWNLOAD_BASE_URL = ${ORIGIN_URL}/palmpre2
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '15.5G'),1)
$(error $(shell echo |awk '{print 16.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
CODENAME = roadrunner
ifeq (${CARRIER},wr)
MODEL = p103ueuna
VERSION = 2.1.0
ifeq (${VERSION},2.0.1)
MODEL = p102ueuna
endif
ifeq (${VERSION},2.0.0)
MODEL = p103ueu
endif
endif
ifeq (${CARRIER},verizonwireless)
MODEL = p102eww
VERSION = 2.0.1
CARRIER_TARBALL = verizon.tar
DOCTOR = webosdoctorp102${CARRIER}-${VERSION}.jar
endif

endif

ifeq (${TARGET},hpveer)
IMAGEFILENAME = full-hpveer.tar.gz
KERNELFILENAME = uImage-hpveer.bin
DOWNLOAD_BASE_URL = ${ORIGIN_URL}/hpveer
TIME_TO_WAIT_FOR_DEVICE = 15
BOOT_PARTITION = /dev/mmcblk0p14
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '07.5G'),1)
$(error $(shell echo |awk '{print 8.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
CODENAME = broadway
ifeq (${CARRIER},wr)
MODEL = p160una
VERSION = 2.1.1
endif
ifeq (${CARRIER},att)
MODEL = p160una
VERSION = 2.1.2
endif
endif

ifndef DOCTOR
DOCTOR = webosdoctor${MODEL}${CARRIER}-${VERSION}.jar
ifeq (${CARRIER},wr)
DOCTOR = webosdoctor${MODEL}-${CARRIER}-${VERSION}.jar
endif
ifeq (${CARRIER},wifi)
DOCTOR = webosdoctor${MODEL}${CARRIER}-${VERSION}.jar
endif
endif

PATIENT = ${TARGET}-${MODEL}-${CARRIER}-${VERSION}

IMAGE ?= $(DOWNLOADDIRECTORY)/$(IMAGEFILENAME)
KERNEL ?= $(DOWNLOADDIRECTORY)/$(KERNELFILENAME)
BOOTR = bootr_0.4.tar.gz
BOOTRURL = https://github.com/downloads/slyon/bootr/${BOOTR}
# Base URL for downloading GNU patch
PATCH_URL = http://ipkg.preware.org/feeds/webos-internals/armv7/
PATCH_IPK = org.webosinternals.patch_2.5.9-4_armv7.ipk
NOVAINSTALLER = nova-installer-image-${CODENAME}.uImage
NOVA_IMAGE_NAME = nova-cust-image-${CODENAME}.rootfs.tar.gz

###############################################################################
# General
###############################################################################

.PHONY: all
all:

.PHONY: update
update:
	@echo TODO? update this makefile ?

.PHONY: clean
clean: 
	@echo "Cleanup everything ..."
	@rm -rf build deps

###############################################################################
# webOS doctor unpack stuff
###############################################################################

build/${PATIENT}: downloads/$(DOCTOR)
	@mkdir -p build/${PATIENT}

build/${PATIENT}/resources/.unpacked: build/${PATIENT}
	@echo "Extract the webos doctor ..."
	@if [ `which jar` ] ; then export unjar="jar -xf" ; elif [ `which unzip` ] ; then export unjar="unzip" ; fi ; \
	if [ "$$unjar" = "" ] ; then echo "Not found any command to extract webos doctor. Please install the jar or unzip utility to proceed!" ; false ; \
	else (cd build/${PATIENT}; $$unjar ../../downloads/$(DOCTOR) > /dev/null || true ) ; fi
	@(touch build/${PATIENT}/resources/.unpacked)

build/${PATIENT}/webos/.unpacked: build/${PATIENT}/resources/.unpacked
	@(mkdir -p build/${PATIENT}/webos)
	@(tar xf build/${PATIENT}/resources/webOS.tar -C build/${PATIENT}/webos)
	@(touch build/${PATIENT}/webos/.unpacked)

build/${PATIENT}/rootfs/.unpacked: build/${PATIENT}/webos/.unpacked
	@(mkdir -p build/${PATIENT}/rootfs ; \
	  tar xzf build/${PATIENT}/webos/$(NOVA_IMAGE_NAME) -C build/${PATIENT}/rootfs 2> /dev/null) || true
	@(touch build/${PATIENT}/rootfs/.unpacked)


###############################################################################
# download stuff
##############################################################################

downloads:
	@(mkdir -p downloads)

.PHONY: wget-kernel
wget-kernel: downloads
	@echo "Downloading kernel for your device ..."
	wget -N --directory-prefix=downloads/ ${DOWNLOAD_BASE_URL}/$(KERNELFILENAME)

.PHONY: wget-image
wget-image: downloads
	@echo "Downloading image for your device ..."
	wget -N --directory-prefix=downloads/ ${DOWNLOAD_BASE_URL}/$(IMAGEFILENAME)

.PHONY: wget-bootr
wget-bootr: downloads
	@echo "Downloading Bootr for your device ..."
	wget --no-check-certificate -N --directory-prefix=downloads/ $(BOOTRURL)

.PHONY: wget-patch
wget-patch: downloads
	@echo "Downloading GNU Patch for your device ..."
	wget -N --directory-prefix=downloads/ ${PATCH_URL}/$(PATCH_IPK)

.PHONY: wget-stuff
wget-stuff: wget-kernel wget-image wget-bootr wget-patch

###############################################################################
# Device preparation
###############################################################################

.PHONY: memload
memload: build/${PATIENT}/webos/.unpacked
	@echo "Transfer nova installer to device and boot it ..."
	@novacom boot mem:// < build/${PATIENT}/webos/${NOVAINSTALLER}
	@echo "Wait for device to come back ..."
	@sleep 10

.PHONY: memboot 
memboot: build/${PATIENT}/webos/.unpacked recover
	@echo "Transfer nova installer to device and reboot your device with it ..."
	@novacom -w boot mem:// < build/${PATIENT}/webos/${NOVAINSTALLER} 2> /dev/null || true
	@echo "Wait for device to come back ..."
	@sleep ${TIME_TO_WAIT_FOR_DEVICE}

.PHONY: reboot
reboot:
	@echo "Rebooting your device ..."
	@novacom -w run file://sbin/tellbootie || true
	@sleep 5
	@echo "Done!"

.PHONY: recover
recover:
	@echo "Rebooting your device in recovery mode ..."
	@novacom -w run file://sbin/tellbootie recover 2> /dev/null || true
	@sleep 5
	@echo "Done!"

.PHONY: backup
backup: mount
	@echo "Backuping all your device..."
	@export id="`novacom -w run file://bin/cat -- /proc/nduid | cut -c 1-8`" ; \
	mkdir -p backup/$$id ; \
	echo " Creating backup/$$id/mmcblk0p1.bak" ; \
	( novacom -w run file://bin/dd if=/dev/mmcblk0p1 ) | \
	  dd of=backup/$$id/mmcblk0p1.bak ; \
	echo " Creating backup/$$id/varfs.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/var/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/varfs.tar.gz ; \
	echo " Creating backup/$$id/rootfs.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/root/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/rootfs.tar.gz ; \
	echo " Creating backup/$$id/boot.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/boot/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/boot.tar.gz ; \
	echo " Creating backup/$$id/media.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/media/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/media.tar.gz ; \

.PHONY: lvm-setup
lvm-setup:
	@novacom -w run file://usr/sbin/lvm.static -- vgscan --ignorelockingfailure 2> /dev/null || true
	@novacom -w run file://usr/sbin/lvm.static -- vgchange -ay --ignorelockingfailure 2> /dev/null || true

.PHONY: mount
mount: unmount lvm-setup
	@echo "Mounting partitions ..."
	@for f in var root media ; do \
	  echo " Mounting /dev/mapper/store-$$f" ; \
	  novacom -w run file://bin/mkdir -- -p /tmp/$$f ; \
	  novacom -w run file://bin/mount -- /dev/mapper/store-$$f /tmp/$$f -o ro ; \
	done
	@echo " Mounting ${BOOT_PARTITION}"
	@novacom -w run file://bin/mkdir -- -p /tmp/boot
	@novacom -w run file://bin/mount -- ${BOOT_PARTITION} /tmp/boot -o rw

.PHONY: unmount
unmount:
	@echo "Unmounting partitions ..."
	@for f in var root media boot ; do \
	  echo " Unmounting /tmp/$$f" ; \
	  ( novacom -w run file://bin/umount -- /tmp/$$f 2> /dev/null || true ) ; \
	done
	@echo " Unmounting ${BOOT_PARTITION}"
	@novacom -w run file://bin/umount -- ${BOOT_PARTITION} 2> /dev/null || true

.PHONY: unmount-fso
unmount-fso:
	@echo "Unmounting FSO partition ..."
	@novacom -w run file://bin/umount -- /tmp/fso 2> /dev/null || true

.PHONY: mount-fso
mount-fso: lvm-setup unmount-fso
	@echo "Mounting FSO partition ..."
	@novacom -w run file://bin/mkdir -- -p /tmp/fso
	@novacom -w run file://bin/mount -- /dev/mapper/store-fso /tmp/fso -o rw

.PHONY: lvm-resize
lvm-resize: unmount unmount-fso lvm-setup
	@echo "Now we are resizing lvm partition to get some space for FSO/SHR ..."
	@novacom -w run file://usr/sbin/lvm.static -- lvresize -f -L $(CONFIG_WEBOS_MEDIA_TARGET_SIZE) /dev/mapper/store-media
	@novacom -w run file://usr/sbin/mkdosfs -- -F 32 /dev/mapper/store-media
	@novacom -w run file://usr/sbin/lvm.static -- lvcreate -l 100%FREE -n fso store
	@novacom -w run file://sbin/mkfs.ext3 /dev/store/fso
	@echo "Resizing lvm partition done!"

###############################################################################
# Image flashing + erasing
###############################################################################

.PHONY: erase-image
erase-image: unmount-fso
	@echo "Erasing old image. This may take some time ..."
	@novacom -w run file://sbin/mkfs.ext3 /dev/store/fso
	@echo "Erasing done!"

.PHONY: flash-image
flash-image: erase-image mount-fso
ifneq (${IMAGE},undefined)
	@if [ ! -f ${IMAGE} ] ; then \
		echo "${IMAGE} is missing / You have to specify an correct image file (.tar.gz)!" ; false ; \
	fi
	@echo "Starting to flash the given image file '$(IMAGE)' to the device ..."
	@novacom -w run file://bin/rm -- -rf /tmp/fso/*
	@novacom -w run file://bin/tar -- -xz -C /tmp/fso -f - < $(IMAGE)
	@make --no-print-directory TARGET=${TARGET} unmount-fso
	@echo "Image was flashed successfully!"
else
	@echo "downloads/${IMAGE} is missing / You have to specify an correct image file (.tar.gz)!"
endif

.PHONY: flash-kernel
flash-kernel: mount
ifneq (${KERNEL},undefined)
	@if [ ! -f ${KERNEL} ] ; then \
		echo "${KERNEL} is missing / You have to specify an correct kernel image file (uImage)!" ; false ; \
	fi

	@echo "Starting to flash the given kernel file '$(KERNEL)' to the device ..."
	@novacom put file://tmp/boot/$(KERNELFILENAME) < $(KERNEL)
	@make --no-print-directory TARGET=${TARGET} unmount
else
	@echo "downloads/${KERNEL} is missing / You have to specify an correct kernel image file (uImage)!" 
endif

.PHONY: install-patch
install-patch: wget-patch
	@echo "Temporary installing GNU Patch to your device ..."
	@novacom put file://$(shell basename $(PATCH_IPK)) < downloads/$(PATCH_IPK)
	@novacom -w run file://usr/bin/ipkg -- install /$(shell basename $(PATCH_IPK)) 2> /dev/null || true
	@sleep 1
	@echo "GNU Patch installed!"

.PHONY: install-bootr
install-bootr: unmount wget-bootr install-patch
	@echo "Installing Bootr to your device ..."
	@novacom -w run file://bin/mount -- ${BOOT_PARTITION} /boot
	@novacom -w run file://bin/tar -- -xz -C /boot -f - < downloads/$(BOOTR)
	@novacom -w run file://boot/bootr/bin/install.sh install
	@novacom -w run file://bin/umount -- ${BOOT_PARTITION}
	@echo "Bootr installed!"

.PHONY: restore-media
restore-media:
	@echo "Restoring your data to /media/internal ..."
	@novacom -w run file://bin/mkdir -- -p /tmp/media
	@novacom -w run file://bin/mount -- /dev/mapper/store-media /tmp/media -o rw
	@export id="`novacom -w run file://bin/cat -- /proc/nduid | cut -c 1-8`" ; \
	novacom -w run file://bin/tar -- -xz -C /tmp/media -f - < backup/$$id/media.tar.gz; \
	novacom -w run file://bin/umount -- /tmp/media
	@echo "Restoring done!"

.PHONY: install-all
install-all:
	@echo "==> The installation will begin ..."
	@echo "==> Step 1/9 : downloading stuff, please wait ..."
	@make --no-print-directory wget-stuff
	@echo "==> Step 2/9 : rebooting your device, please wait ..."
	@make --no-print-directory memboot
	@echo "==> Step 3/9 : backuping your device, please wait ..."
	@make --no-print-directory backup
	@echo "==> Step 4/9 : partition layout, please wait ..."
	@make --no-print-directory lvm-resize
	@echo "==> Step 5/9 : restoring your data, please wait ..."
	@make --no-print-directory restore-media
	@echo "==> Step 6/9 : flashing the image, please wait ..."
	@make --no-print-directory flash-image
	@echo "==> Step 7/9 : flashing the kernel, please wait ..."
	@make --no-print-directory flash-kernel
	@echo "==> Step 8/9 : installing booting stuff, please wait ..."
	@make --no-print-directory install-bootr
	@echo "==> Step 9/9 : rebooting ..."
	@make --no-print-directory reboot
	@echo "==> Installation complete! Congratulations!"

.PHONY: update-kernel
update-kernel: mount
	@echo "Updating your Kernel ..."
	@novacom put file://tmp/boot/$(KERNELFILENAME) < $(KERNEL)
	@echo "Kernel updated!"

.PHONY: update-rootfs
update-rootfs: erase-image mount-fso
	@echo "Updating your rootfs ..."
	@novacom -w run file://bin/tar -- -xz -C /tmp/fso -f - < $(IMAGE)
	@echo "Root filesystem updated!"

.PHONY: update-all
update-all: wget-image wget-kernel memboot update-kernel update-rootfs reboot
	@echo FSO/SHR up to date!

.PHONY: neuralyze-me
neuralyze-me: memboot
	@echo "This will restore your device like you had never seen us ..."
	@novacom -w run file://bin/mkdir -- -p /tmp/media
	@novacom -w run file://bin/mount -- /dev/mapper/store-media /tmp/media -o rw
	@export id="`novacom -w run file://bin/cat -- /proc/nduid | cut -c 1-8`" ; \
	mkdir -p backup/$$id ; \
	echo " Creating backup/$$id/media.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/media/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/media.tar.gz ; \
	echo "Backup Done!"
	@novacom -w run file://bin/umount -- /tmp/media
	@echo "lvm layout rearrangement"
	@make --no-print-directory lvm-setup
	@novacom -w run file://usr/sbin/lvm.static -- lvremove -f /dev/mapper/store-media
	@novacom -w run file://usr/sbin/lvm.static -- lvremove -f /dev/mapper/store-fso
	@novacom -w run file://usr/sbin/lvm.static -- lvcreate -l 100%FREE -n media store
	@novacom -w run file://usr/sbin/mkdosfs -- -F 32 /dev/mapper/store-media
	@echo "lvm layout rearranged!"
	@make --no-print-directory restore-media
	@novacom -w run file://bin/mount -- ${BOOT_PARTITION} /boot
	@novacom -w run file://bin/rm -- -rf /boot/$(KERNELFILENAME)
	@echo "Kernel removed!"
	@novacom -w run file://boot/bootr/bin/install.sh uninstall
	@novacom -w run file://bin/mount -- -o remount,rw /boot
	@novacom -w run file://bin/rm -- -rf /boot/bootr/
	@echo "Bootr removed!"
	@novacom -w run file://bin/umount -- ${BOOT_PARTITION}
	@make --no-print-directory reboot


###############################################################################
# Installing additional files
###############################################################################

.PHONY: setup-extra
setup-extra: build/${PATIENT}/rootfs/.unpacked
	@echo "Copying all extra files to extra directory ..."
	@sh scripts/setup-extra.sh

.PHONY: install-extra
install-extra: mount-fso
	@if [ -d extra ] ; then \
		echo "Installing extra files to device ..." ; \
		rm -f build/extrafiles.tar ; \
		( cd extra ; tar cf ../build/extra-files.tar * ); \
		novacom -w run file://bin/tar -- x -C /tmp/fso -f - < build/extra-files.tar 2> /dev/null || true; \
	fi


