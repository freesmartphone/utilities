LIBS = $(shell grep /usr/lib ${IPKG_FILES_LIST})

stage:: $(foreach LIB,${LIBS},${STAGING_DIR}${LIB})

${STAGING_DIR}/% : ${ROOTFS_DIR}/%
	mkdir -p $(@D)
	rm -f $@
	cp -pR $< $@

clobber::
	rm -f $(foreach LIB,${LIBS},${STAGING_DIR}${LIB})
