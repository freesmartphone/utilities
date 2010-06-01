/*
 * (C) 2010 by Simon Busch <morphis@gravedo.de>
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
 
#include <unistd.h>
#include <linux/reboot.h>
#include <stdio.h>
#include <stdlib.h>

#define LINUX_REBOOT_MAGIC1 		0xfee1dead
#define LINUX_REBOOT_MAGIC2 		0x28121969
#define LINUX_REBOOT_CMD_RESTART	0xa1b2c3d4
 
int main(int argc, char *argv[])
{
	char *arg = NULL;
	if (argc == 2) 
		arg = argv[1];
	reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, arg);
	return 0;
}
