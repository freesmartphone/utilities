LIBS = $(shell grep /usr/lib ${IPKG_FILES_LIST})

install-libs:: $(foreach LIB,${LIBS},${ROOTDIR}${LIB})

${ROOTDIR}/% : ${STAGING_DIR}/%
	mkdir -p $(@D)
	rm -f $@
	cp -pR $< $@

clobber::
	rm -f $(foreach LIB,${LIBS},${STAGING_DIR}${LIB})
