/* tools/mkbootimg/unbootimg.c
**
** Copyright 2007, The Android Open Source Project
** Copyright 2011, Robert Ham <rah@bash.sh>
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "bootimg.h"

static void *load_file(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops;
    close(fd);

    if(_sz) *_sz = sz;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

static int write_file(const char *fn, void *data, unsigned sz)
{
    int fd, ret = 0;

    fd = open(fn, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fd < 0) {
        fprintf(stderr, "error: could not open file `%s' for writing: %s\n", fn, strerror(errno));
	return -1;
    }

    if(write(fd, data, sz) != sz) {
        fprintf(stderr, "error: could not write to file `%s': %s\n", fn, strerror(errno));
	unlink(fn);
	ret = -1;
    }

    close(fd);
    return ret;
}

static int write_image(const char *img, char **page_ptr, const char *fn, unsigned sz, unsigned page_size)
{
    if (fn) {
        if(sz == 0)
	    fprintf(stderr,"warning: no %s in boot image\n", img);
	else
	    if (write_file(fn, (*page_ptr), sz)) return -1;
    }

    *page_ptr += ((sz + page_size - 1) / page_size) * page_size;

    return 0;
}

int usage(void)
{
    fprintf(stderr,"usage: unbootimg\n"
            "       [ --kernel <filename> ]\n"
            "       [ --ramdisk <filename> ]\n"
            "       [ --second <2ndbootloader-filename> ]\n"
            "       -i|--input <filename>\n"
            );
    return 1;
}


int main(int argc, char **argv)
{
    boot_img_hdr hdr;

    char *kernel_fn = 0;
    void *kernel_data = 0;
    char *ramdisk_fn = 0;
    void *ramdisk_data = 0;
    char *second_fn = 0;
    void *second_data = 0;
    char *cmdline = 0;
    char *bootimg = 0;
    char *board = 0;
    int fd;
    char *data;
    unsigned sz;
    char *page_ptr;
    unsigned i;

    argc--;
    argv++;

    while(argc > 0){
        char *arg = argv[0];
        char *val = argv[1];
        if(argc < 2) {
            return usage();
        }
        argc -= 2;
        argv += 2;
        if(!strcmp(arg, "--input") || !strcmp(arg, "-i")) {
            bootimg = val;
        } else if(!strcmp(arg, "--kernel")) {
            kernel_fn = val;
        } else if(!strcmp(arg, "--ramdisk")) {
            ramdisk_fn = val;
        } else if(!strcmp(arg, "--second")) {
            second_fn = val;
        } else {
            return usage();
        }
    }

    if(bootimg == 0) {
        fprintf(stderr,"error: no input filename specified\n");
        return usage();
    }


    data = load_file(bootimg, &sz);
    if (!data) {
        fprintf(stderr,"error loading boot image: $s\n", strerror(errno));
	return 1;
    }

    if (memcmp(data, BOOT_MAGIC, BOOT_MAGIC_SIZE) != 0) {
        fprintf(stderr,"error: supplied file is not an Android boot image\n");
        return 1;
    }

    memcpy(&hdr, data, sizeof(hdr));

    printf("total image size:   %u\n"
	   "kernel size:        %u\n"
	   "kernel load addr:   0x%x\n"
	   "ramdisk size:       %u\n"
	   "ramdisk load addr:  0x%x\n"
	   "2nd boot size:      %u\n"
	   "2nd boot load addr: 0x%x\n"
	   "kernel tags addr:   0x%x\n"
	   "page size:          %u\n"
	   "board:              `%s'\n"
	   "cmdline:            `%s'\n"
	   "id:                 ",
	   sz,
	   hdr.kernel_size, hdr.kernel_addr,
	   hdr.ramdisk_size, hdr.ramdisk_addr,
	   hdr.second_size, hdr.second_addr,
	   hdr.tags_addr, hdr.page_size, hdr.name, hdr.cmdline);
    for (i = 0; i < 8; ++i) printf("%x", hdr.id[i]);
    printf("\n");

    page_ptr = data + hdr.page_size;

    if (write_image("kernel",   &page_ptr, kernel_fn,  hdr.kernel_size,  hdr.page_size)) return 1;
    if (write_image("ramdisk",  &page_ptr, ramdisk_fn, hdr.ramdisk_size, hdr.page_size)) return 1;
    if (write_image("2nd boot", &page_ptr, second_fn,  hdr.second_size,  hdr.page_size)) return 1;

    free(data);
    return 0;
}
