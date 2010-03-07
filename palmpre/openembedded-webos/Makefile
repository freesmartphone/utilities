DOCTORPRE  = webosdoctorp100ewwsprint-1.3.5.jar
ROOTDIR = /

ifeq ($(shell uname -s),Darwin)
TAR	 = gnutar
MD5SUM	 = md5
FAKEROOT = sudo
else
TAR	 = tar
MD5SUM	 = md5sum
FAKEROOT = fakeroot
endif

.PHONY: all
all: rootfs stage

.PHONY: rootfs
rootfs: rootfs/armv7/.unpacked

stage: rootfs staging-armv7

.PHONY: staging-%
staging-%: staging/mapping-%
	for f in `find packages -mindepth 1 -maxdepth 1 -type d -print` ; do \
	  if [ -e $$f/Makefile ]; then \
	    ${MAKE} -C $$f ARCH=$* stage || exit ; \
	  fi; \
	done
	
.PRECIOUS: staging/mapping-%
staging/mapping-%:
	mkdir -p staging/$*/usr

rootfs/armv7/.unpacked: doctors/webosdoctorp100ewwsprint-1.3.5.jar
	rm -rf rootfs/armv7
	mkdir -p rootfs/armv7
	${FAKEROOT} scripts/unpack-doctor-rootfs $< rootfs/armv7
	touch $@
	
doctors/webosdoctorp100ewwsprint-1.3.5.jar:
	mkdir -p doctors
	wget -O $@ http://palm.cdnetworks.net/rom/pre/p135r0d12302009/sr1ntp135rod/webosdoctorp100ewwsprint.jar
	
.PHONY: install
install: 
	for f in `find packages -mindepth 1 -maxdepth 1 -type d -print` ; do \
	  if [ -e $$f/Makefile ]; then \
	    ${MAKE} -C $$f ROOTDIR=$(ROOTDIR) install || exit ; \
	  fi; \
	done
	

.PHONY: clean
clean:
	rm -f .*~ *~ scripts/*~ support/*~ packages/*/*~
	
.PHONY: clobber
clobber: clobber-armv7
	rm -rf rootfs staging

.PHONY: clobber-%
clobber-%:
	for f in `find packages -mindepth 1 -maxdepth 1 -type d -print` ; do \
	  if [ -e $$f/Makefile ]; then \
	    ${MAKE} -C $$f ARCH=$* clobber || exit ; \
	  fi; \
	done
