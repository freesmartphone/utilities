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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <linux/uinput.h>
#include <signal.h>

#include "tsmd.h"

#define CONFIG_FILE "/etc/tsmd.conf"

unsigned int no_foreground = 1;
unsigned int network_port = 0;
unsigned int interrupt_read_and_send = 0;
unsigned int need_reopen_touchscreen = 0;
char network_addr[BUF_SIZE] = "\0";
char device_node[BUF_SIZE] = "\0";

#define MAX_X   319
#define MAX_Y   527

static int open_network_socket(char *address, int port)
{
    int socket_fd;

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        die("Failed to create network socket");
    }

    struct hostent *host = gethostbyname(address);

    if (!host)
    {
        die("Failed to get the hostent\n");
    }

    struct sockaddr_in addr = { 0, };
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *(struct in_addr *)host->h_addr;

    int result = connect(socket_fd, (struct sockaddr *)&addr, sizeof (addr));

    if (result < 0)
    {
        die("Connection failed");
    }

    return socket_fd;
}

int open_touchscreen_device(void)
{
    int fd = -1;

    if (device_node[0] != 0)
    {
        // open real touch screen device node
        fd = open(device_node, O_RDONLY);
        if (fd < 0)
        {
            die("Failed to open touchscreen device node");
        }
    }
    else if (network_addr[0] != 0)
    {
        // open network socket
        fd = open_network_socket(network_addr, network_port);
    }

    return fd;
}

void close_touchscreen_device(int fd)
{
    close(fd);
}

int open_uinput_device(void)
{
    struct uinput_user_dev dev;

    int fd = -1;

    int i = 0;

    while (devs[i] != NULL)
    {
        if ((fd = open(devs[i], O_WRONLY | O_NONBLOCK)) >= 0)
        {
            break;
        }
        i++;
    }

    if (fd < 0)
    {
        die("Failed to open uinput device");
    }
    else
    {
        memset(&dev, 0, sizeof (struct uinput_user_dev));
        strncpy(dev.name, "touchscreen", UINPUT_MAX_NAME_SIZE);
        dev.id.bustype = BUS_USB;
        dev.id.version = 4;
        dev.id.vendor = 0x1;
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

        if (write(fd, &dev, sizeof (dev)) < 0)
        {
            close(fd);
            die("Failed to write uinput device\n");
        }

        if (ioctl(fd, UI_DEV_CREATE) != 0)
        {
            close(fd);
            die("Set evbit: UI_DEV_CREATE\n");
        }

#ifdef DEBUG
        printf("uinput device successfully created\n");
#endif
    }

    return fd;
}

int send_uinput_event(int fd, __u16 type, __u16 code, __s32 value)
{
    struct input_event event;

    memset(&event, 0, sizeof (event));
    event.type = type;
    event.code = code;
    event.value = value;

    if (write(fd, &event, sizeof (event)) != sizeof (event))
    {
        fprintf(stderr, "%s: failed to write to uinput device\n", strerror(errno));
    }
}

static void read_and_send(int source_fd, int dest_fd)
{
    struct tsdev *ts;
    char *tsdevice = NULL;
    struct ts_sample samp;
    int ret;
    int in_movement = 0;

    ts = malloc(sizeof (struct tsdev));
    if (ts)
    {
        memset(ts, 0, sizeof (struct tsdev));
        ts->fd = source_fd;
    }
    else
    {
        die("ts_open");
    }
    if (ts_config(ts))
    {
        die("ts_config");
    }

    while (!interrupt_read_and_send)
    {
        ret = ts_read(ts, &samp, 1);

        if (ret < 0)
        {
            die("ts_read");
        }
        else if (ret == 0)
        {
            continue;
        }

        // ignore sample if it is out of screen (we ignore the gesture area here)
        if (samp.x > MAX_X || samp.y > MAX_Y)
        {
            continue;
        }

        send_uinput_event(dest_fd, EV_ABS, ABS_X, samp.x);
        send_uinput_event(dest_fd, EV_ABS, ABS_Y, samp.y);
        if (samp.pressure == 0 && in_movement)
        {
            // if we were in movement before, movement is now finished an we can send a
            // BTN_TOUCH up event
            send_uinput_event(dest_fd, EV_KEY, BTN_TOUCH, 0);
            send_uinput_event(dest_fd, EV_ABS, ABS_PRESSURE, 0);
            in_movement = 0;
#ifdef DEBUG
            printf("finger up\n");
#endif
        }
        else if (samp.pressure > 0)
        {
            if (!in_movement)
            {
                in_movement = 1;

                // if we have a pressure then report button touch down event
                send_uinput_event(dest_fd, EV_KEY, BTN_TOUCH, 1);
#ifdef DEBUG
                printf("finger down\n");
#endif
            }
            else
            {
                //Report pressure 
#ifdef DEBUG
                printf("%ld.%06ld: %6d %6d %6d\n", samp.tv.tv_sec, samp.tv.tv_usec, samp.x, samp.y,
                       samp.pressure);
#endif
                send_uinput_event(dest_fd, EV_ABS, ABS_PRESSURE, samp.pressure);
            }
        }
        send_uinput_event(dest_fd, EV_SYN, SYN_REPORT, 0);
    }
}

void handle_signals(int signum)
{
    switch (signum)
    {
        case SIGUSR1:
            interrupt_read_and_send = 1;
            signal(SIGUSR1, handle_signals);
            break;
        case SIGUSR2:
            interrupt_read_and_send = 0;
            need_reopen_touchscreen = 1;
            signal(SIGUSR2, handle_signals);
            break;
    }
}

int daemonize(void)
{
    int rc;

    /* process ID and Session ID */
    pid_t pid, sid;

    int i, lfp;

    char str[10];

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0)
    {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0)
    {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Create lock file */
    lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
    if (lfp < 0)
    {
        die("Can not open lock file");
    }
    if (lockf(lfp, F_TLOCK, 0) < 0)
    {
        die("Can not lock lock file, already locked");
    }
    sprintf(str, "%d\n", getpid());
    /* record pid to lock file */
    rc = write(lfp, str, strlen(str));

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void parse_config(char *config)
{
    FILE *fp = fopen(config, "r");
    char buf[1024];
    int len = 0;
    char *rc = NULL;

    buf[1023] = '\0';
    if (fp == NULL)
    {
#ifdef DEBUG
        perror("open config");
#endif
        return;
    }
    while (!feof(fp))
    {
        rc = fgets(buf, 1023, fp);
        len = strnlen(buf, 1023);
        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        if (!strncmp("device=", buf, 7))
        {
            strncpy(device_node, buf + 7, BUF_SIZE);
        }
        else if (!strncmp("addr=", buf, 5))
        {
            strncpy(network_addr, buf + 5, BUF_SIZE);
        }
        else if (!strncmp("port=", buf, 5))
        {
            network_port = atoi(buf + 5);
        }
        else if (!strncmp("daemonize=", buf, 10))
        {
            no_foreground = atoi(buf + 10);
        }
#ifdef DEBUG
        else
            printf("Cannot parse config: %s", buf);
#endif
    }
}

struct option opts[] = {
    {"foreground", no_argument, NULL, 'f'},
    {"node", required_argument, NULL, 'n'},
    {"addr", required_argument, NULL, 'a'},
    {"port", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}
};

int main(int argc, char *argv[])
{
    opterr = 0;
    int option_index;
    int chr;
    int showhelp = 0;
    int source_fd;
    int uinput_fd;

    /* setup our signal handlers */
    signal(SIGUSR1, handle_signals);
    signal(SIGUSR2, handle_signals);

    /* parse config before arguments. This makes it possible to override the config */
    parse_config(CONFIG_FILE);

    while (1)
    {
        option_index = 0;
        chr = getopt_long(argc, argv, "n:a:p:fh", opts, &option_index);

        if (chr == -1)
            break;

        switch (chr)
        {
            case 'h':
                showhelp = 1;
                break;
            case 'f':
                no_foreground = 0;
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
    printf("daemon = %i", no_foreground);
#endif

    if (showhelp || 
        (network_addr[0] == 0 && device_node[0] == 0) || 
        (network_addr[0] != 0 && network_port == 0))
    {
        printf("Usage: tsmd -n <device node> -a <network address> -p <network port> -f\n");
        exit(EXIT_FAILURE);
    }

    source_fd = open_touchscreen_device();
    uinput_fd = open_uinput_device();

    if (no_foreground)
    {
        daemonize();
    }
    
    /*
     * We have here two nested work loops as we need to interrupt the inner loop when we
     * get the SIGUSR1 signal and restart it when the SIGUSR2 signal arrives. This is
     * necessary as when the device turn of it's screen or goes into suspend then the
     * touchscreen supplies afterwards curious results. So we have to restart it then.
     */
    interrupt_read_and_send = 0;
    need_reopen_touchscreen = 0;
    while (1)
    {
        /* check if read/send is currently interrupted */
        if (!interrupt_read_and_send)
        {
            if (need_reopen_touchscreen)
            {
                close_touchscreen_device(source_fd);
                source_fd = open_touchscreen_device();
                need_reopen_touchscreen = 0;
            }

            read_and_send(source_fd, uinput_fd);
        }
        else
        {
            /* We are currently interrupt and should not read and send any events */
            /* NOTE: we use pause() here as we don't want to stay in a busy wait infinite
             * loop */
            pause();
        }
    }
}
