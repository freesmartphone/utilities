/* vim: set expandtab noai ts=4 sw=4: */
/*
 * tsmd -- Touchscreen management daemon
 *
 * (C) 2010 by Valéry Febvre <vfebvre@easter-eggs.com>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <tslib.h>

#define BUF_SIZE 256

/* device node uinput IOCTLs */
#define X_AXIS_MAX   319
#define X_AXIS_MIN   0
#define Y_AXIS_MAX   527
#define Y_AXIS_MIN   0
#define PRESSURE_MIN 0
#define PRESSURE_MAX 256

/* device node CY8MRLN touchscreen IOCTLs */
#define CY8MRLN_IOCTL_SET_SCANRATE       _IOW('c', 0x08, int)
#define CY8MRLN_IOCTL_SET_SLEEPMODE      _IOW('c', 0x09, int)
#define CY8MRLN_IOCTL_SET_VERBOSE_MODE   _IOW('c', 0x0e, int)
#define CY8MRLN_IOCTL_SET_TIMESTAMP_MODE _IOW('c', 0x17, int)
#define CY8MRLN_IOCTL_SET_WOT_THRESHOLD  _IOW('c', 0x1d, int)
#define CY8MRLN_IOCTL_SET_WOT_SCANRATE   _IOW('c', 0x22, int)

/* PSoC Power State */
enum
    {
        CY8MRLN_OFF_STATE = 0,
        CY8MRLN_SLEEP_STATE,
        CY8MRLN_ON_STATE
    };

/* WOT Scan Rate Index */
enum
    {
        WOT_SCANRATE_512HZ = 0,
        WOT_SCANRATE_256HZ,
        WOT_SCANRATE_171HZ,
        WOT_SCANRATE_128HZ
    };

/* daemon lock file */
#define LOCK_FILE "/var/lock/tsmd.lock"

/* from tslib-private.h */
struct tsdev
    {
        int fd;
        struct tslib_module_info *list;
        struct tslib_module_info *list_raw;     /* points to position in 'list' where raw reads
                                                   come from.  default is the position of the
                                                   ts_read_raw module. */
        unsigned int res_x;
        unsigned int res_y;
        int rotation;
    };

static char *devs[] = {
    "/dev/input/uinput",
    "/dev/uinput",
    NULL
};

#define die(str, args...) do { \
    perror(str);               \
    exit(EXIT_FAILURE);        \
  } while(0)
