# Makefile for PreWare downloads
#
# Copyright (C) 2009 by Rod Whitby <rod@whitby.id.au>
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

.PHONY: download

DL_DIR = ../../downloads

PREWARE_SANITY =
ifndef VERSION
PREWARE_SANITY += $(error "Please define VERSION in your Makefile")
endif

ifdef SRC_TGZ

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-${VERSION}.tar.gz

${DL_DIR}/${NAME}-${VERSION}.tar.gz:
	rm -f $@ $@.tmp
	$(call PREWARE_SANITY)
	mkdir -p ${DL_DIR}
	curl -f -R -L -o $@.tmp ${SRC_TGZ}
	mv $@.tmp $@

endif

ifdef SRC_BZ2

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-${VERSION}.tar.bz2

${DL_DIR}/${NAME}-${VERSION}.tar.bz2:
	rm -f $@ $@.tmp
	$(call PREWARE_SANITY)
	mkdir -p ${DL_DIR}
	curl -f -R -L -o $@.tmp ${SRC_BZ2}
	mv $@.tmp $@

endif

ifdef SRC_ZIP

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-${VERSION}.zip

${DL_DIR}/${NAME}-${VERSION}.zip:
	rm -f $@ $@.tmp
	$(call PREWARE_SANITY)
	mkdir -p ${DL_DIR}
	curl -f -R -L -o $@.tmp ${SRC_ZIP}
	mv $@.tmp $@

endif

ifdef SRC_GIT

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-${VERSION}.tar.gz

${DL_DIR}/${NAME}-${VERSION}.tar.gz:
	rm -f $@
	$(call PREWARE_SANITY)
	rm -rf build
	mkdir build
	( cd build ; git clone -n ${SRC_GIT} ; cd `basename ${SRC_GIT} .git` ; git checkout v${VERSION} )
	mkdir -p ${DL_DIR}
	tar -C build/`basename ${SRC_GIT} .git` -zcf $@ .
	( cd build/`basename ${SRC_GIT} .git` ; git log --pretty="format:%ct" -n 1 v${VERSION} ) | \
	python -c 'import os,sys; time = int(sys.stdin.read()); os.utime("$@",(time,time));'
endif

ifdef SRC_SVN

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-${VERSION}.tar.gz

${DL_DIR}/${NAME}-${VERSION}.tar.gz:
	rm -f $@
	$(call PREWARE_SANITY)
	rm -rf build
	mkdir build
	( cd build ; svn checkout ${SRC_SVN} )
	mkdir -p ${DL_DIR}
	tar -C build/`basename ${SRC_SVN}` -zcf $@ .
endif

ifdef SRC_SVN_REV

ifndef NAME
PREWARE_SANITY += $(error "Please define NAME in your Makefile")
endif

download: ${DL_DIR}/${NAME}-svn-r${VERSION}.tar.gz

${DL_DIR}/${NAME}-svn-r${VERSION}.tar.gz:
	rm -f $@
	$(call PREWARE_SANITY)
	rm -rf build
	mkdir build
	( cd build ; svn checkout -r ${VERSION} ${SRC_SVN_REV} )
	mkdir -p ${DL_DIR}
	tar -C build/`basename ${SRC_SVN_REV}` -zcf $@ .
endif

ifdef SRC_FILE

ifndef LOCAL_FILE
PREWARE_SANITY += $(error "Please define LOCAL_FILE in your Makefile")
endif

download: ${DL_DIR}/${LOCAL_FILE}

${DL_DIR}/${LOCAL_FILE}:
	rm -f $@ $@.tmp
	$(call PREWARE_SANITY)
	mkdir -p ${DL_DIR}
	curl -f -R -L -o $@.tmp ${SRC_FILE}
	mv $@.tmp $@

endif

