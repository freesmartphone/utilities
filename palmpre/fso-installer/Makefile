#
# Copyright (C) 2011 by Sylvain Paré <sylvain.pare@gmail.com>
#                       Simon Busch <morphis@gravedo.de>
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

# Select "pre", "preplus" or "pre2".
TARGET = unknown
# Define the size you want to let on your media partition of WebOS: 00.5G < X < 07.5G (15.5G for a pre2)
# It NEEDS to be formated as XX.XG
CONFIG_WEBOS_MEDIA_TARGET_SIZE = 05.5G

# Base URL for downloading kernels and images
ORIGIN_URL = http://build.shr-project.org/shr-unstable/images/
# Name of your webos doctor file
WEBOS_DOCTOR_NAME = webosdoctor*.jar

###############################################################################
# !!! DO NOT MODIFY ANY OF THE FOLLOWING LINES !!!
###############################################################################

DOWNLOADDIRECTORY = downloads
SUPPORTEDDEVICES = pre preplus pre2
ifeq ($(filter ${TARGET},${SUPPORTEDDEVICES}),)
$(error You did not enter a proper TARGET)
endif

ifneq ($(shell echo ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}|wc -c),6)
$(error Wrong CONFIG_WEBOS_MEDIA_TARGET_SIZE format!!)
endif

ifeq (${TARGET},preplus)
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '<' '00.5G'),1)
$(error CONFIG_WEBOS_MEDIA_TARGET_SIZE needs to be bigger than 00.5G)
endif
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '15.5G'),1)
$(error $(shell echo |awk '{print 8.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
# We just have to verify the size of the media partition for the preplus device
# different than for the pre device. All other things are the same so we switch
# over here to TARGET = pre
TARGET = pre
endif

ifeq (${TARGET},pre)
NOVAINSTALLER = nova-installer-image-castle.uImage
NOVA_IMAGE_NAME = nova-cust-image-castle.rootfs.tar.gz
IMAGEFILENAME = full-palmpre.tar.gz
KERNELFILENAME = uImage-palmpre.bin
BOOTR ?= bootr-palmpre_0.3.tar.gz
DOWNLOAD_BASE_URL = "${ORIGIN_URL}/palmpre"
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '<' '00.5G'),1)
$(error CONFIG_WEBOS_MEDIA_TARGET_SIZE needs to be bigger than 00.5G)
endif
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '07.5G'),1)
$(error $(shell echo |awk '{print 8.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
endif

ifeq (${TARGET},pre2)
NOVAINSTALLER = nova-installer-image-roadrunner.uImage
NOVA_IMAGE_NAME = nova-cust-image-roadrunner.rootfs.tar.gz
IMAGEFILENAME = full-palmpre2.tar.gz
KERNELFILENAME = uImage-palmpre2.bin
BOOTR ?= bootr-palmpre2_0.3.tar.gz
DOWNLOAD_BASE_URL = ${ORIGIN_URL}/palmpre2
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '<' '00.5G'),1)
$(error CONFIG_WEBOS_MEDIA_TARGET_SIZE needs to be bigger than 00.5G)
endif
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '15.5G'),1)
$(error $(shell echo |awk '{print 16.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
endif

IMAGE ?= $(DOWNLOADDIRECTORY)/$(IMAGEFILENAME)
KERNEL ?= $(DOWNLOADDIRECTORY)/$(KERNELFILENAME)
BOOTRURL = https://github.com/downloads/slyon/bootr/${BOOTR}

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

build/unpacked-doctor: downloads/$(WEBOS_DOCTOR_NAME)
	@mkdir -p build/unpacked-doctor

build/unpacked-doctor/resources/.unpacked: build/unpacked-doctor
	@echo "Extracting webos doctor ..."
	@if [ -x /usr/bin/jar ] ; then export unjar="jar -xf" ; elif [ -x /usr/bin/unzip ] ; then export unjar="unzip" ; fi ; \
	if [ "$$unjar" = "" ] ; then echo "Not found command for extract webos doctor / Install package default-jdk or unzip." ; false ; \
	else (cd build/unpacked-doctor; $$unjar ../../downloads/$(WEBOS_DOCTOR_NAME) > /dev/null || true ) ; fi
	@(touch build/unpacked-doctor/resources/.unpacked)

build/unpacked-doctor/webos/.unpacked: build/unpacked-doctor/resources/.unpacked
	@(mkdir -p build/unpacked-doctor/webos)
	@(tar xf build/unpacked-doctor/resources/webOS.tar -C build/unpacked-doctor/webos)
	@(touch build/unpacked-doctor/webos/.unpacked)

build/unpacked-doctor/rootfs/.unpacked: build/unpacked-doctor/webos/.unpacked
	@(mkdir -p build/unpacked-doctor/rootfs ; \
	  tar xzf build/unpacked-doctor/webos/$(NOVA_IMAGE_NAME) -C build/unpacked-doctor/rootfs 2> /dev/null) || true
	@(touch build/unpacked-doctor/rootfs/.unpacked)


###############################################################################
# download stuff
##############################################################################

downloads:
	@(mkdir -p downloads)

.PHONY: wget-kernel
wget-kernel: downloads
	@echo "Downloading kernel for your device ..."
	wget -N --directory-prefix=downloads/ ${DOWNLOAD_BASE_URL}/$(KERNELFILENAME)

.PHONY: wget-kernel
wget-image: downloads
	@echo "Downloading image for your device ..."
	wget -N --directory-prefix=downloads/ ${DOWNLOAD_BASE_URL}/$(IMAGEFILENAME)

PHONY: wget-bootr
wget-bootr: downloads
	@echo "Downloading Bootr for your device ..."
	wget --no-check-certificate -N --directory-prefix=downloads/ $(BOOTRURL)

.PHONY: wget-stuff
wget-stuff: wget-kernel wget-image wget-bootr

###############################################################################
# Device preparation
###############################################################################

.PHONY: memload
memload: build/unpacked-doctor/webos/.unpacked
	@echo "Transfer nova installer to device and boot it ..."
	@novaterm boot mem:// < build/unpacked-doctor/webos/${NOVAINSTALLER}
	@echo "Wait for device to come back ..."
	@sleep 10

.PHONY: memboot 
memboot: recover build/unpacked-doctor/webos/.unpacked
	@echo "Transfer nova installer to device and reboot your device with it ..."
	@novacom -w boot mem:// < build/unpacked-doctor/webos/${NOVAINSTALLER} 2> /dev/null || true

.PHONY: reboot
reboot:
	@echo "Rebooting your device ..."
	@novacom -w run file://sbin/tellbootie || true
	@sleep 5
	@echo "Done!"

.PHONY: recover
recover:
	@echo "Rebooting your device in recovery mode..."
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
	@echo " Mounting /dev/mmcblk0p2"
	@novacom -w run file://bin/mkdir -- -p /tmp/boot
	@novacom -w run file://bin/mount -- /dev/mmcblk0p2 /tmp/boot -o rw

.PHONY: unmount
unmount:
	@echo "Unmounting partitions ..."
	@for f in var root media boot ; do \
	  echo " Unmounting /tmp/$$f" ; \
	  ( novacom -w run file://bin/umount -- /tmp/$$f 2> /dev/null || true ) ; \
	done

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
	@novacom -w run file://usr/sbin/lvm.static -- lvresize -f -L $(CONFIG_WEBOS_MEDIA_TARGET_SIZE) /dev/mapper/store-media 2> /dev/null || true
	@novacom -w run file://usr/sbin/mkdosfs -- -F 32 /dev/mapper/store-media 2> /dev/null || true
	@novacom -w run file://usr/sbin/lvm.static -- lvcreate -l 100%FREE -n fso store 2> /dev/null || true
	@novacom -w run file://sbin/mkfs.ext3 /dev/store/fso 2> /dev/null || true

###############################################################################
# Image flashing + erasing
###############################################################################

.PHONY: erase-image
erase-image: unmount-fso
	@echo "Erasing old image. This may take some time ..."
	novacom -w run file://sbin/mkfs.ext3 -- /dev/store/fso &> /dev/null || true
	@sleep 20
	@echo "Erasing done!"

.PHONY: flash-image
flash-image: erase-image mount-fso
ifneq (${IMAGE},undefined)
	@if [ ! -f ${IMAGE} ] ; then \
		echo "${IMAGE} is missing / You have to specify an correct image file (.tar.gz)!" ; false ; \
	fi

	@echo "Starting to flash the given image file '$(IMAGE)' to the device ..."
	novacom -w run file://bin/rm -- -rf /tmp/fso/*
	novaterm put file://tmp/fso/$(shell basename $(IMAGE)) < $(IMAGE)
	novacom -w run file://bin/tar -- -xzf /tmp/fso/$(shell basename $(IMAGE)) -C /tmp/fso 2> /dev/null || true
	novacom -w run file://bin/rm -- /tmp/fso/$(shell basename $(IMAGE)) 2> /dev/null || true
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
	novaterm put file://tmp/boot/uImage-palmpre.bin < $(KERNEL)
else
	@echo "downloads/${KERNEL} is missing / You have to specify an correct kernel image file (uImage)!" 
endif

.PHONY: install-bootr
install-bootr: unmount wget-bootr
	@echo "Installing Bootr to your device ..."
	novacom -w run file://bin/mount -- /dev/mmcblk0p2 /boot
	novaterm put file://boot/$(shell basename $(BOOTR)) < downloads/$(BOOTR)
	novacom -w run file://bin/tar -- -xzf /boot/$(shell basename $(BOOTR)) -C /boot 2> /dev/null || true
	novacom -w run file://bin/rm -- /boot/$(shell basename $(BOOTR)) 2> /dev/null || true
	novacom -w run file://boot/bootr/bin/install.sh install
	@echo "Bootr installed!"

.PHONY: restore-media
restore-media:
	@echo "Restoring your data to /media/internal ..."
	novacom -w run file://bin/mkdir -- -p /tmp/media
	novacom -w run file://bin/mount -- /dev/mapper/store-media /tmp/media -o rw
	@export id="`novacom -w run file://bin/cat -- /proc/nduid | cut -c 1-8`" ; \
	novaterm put file://tmp/media/$(shell basename backup/$$id/media.tar.gz) < backup/$$id/media.tar.gz ; \
	novacom -w run file://bin/tar -- -xzf /tmp/media/$(shell basename backup/$$id/media.tar.gz) -C /tmp/media ; \
	novacom -w run file://bin/rm -- /tmp/media/$(shell basename backup/$$id/media.tar.gz) ; \
	novacom -w run file://bin/umount -- /tmp/media ; \

.PHONY: install-all
install-all: wget-stuff memboot backup lvm-resize restore-media flash-image flash-kernel install-bootr reboot

.PHONY: update-kernel
update-kernel: mount
	@echo "Updating your Kernel ..."
	novaterm put file://tmp/boot/$(shell basename $(KERNEL)) < $(KERNEL)
	@echo "Kernel updated!"

.PHONY: update-rootfs
update-rootfs: erase-image mount-fso
	@echo "Updating your rootfs ..."
	novaterm put file://tmp/fso/$(shell basename $(IMAGE)) < $(IMAGE)
	novacom -w run file://bin/tar -- -xzf /tmp/fso/$(shell basename $(IMAGE)) -C /tmp/fso 2> /dev/null || true
	novacom -w run file://bin/rm -- /tmp/fso/$(shell basename $(IMAGE))
	@echo "Root filesystem updated!"

.PHONY: update-all
update-all: wget-image wget-kernel memboot update-kernel update-rootfs reboot
	@echo FSO/SHR up to date!

###############################################################################
# Installing additional files
###############################################################################

.PHONY: setup-extra
setup-extra: build/unpacked-doctor/rootfs/.unpacked
	@echo "Copying all extra files to extra directory ..."
	@sh scripts/setup-extra.sh

.PHONY: install-extra
install-extra: mount-fso
	@if [ -d extra ] ; then \
		echo "Installing extra files to device ..." ; \
		rm -f build/extrafiles.tar ; \
		( cd extra ; tar cf ../build/extra-files.tar * ); \
		novaterm put file://tmp/extra-files.tar < build/extra-files.tar 2> /dev/null || true; \
		novacom -w run file://bin/tar -- xf /tmp/extra-files.tar -C /tmp/fso 2> /dev/null || true ; \
		novacom -w run file://bin/rm -- -f /tmp/extra-file.tar 2> /dev/null || true ; \
	fi

