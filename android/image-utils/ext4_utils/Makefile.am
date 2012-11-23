AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libext4_utils.la

libext4_utils_la_SOURCES = \
	make_ext4fs.c \
	ext4fixup.c \
	ext4_utils.c \
	allocate.c \
	contents.c \
	extent.c \
	indirect.c \
	uuid.c \
	sha1.c \
	wipe.c \
	backed_block.c \
	sparse_crc32.c \
	output_file.c

bin_PROGRAMS = \
	make_ext4fs \
	ext2simg \
	simg2img \
	img2simg \
	ext4fixup

make_ext4fs_SOURCES = make_ext4fs_main.c
make_ext4fs_LDADD = \
	-lz \
	$(top_builddir)/ext4_utils/libext4_utils.la
make_ext4fs_LDFLAGS = -static

ext2simg_SOURCES = ext2simg.c
ext2simg_LDADD = \
	-lz \
	$(top_builddir)/ext4_utils/libext4_utils.la
ext2simg_LDFLAGS = -static

simg2img_SOURCES = simg2img.c
simg2img_LDADD = \
	-lz \
	$(top_builddir)/ext4_utils/libext4_utils.la
simg2img_LDFLAGS = -static

img2simg_SOURCES = img2simg.c
img2simg_LDADD = \
	-lz \
	$(top_builddir)/ext4_utils/libext4_utils.la
img2simg_LDFLAGS = -static

ext4fixup_SOURCES = ext4fixup_main.c
ext4fixup_LDADD = \
	-lz \
	$(top_builddir)/ext4_utils/libext4_utils.la
ext4fixup_LDFLAGS = -static
