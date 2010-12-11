
CONFIG_WEBOS_MEDIA_TARGET_SIZE = 6.0G

all:

build/unpacked-doctor:
	@mkdir -p build/unpacked-doctor

build/unpacked-doctor/resources: build/unpacked-doctor downloads/webosdoctorp100ueu-wr.jar
	@echo "Extracting webos doctor ..."
	(cd build/unpacked-doctor; jar -xf ../../downloads/webosdoctorp100ueu-wr.jar)

build/unpacked-doctor/resources/nova-installer-image-castle.uImage: build/unpacked-doctor/resources
	(cd build/unpacked-doctor/resources; tar xf webOS.tar)

.PHONY: memload
memload: build/unpacked-doctor/resources/nova-installer-image-castle.uImage
	@echo "Transfer nova installer to device and boot it ..."
	novaterm boot mem:// < build/unpacked-doctor/resources/nova-installer-image-castle.uImage
	@echo "Wait for device to come back ..."
	@sleep 10

.PHONY: memboot 
memboot: recover build/unpacked-doctor/resources/nova-installer-image-castle.uImage
	novacom -w boot mem:// < build/unpacked-doctor/resources/nova-installer-image-castle.uImage

.PHONY: reboot
reboot:
	novacom -w run file://sbin/tellbootie || true

.PHONY: recover
recover:
	novacom -w run file://sbin/tellbootie recover || true

.PHONY: backup
backup: mount
	@export id="`novacom -w run file://bin/cat -- /proc/nduid | cut -c 1-8`" ; \
	mkdir -p backup/$$id ; \
	echo "Creating backup/$$id/mmcblk0p1.bak" ; \
	( novacom -w run file://bin/dd if=/dev/mmcblk0p1 ) | \
	  dd of=backup/$$id/mmcblk0p1.bak ; \
	echo "Creating backup/$$id/varfs.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/var/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/varfs.tar.gz ; \
	echo "Creating backup/$$id/rootfs.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/root/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/rootfs.tar.gz ; \
	echo "Creating backup/$$id/boot.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/boot/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/boot.tar.gz ; \
	echo "Creating backup/$$id/media.tar.gz" ; \
	( novacom -w run file://bin/tar -- -C /tmp/media/ --totals -cf - . ) | \
	  gzip -c > backup/$$id/media.tar.gz ; \

.PHONY: lvm-setup
lvm-setup:
	novacom -w run file://usr/sbin/lvm.static -- vgscan --ignorelockingfailure 2> /dev/null
	novacom -w run file://usr/sbin/lvm.static -- vgchange -ay --ignorelockingfailure 2> /dev/null

.PHONY: mount
mount: unmount lvm-setup
	@for f in var root media ; do \
	  echo "Mounting /dev/mapper/store-$$f" ; \
	  novacom -w run file://bin/mkdir -- -p /tmp/$$f ; \
	  novacom -w run file://bin/mount -- /dev/mapper/store-$$f /tmp/$$f -o ro ; \
	done
	@echo "Mounting /dev/mmcblk0p2"
	@novacom -w run file://bin/mkdir -- -p /tmp/boot
	@novacom -w run file://bin/mount -- /dev/mmcblk0p2 /tmp/boot -o rw

.PHONY: unmount
unmount:
	@for f in var root media boot ; do \
	  echo "Unmounting /tmp/$$f" ; \
	  ( novacom -w run file://bin/umount -- /tmp/$$f 2> /dev/null || true ) ; \
	done

.PHONY: umount-fso
unmount-fso:
	novacom -w run file://bin/umount -- /tmp/fso 2> /dev/null || true

.PHONY: mount-fso
mount-fso: lvm-setup unmount-fso
	novacom -w run file://bin/mkdir -- -p /tmp/fso
	novacom -w run file://bin/mount -- /dev/mapper/store-fso /tmp/fso -o rw

.PHONY: lvm-resize
lvm-resize: unmount unmount-fso lvm-setup
	@echo "Now we are resizing lvm partition to get some space for FSO/SHR ..."
	novacom -w run file://usr/sbin/lvm.static -- lvresize -f -L $(CONFIG_WEBOS_MEDIA_TARGET_SIZE) /dev/mapper/store-media 2> /dev/null || true
	novacom -w run file://usr/sbin/mkdosfs -- -F 32 /dev/mapper/store-media 2> /dev/null || true
	novacom -w run file://usr/sbin/lvm.static -- lvcreate -l 100%FREE -n fso store 2> /dev/null || true
	novacom -w run file://sbin/mkfs.ext3 /dev/store/fso 2> /dev/null || true

.PHONY: flash-image
flash-image: mount-fso 
ifneq (${IMAGE},undefined)
	@if [ "${IMAGE}" == "" ] ; then \
		echo "You have to specify an correct image file (.tar.gz)!" ; false ; \
	fi

	@echo "Starting to flash the given image file '$(IMAGE)' to the device ..."
	novacom -w run file://bin/rm -- -rf /tmp/fso/*
	novaterm put file://tmp/fso/$(shell basename $(IMAGE)) < $(IMAGE)
	novacom -w run file://bin/tar -- -xzf /tmp/fso/$(shell basename $(IMAGE)) -C /tmp/fso 2> /dev/null || true
	novacom -w run file://bin/rm -- /tmp/fso/$(shell basename $(IMAGE)) 2> /dev/null || true
else
	@echo "You have to specify an correct image file (.tar.gz)!"
endif

.PHONY: flash-kernel
flash-kernel: mount
ifneq (${IMAGE},undefined)
	@if [ "${IMAGE}" == "" ] ; then \
		echo "You have to specify an correct kernel image file (uImage)!" ; false ; \
	fi

	novacom -w run file://bin/rm -- /tmp/boot/uImage 2> /dev/null || true
	novaterm put file://tmp/boot/$(shell basename $(IMAGE)) < $(IMAGE) 2> /dev/null || true
	novacom -w run file://bin/ln -- -s $(shell basename $(IMAGE)) /tmp/boot/uImage 2> /dev/null || true

else
	@echo "You have to specify an correct kernel image file (uImage)!" 
endif

clean: 
	@echo "Cleanup everything ..."
	@rm -rf build deps

