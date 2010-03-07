
FILES = $(shell grep / ${IPKG_FILES_LIST})

stage:: $(foreach FILE,${FILES},${STAGING_DIR}${FILE})

${STAGING_DIR}/% : ${ROOTFS_DIR}/%
	mkdir -p $(@D)
	rm -f $@
	cp -pR $< $@

clobber::
	rm -f $(foreach FILE,${FILES},${STAGING_DIR}${FILE})
