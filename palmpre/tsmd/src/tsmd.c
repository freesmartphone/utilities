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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <linux/uinput.h>

#include "tsmd.h"

// #define DEBUG

static void init_cy8mrln(int fd)
{
  static int scanrate = 60;
  static int verbose = 0;
  static int wot_threshold = 22;
  static int sleepmode = CY8MRLN_ON_STATE;
  static int wot_scanrate = WOT_SCANRATE_512HZ;
  static int timestamp_mode = 1;


  ioctl(fd, CY8MRLN_IOCTL_SET_VERBOSE_MODE, &verbose);
  ioctl(fd, CY8MRLN_IOCTL_SET_SCANRATE, &scanrate);
  ioctl(fd, CY8MRLN_IOCTL_SET_TIMESTAMP_MODE, &timestamp_mode);
  ioctl(fd, CY8MRLN_IOCTL_SET_SLEEPMODE, &sleepmode);
  ioctl(fd, CY8MRLN_IOCTL_SET_WOT_SCANRATE, &wot_scanrate);
  ioctl(fd, CY8MRLN_IOCTL_SET_WOT_THRESHOLD, &wot_threshold);
}

static int open_network_socket(char *address, int port)
{
  int socket_fd;

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    die("Failed to create network socket");
  }

  struct hostent *host = gethostbyname(address);
  if (!host) {
    die("Failed to get the hostent\n");
  }

  struct sockaddr_in addr = {0, };
  addr.sin_family = PF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *(struct in_addr *)host->h_addr;

  int result = connect(socket_fd, (struct sockaddr *)&addr, sizeof (addr));
  if (result < 0) {
    die("Connection failed");
  }

  return socket_fd;
}

int open_uinput_device(void) {
  struct uinput_user_dev dev;
  int fd = -1;
  int i  =  0;

  while (devs[i] != NULL) {
    if ((fd = open(devs[i], O_WRONLY | O_NONBLOCK)) >= 0) {
      break;
    }
    i++;
  }

  if (fd < 0) {
    die("Failed to open uinput device");
  }
  else {
    memset(&dev, 0, sizeof(struct uinput_user_dev));
    strncpy(dev.name, "touchscreen", UINPUT_MAX_NAME_SIZE);
    dev.id.bustype = BUS_USB;
    dev.id.version = 4;
    dev.id.vendor  = 0x1;
    dev.id.product = 0x1;

    dev.absmin[ABS_X] = X_AXIS_MIN;
    dev.absmax[ABS_X] = X_AXIS_MAX;
    dev.absmin[ABS_Y] = Y_AXIS_MIN;
    dev.absmax[ABS_Y] = Y_AXIS_MAX;
    dev.absmin[ABS_PRESSURE] = PRESSURE_MIN;
    dev.absmax[ABS_PRESSURE] = PRESSURE_MAX;
     
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE);

    ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);

    if (write(fd, &dev, sizeof(dev)) < 0) {
      close(fd);
      die("Failed to write uinput device\n");
    }
 
    if (ioctl(fd, UI_DEV_CREATE) != 0) {
      close(fd);
      die("Set evbit: UI_DEV_CREATE\n");
    }

#ifdef DEBUG
    printf("uinput device successfully created\n");
#endif
  }

  return fd;
}

int send_uinput_event(int fd, __u16 type, __u16 code, __s32 value) {
  struct input_event event;

  memset(&event, 0, sizeof(event));
  event.type = type;
  event.code = code;
  event.value = value;

  if(write(fd, &event, sizeof(event)) != sizeof(event)) {
    fprintf(stderr,"%s: failed to write to uinput device\n", strerror(errno));
  }
} 

static void read_and_send(int source_fd, int dest_fd)
{
  struct tsdev *ts;
  char *tsdevice = NULL;

  struct ts_sample samp;
  int ret;
  int in_movement = 0;

  ts = malloc(sizeof(struct tsdev));
  if (ts) {
    memset(ts, 0, sizeof(struct tsdev));
    ts->fd = source_fd;
  }
  else {
    die("ts_open");
  }
  if (ts_config(ts)) {
    die("ts_config");
  }

  while (1) {
    ret = ts_read(ts, &samp, 1);

    if (ret < 0) {
      die("ts_read");
    }

    send_uinput_event(dest_fd, EV_ABS, ABS_X, samp.x);
    send_uinput_event(dest_fd, EV_ABS, ABS_Y, samp.y);
    if (samp.pressure == 0 && in_movement) {
        // if we were in movement before, movement is now finished an we can send a
        // BTN_TOUCH up event
        send_uinput_event(dest_fd, EV_KEY, BTN_TOUCH, 0);
        in_movement = 0;
#ifdef DEBUG
        printf("finger up\n");
#endif
    } else if (samp.pressure > 0) {
        if (!in_movement) {
            in_movement = 1;

            // if we have a pressure then report button touch down event
            send_uinput_event(dest_fd, EV_KEY, BTN_TOUCH, 1);
#ifdef DEBUG
            printf("finger down\n");
#endif
        } else {
            //Report pressure 
#ifdef DEBUG
            printf("%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y, samp.pressure);
#endif
            send_uinput_event(dest_fd, EV_ABS, ABS_PRESSURE, samp.pressure);
        }
    }
    send_uinput_event(dest_fd, EV_SYN, SYN_REPORT, 0);
  }
}

int daemonize(void) {
  /* process ID and Session ID */
  pid_t pid, sid;
  int i, lfp;
  char str[10];

  /* Fork off the parent process */
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then we can exit the parent process. */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Change the file mode mask */
  umask(0);

  /* Open any logs here */        

  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0) {
    /* Log the failure */
    exit(EXIT_FAILURE);
  }

  /* Change the current working directory */
  if ((chdir("/")) < 0) {
    /* Log the failure */
    exit(EXIT_FAILURE);
  }

  /* Create lock file */
  lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
  if (lfp < 0) {
    die("Can not open lock file");
  }
  if (lockf(lfp, F_TLOCK, 0) < 0) {
    die("Can not lock lock file, already locked");
  }
  sprintf(str, "%d\n", getpid());
  /* record pid to lock file */
  write(lfp, str, strlen(str));

  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int main(int argc, char *argv[])
{
  opterr = 0;
  int option_index;
  int chr;

  struct option opts[] = {
    { "foreground", no_argument, NULL, 'f' },
    { "node", required_argument, NULL, 'n' },
    { "addr", required_argument, NULL, 'a' },
    { "port", required_argument, NULL, 'p' },
    { "help", no_argument, NULL, 'h' },
    {NULL, no_argument, NULL, 0}
  };
  int showhelp = 0;
  int daemon = 1;

  int network_port = 0;
  char network_addr[BUF_SIZE] = "\0";
  char device_node[BUF_SIZE] = "\0";
  int source_fd;
  int uinput_fd;

  while (1) {
    option_index = 0;
    chr = getopt_long(argc, argv, "n:a:p:fh", opts, &option_index);

    if (chr == -1)
      break;

    switch(chr) {
    case 'h':
      showhelp = 1;
      break;
    case 'f':
      daemon = 0;
      break;
    case 'n':
      snprintf(device_node, 30, "%s", optarg);
      break;
    case 'a':
      snprintf(network_addr, 16, "%s", optarg);
      break;
    case 'p':
      network_port = atoi(optarg);
      break;
    default:
      break;
    }
  }
#ifdef DEBUG
  printf("device node = %s\n", device_node);
  printf("network port = %d\n", network_port);
  printf("network addr = %s\n", network_addr);
#endif

  if (showhelp || (network_addr[0] == 0 && device_node[0] == 0) || (network_addr[0] != 0 && network_port == 0)) {
    printf("Usage: tsmd -n <device node> -a <network address> -p <network port> -f\n");
    exit(EXIT_FAILURE);
  }

  if (device_node[0] != 0) {
    // open real touch screen device node
    source_fd = open(device_node, O_RDONLY);
    if (source_fd < 0) {
      die("Failed to open touchscreen device node");
    }
    init_cy8mrln(source_fd);
  }
  else if (network_addr[0] != 0) {
    // open network socket
    source_fd = open_network_socket(network_addr, network_port);
  }

  uinput_fd = open_uinput_device();

  if (daemon) {
    daemonize();
  }
  
  read_and_send(source_fd, uinput_fd);
}
