#TODO :
# verify all install-all steps by step when done in a raw
# should we call unmount

#####################
# Select "pre" or "pre2".
TARGET = pre
# define the size you want to let on your media partition of WebOS: 00.5G < X < 07.5G (15.5G for a pre2)
# It NEEDS to be formated as XX.XG
CONFIG_WEBOS_MEDIA_TARGET_SIZE = 06.0G


#DO NOT MODIFY ANY OF THOSE FOLLOWING LINES
#####################
DOWNLOADDIRECTORY= downloads
SUPPORTEDDEVICES = pre pre2
ifeq ($(filter ${TARGET},${SUPPORTEDDEVICES}),)
$(error You did not enter a proper TARGET)
endif

ifneq ($(shell echo ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}|wc -c),6)
$(error Wrong CONFIG_WEBOS_MEDIA_TARGET_SIZE format!!)
endif

ifeq (${TARGET},pre)
NOVAINSTALLER = nova-installer-image-castle.uImage
IMAGE ?= $(DOWNLOADDIRECTORY)/lite-palmpre.tar.gz
KERNEL ?= $(DOWNLOADDIRECTORY)/uImage-palmpre.bin
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
IMAGE ?= $(DOWNLOADDIRECTORY)/lite-palmpre2.tar.gz
KERNEL ?= $(DOWNLOADDIRECTORY)/uImage-palmpre2.bin
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '<' '00.5G'),1)
$(error CONFIG_WEBOS_MEDIA_TARGET_SIZE needs to be bigger than 00.5G)
endif
ifeq ($(shell expr '${CONFIG_WEBOS_MEDIA_TARGET_SIZE}' '>' '15.5G'),1)
$(error $(shell echo |awk '{print 16.0 - ${CONFIG_WEBOS_MEDIA_TARGET_SIZE}}')G is left to FSO/SHR, It is too small .\
 You need to decrease CONFIG_WEBOS_MEDIA_TARGET_SIZE)
endif
endif


all:

build/unpacked-doctor:
	@mkdir -p build/unpacked-doctor

build/unpacked-doctor/resources: build/unpacked-doctor check-webosdoctor
	@echo "Extracting webos doctor ..."
	@(cd build/unpacked-doctor; jar -xf ../../downloads/webosdoctor*.jar)

build/unpacked-doctor/resources/nova-installer-image: build/unpacked-doctor/resources
	@(cd build/unpacked-doctor/resources; tar xf webOS.tar)


#####################
# download stuff

downloads-dir:
ifeq ($(wildcard downloads/),) 
	@mkdir downloads
	@echo "downloads/ directory created"
else
	@echo "downloads/ directory already present"
endif

wget-kernel: downloads-dir
ifeq (${TARGET},pre)
	@echo "Downloading kernel for your pre ..."
	wget -N --directory-prefix=downloads/ http://amethyst.openembedded.net/~morphis/oe/palmpre/deploy/images/palmpre/uImage-palmpre.bin
endif
ifeq (${TARGET},pre2)
	@echo "Downloading kernel for your pre2 ..."
	wget -N --directory-prefix=downloads/ http://amethyst.openembedded.net/~morphis/oe/palmpre/deploy/images/palmpre2/uImage-palmpre2.bin
endif

wget-image: downloads-dir
ifeq (${TARGET},pre)
	@echo "Downloading image for your pre ..."
	wget -N --directory-prefix=downloads/ http://amethyst.openembedded.net/~morphis/oe/palmpre/deploy/images/palmpre/lite-palmpre.tar.gz
endif
ifeq (${TARGET},pre2)
	@echo "Downloading image for your pre2 ..."
	wget -N --directory-prefix=downloads/ http://amethyst.openembedded.net/~morphis/oe/palmpre/deploy/images/palmpre2/lite-palmpre2.tar.gz
endif

check-webosdoctor:
ifeq ($(wildcard downloads/webosdoctor*.jar),) 
	@echo "You have to download WebOS Doctor jar yourself and put it in downloads/ , take a look here http://www.webos-internals.org/wiki/Webos_Doctor_Versions " && false
else
	@echo "downloads/webosdoctor*.jar present"
endif


.PHONY: wget-stuff
wget-stuff: wget-kernel wget-image check-webosdoctor

#
###################

.PHONY: memload
memload: build/unpacked-doctor/resources/nova-installer-image
	@echo "Transfer nova installer to device and boot it ..."
	@novaterm boot mem:// < build/unpacked-doctor/resources/${NOVAINSTALLER}
	@echo "Wait for device to come back ..."
	@sleep 10


.PHONY: memboot 
memboot: recover build/unpacked-doctor/resources/nova-installer-image
	@echo "Transfer nova installer to device and reboot your device with it ..."
	@novacom -w boot mem:// < build/unpacked-doctor/resources/${NOVAINSTALLER}


.PHONY: reboot
reboot:
	@echo "Rebooting your device ..."
	@novacom -w run file://sbin/tellbootie || true
	@sleep 5
	@echo "Done!"


.PHONY: recover
recover:
	@echo "Rebooting your device in recovery mode..."
	@novacom -w run file://sbin/tellbootie recover || true
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
	#NOTE: do we print something ?
	novacom -w run file://usr/sbin/lvm.static -- vgscan --ignorelockingfailure 2> /dev/null || true
	novacom -w run file://usr/sbin/lvm.static -- vgchange -ay --ignorelockingfailure 2> /dev/null || true


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
	novacom -w run file://bin/umount -- /tmp/fso 2> /dev/null || true


.PHONY: mount-fso
mount-fso: lvm-setup unmount-fso
	@echo "Mounting FSO partition ..."
	novacom -w run file://bin/mkdir -- -p /tmp/fso
	novacom -w run file://bin/mount -- /dev/mapper/store-fso /tmp/fso -o rw


.PHONY: lvm-resize
lvm-resize: unmount unmount-fso lvm-setup
	@echo "Now we are resizing lvm partition to get some space for FSO/SHR ..."
	novacom -w run file://usr/sbin/lvm.static -- lvresize -f -L $(CONFIG_WEBOS_MEDIA_TARGET_SIZE) /dev/mapper/store-media 2> /dev/null || true
	novacom -w run file://usr/sbin/mkdosfs -- -F 32 /dev/mapper/store-media 2> /dev/null || true
	novacom -w run file://usr/sbin/lvm.static -- lvcreate -l 100%FREE -n fso store 2> /dev/null || true
	novacom -w run file://sbin/mkfs.ext3 /dev/store/fso 2> /dev/null || true

.PHONY: erase-image
erase-image: unmount-fso
	@echo "Erasing old image; This may take some time ..."
	novacom -w run file://sbin/mkfs.ext3 -- /dev/store/fso &> /dev/null || true

.PHONY: flash-image
flash-image: erase-image mount-fso
ifneq (${IMAGE},undefined)
	@if [ "${IMAGE}" == "" ] ; then \
		echo "downloads/${IMAGE} is missing / You have to specify an correct image file (.tar.gz)!" ; false ; \
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
	@if [ "${KERNEL}" == "" ] ; then \
		echo "downloads/${KERNEL} is missing / You have to specify an correct kernel image file (uImage)!" ; false ; \
	fi

	@echo "Starting to flash the given kernel file '$(KERNEL)' to the device ..."
	novaterm put file://tmp/boot/$(shell basename $(KERNEL)) < $(KERNEL)


else
	@echo "downloads/${KERNEL} is missing / You have to specify an correct kernel image file (uImage)!" 
endif


.PHONY: setup-init
setup-init: mount
	@echo "Setting up init script ..."
	novacom -w run file://bin/cp -- /tmp/boot/sbin/init /tmp/boot/sbin/init.fso
	novacom -w run file://bin/mv -- /tmp/boot/sbin/init /tmp/boot/sbin/init.webos
	echo -e "#!/bin/sh \n cd /tmp/boot/sbin ;\n ln -s init.fso init ;\n" >> switch
	chmod +x switch
	novaterm put file://tmp/switch < switch
	rm switch
	novacom -w run file://tmp/switch
	novacom -w run file://bin/sed -- -i -e 's#/dev/mapper/store-root#/dev/mapper/store-fso#g' -e 's#\ ro\ #\ rw\ #g' /tmp/boot/sbin/init.fso
	novacom -w run file://bin/rm -- /tmp/boot/uImage 2> /dev/null || true
	novacom -w run file://bin/ln -- -s $(shell basename $(KERNEL)) /tmp/boot/uImage 2> /dev/null || true


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


.PHONY: install-all												#TODO TEST IT!
install-all: wget-stuff memboot backup lvm-resize restore-media flash-image flash-kernel setup-init reboot


.PHONY: switch-boot
switch-boot: memload mount
	@if [ $$(novacom -w run file://bin/ls -- -l /tmp/boot/sbin/init | cut -c 85-) == "webos" ] ; then \
		echo "Switching to FSO" ; \
		echo -e "#!/bin/sh \n cd /tmp/boot/sbin ;\n rm init ;\n ln -s init.fso init ;\n cd /tmp/boot ;\n rm uImage ;\n ln -s ${KERNEL} uImage ;\n" > switch ; \
		chmod +x switch ; \
		novaterm put file://tmp/switch < switch ; \
		rm switch ; \
		novacom -w run file://tmp/switch ; \
	elif [ $$(novacom -w run file://bin/ls -- -l /tmp/boot/sbin/init | cut -c 85-) == "fso" ] ; then \
		echo "Switching to WebOS" ; \
		echo -e "#!/bin/sh \n cd /tmp/boot/sbin ;\n rm init ;\n ln -s init.webos init ;\n cd /tmp/boot ;\n rm uImage ;\n ln -s uImage-2.6.24-palm* uImage ;\n" > switch ; \
		chmod +x switch ; \
		novaterm put file://tmp/switch < switch ; \
		rm switch ; \
		novacom -w run file://tmp/switch ; \
	fi
	@echo "Done!"

.PHONY: update-kernel
update-kernel: mount wget-kernel
	@if [ $(shell date +%s -d '- $(shell stat -c%X $(KERNEL)) seconds') -le 10 ] ; then \
		echo "Updating your Kernel ..." ; \
		novaterm put file://tmp/boot/$(shell basename $(KERNEL)) < $(KERNEL) ; \
		echo "Kernel updated!" ; \
	else \
		echo "Nothing to do, you have already the last kernel" ;\
	fi


.PHONY: update-rootfs
update-rootfs: mount-fso wget-image
	@if [ $(shell date +%s -d '- $(shell stat -c%X $(IMAGE)) seconds') -le 10 ] ; then \
		echo "Removing old rootfs and creating new one ..." ; \
		echo -e "#!/bin/sh \n rm -rf /tmp/fso/* ;\n" > eraser ; \
		chmod +x eraser ; \
		novaterm put file://tmp/eraser < eraser ; \
		rm eraser ; \
		novacom -w run file://tmp/eraser ; \
		novaterm put file://tmp/fso/$(shell basename $(IMAGE)) < $(IMAGE) ;\
		novacom -w run file://bin/tar -- -xzf /tmp/fso/$(shell basename $(IMAGE)) -C /tmp/fso 2> /dev/null || true;\
		novacom -w run file://bin/rm -- /tmp/fso/$(shell basename $(IMAGE)) ;\
		echo "Root filesystem updated!" ; \
	else \
		echo "Nothing to do, you have already the last rootfs" ;\
	fi


.PHONY: update-all
update-all: memload update-kernel update-rootfs
	@echo FSO/SHR up to date!

.PHONY: update
update: 
	@echo TODO? update this makefile ?

clean: 
	@echo "Cleanup everything ..."
	@rm -rf build deps
