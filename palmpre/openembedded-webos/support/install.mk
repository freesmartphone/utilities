
FILES = $(shell grep / ${IPKG_FILES_LIST})

install:: $(foreach FILE,${FILES},${ROOTDIR}${FILE})

${ROOTDIR}/% : ${STAGING_DIR}/%
	mkdir -p $(@D)
	rm -f $@
	cp -pR $< $@

uninstall::
	rm -f $(foreach FILE,${FILES},${ROOTDIR}${FILE})



