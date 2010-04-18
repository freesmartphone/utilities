/*
 * (c) 2010 by Simon Busch <morphis@gravedo.de>
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

/*
 * NOTES:
 * This version of the tslib is mainly based on code written by Palm
 * for there mobile operating system webOS. The code is published by Palm
 * on their webOS Open Source website at http://opensource.palm.com/
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include "hid.h"


#define TSLIB_MULTITOUCH

struct tsdev;

struct ts_sample {
#ifdef TSLIB_MULTITOUCH
	int id;
#endif
	int		x;
	int		y;
	unsigned int	pressure;
	struct timeval	tv;
};

#define DEBUG

#define HID_CONFIG_FILE			"/etc/hidd/HidPlugins.xml"
#define HID_DEVICE_TOUCHPANEL	0

#define DEBUGTRACE(format, ...) do {			\
		printf("[%s:%4d] ", __FILE__, __LINE__);	\
		printf(format, ## __VA_ARGS__);				\
		} while(0) 

static int num_plugins = 0;
static const char *plugin_names[] = {
	"HidTouchpanel",
};

int 
hid_get_num_plugins()
{
	return num_plugins;
}

int 
hid_init(hid_plugin_settings_t *settings, const char *config)
{
	int rc;
	if (settings == NULL) {
		rc = HidAllocPluginSettings(config, settings, &num_plugins);
		if (rc < 0) {
			settings = NULL;
			return -1;
		}
	}

#ifdef DEBUG
	fprintf(stdout, "HidAllocSettings: success\n");
#endif

	return 0;
}

void 
hid_deinit(hid_plugin_settings_t *settings)
{
    if (NULL != settings)
    {
        HidFreePluginSettings(&settings, num_plugins);
        settings = NULL;
    }
}

hid_handle_t*
hid_handle_open(hid_plugin_settings_t *settings, int device)
{
	int rc;
	hid_handle_t* handle = NULL;    
    
	if (settings == NULL)
		return NULL;
            
	rc = HidInitPluginTransport(plugin_names[device], settings, num_plugins, &handle);

	if (rc < 0) {
		HidFreePluginSettings(&settings, num_plugins);
		settings = NULL;
		return NULL;
	}

#ifdef DEBUG
	fprintf(stdout, "HidInitPluginTransport: success\n");
#endif

	return handle;
}  

void 
hid_handle_close(hid_handle_t *handle)
{
	if (NULL != handle) {
		HidDestroyPluginTransport(&handle);
	} 
}

int 
hid_handle_get_fd(hid_handle_t *handle)
{
	return HidHandleGetFd(handle);
}

int 
hid_handle_event_read(hid_handle_t *handle, struct input_event *events, int max_events)
{
	return HidHandleReadInputEvent(handle, (struct input_event*)events, max_events);
}

/*
 * tslib stuff 
 */
static hid_plugin_settings_t *settings;

struct tsdev* ts_open(const char *dev_name, int nonblock)
{
	if (hid_init(settings, HID_CONFIG_FILE) < 0) 
		return NULL;

	hid_handle_t *handle = hid_handle_open(settings, HID_DEVICE_TOUCHPANEL);
	if (handle) {
		return (struct tsdev*)handle;
	}

	return NULL;
}

int ts_config(struct tsdev *handle)
{
	return 1;
}

int ts_close(struct tsdev *handle)
{
	hid_handle_close((hid_handle_t*)handle);
	hid_deinit(settings);
	return 0;
}

int ts_fd(struct tsdev *handle)
{
	return hid_handle_get_fd((hid_handle_t*)handle);
}

#define MAX_MOUSE 					5
#define MAX_EVENTS 					32
#define UNINIT_PRESSURE 			(~0)
#define EV_FINGERID					0xf


#ifdef TSLIB_MULTITOUCH
#define SAMPLE_WHICH(cur_sample) (cur_sample->id)
#else
#define SAMPLE_WHICH(cur_sample) (0)
#endif

static struct ts_sample samples[MAX_EVENTS];
static int sample_index = 0;
static int pending_samples = 0;
struct ts_sample *cur_sample = NULL;
static int ignore = 0;
static int fingers[MAX_MOUSE];
static int pressure[MAX_MOUSE];

static void sample_end()
{
	if (cur_sample == NULL && 
		SAMPLE_WHICH(cur_sample) != -1 &&
		cur_sample->pressure != UNINIT_PRESSURE) {
		pending_samples++;
	}
	cur_sample = NULL;
}

static void sample_start()
{
	sample_end();
	cur_sample = &samples[pending_samples];
	cur_sample->pressure = UNINIT_PRESSURE;
	cur_sample->x = 0;
	cur_sample->y = 0;
}

int ts_read(struct tsdev *dev, struct ts_sample *sample, int max_sample)
{
	hid_handle_t *handle = (hid_handle_t*)dev;
	if (pending_samples == 0) {
		/* if we were in the middle of producing a sample, make sure we
		 * pick up where we left off!
		 */
		if (cur_sample != NULL) {
			memcpy(&samples[0], cur_sample, sizeof(struct ts_sample));
			cur_sample = &samples[0];
		}
		
		fd_set fds;
		struct timeval tv_zero = {0,0};
		
		FD_ZERO(&fds);
		FD_SET(ts_fd(dev), &fds);
		
		/* assume we are non-blocking and check for socket data */
		if (select(ts_fd(dev) + 1, &fds, NULL, NULL, &tv_zero) > 0) {
#ifdef DEBUG
			fprintf(stdout, "ts_read: got a sample\n");
#endif
			struct input_event hid_events[MAX_EVENTS];
			int max_event = sizeof(hid_events) / sizeof(hid_events[0]);
			int num_event = hid_handle_event_read(handle, hid_events, max_event);
			
			DEBUGTRACE("read %d events\n", num_event);
			
			int i;
			for (i=0; i<num_event; i++) {
				switch(hid_events[i].type) {
				case EV_KEY:
					if (!ignore) {
						DEBUGTRACE("got EV_KEY\n");
						switch(hid_events[i].code) {
						case BTN_TOUCH:
							DEBUGTRACE("BTN_TOUCH %d\n", hid_events[i].value);
							if (pressure[SAMPLE_WHICH(cur_sample)] == UNINIT_PRESSURE &&
								hid_events[i].value == 0) {
								/* this can occur when we max out on fingers */
								DEBUGTRACE("Ignoring release without previous press\n");
							} else {
								pressure[SAMPLE_WHICH(cur_sample)] = cur_sample->pressure = hid_events[i].value;
							}
							
							if (hid_events[i].value == 0) {
								/* finger is complete */
								fingers[SAMPLE_WHICH(cur_sample)] = 0;
							}
							break;
						default:
							DEBUGTRACE("ts_read: unknown EV_KEY code %d\n", hid_events[i].code);
							break;
						}
					}
					break;
				case EV_ABS:
					if (!ignore) {
						DEBUGTRACE("got EV_ABS %s %d\n",
								   hid_events[i].code == ABS_X ? "ABS_X" : (hid_events[i].type == ABS_Y ? "ABS_Y" : "ABS_PRESSURE"),
								   hid_events[i].value);
						switch (hid_events[i].value) {
						case ABS_X:
							cur_sample->x = hid_events[i].value;
							break;
						case ABS_Y:
							cur_sample->y = hid_events[i].value;
							break;
						case ABS_PRESSURE:
							break;
						default:
							DEBUGTRACE("ts_read: unknown EV_ABS code %d\n", hid_events[i].code);
							break;
						}
					}
					break;
				case EV_FINGERID:
					DEBUGTRACE("got EV_FINGERID %d %d\n", hid_events[i].value,
							   hid_events[i].code);
					int id = hid_events[i].value * 1000 + hid_events[i].code;
					
					/* assume we can handle this finger */
					ignore = 0;
					sample_start();
					
#ifndef TSLIB_MULTITOUCH
					/* ignore this finger if we're already tracking one */
					if (fingers[0] == 0) {
						fingers[0] = id;
					}
					else if (fingers[0] == id) {
						cur_sample->pressure = pressure[0];
					}
					else {
						DEBUGTRACE("ignoring multitouch finger\n");
						ignore = 1;
						break;
					}
#else
					cur_sample->id = -1;
					
					/* is this a finger we're already tracking or a new one? */
					int free = -1;
					int i;
					for (i=0; i<sizeof(fingers)/sizeof(fingers[0]); i++) {
						if (fingers[i] == id) {
							/* found it; retrieve the pressure state */
							cur_sample->id = i;
							cur_sample->pressure = pressure[i];
							break;
						}
						else if (free = -1 && fingers[i] == 0) {
							free = i;
						}
					}
					
					if (cur_sample->id == -1) {
						if (free == -1) {
							DEBUGTRACE("*** Couldn't find a free finger %d %d %d %d %d\n",
									   fingers[0], fingers[1], fingers[2], fingers[3], fingers[4]);
							ignore = 1;
							break;
						}
						fingers[free] = id;
						pressure[free] = cur_sample->pressure = UNINIT_PRESSURE;
						cur_sample->id = free;
					}
					DEBUGTRACE("using id %d\n", cur_sample->id);
#endif
					break;
				case EV_SYN:
					DEBUGTRACE("got EV_SYNC\n");
					sample_end();
					break;
				default:
					DEBUGTRACE("got type %d\n", hid_events[i].type);
					break;
				}
			}
		}
		
		sample_index = 0;
	}
	
	if (pending_samples) {
		*sample = samples[sample_index++];
#ifdef TSLIB_MULTITOUCH
		DEBUGTRACE("Returning sample id %d x %d y %d pressure %d\n",
				   sample->id, sample->x, sample->y, sample->pressure);
#else
		DEBUGTRACE("Returning sample x %d y %d pressure %d\n", 
				   sample->x, sample->y, sample->pressure);
#endif
		pending_samples--;
		return 1;
	}
	
	return 0;
}

int ts_read_raw(struct tsdev *dev, struct ts_sample *sample, int max_sample)
{
    sample = NULL;
    return 0;
}

int main (int argc, char *argv[]) {
	struct tsdev *dev;

	dev = ts_open(NULL, 0);
	if (dev == NULL) {
		fprintf(stderr, "could not open hidd\n");
		exit(1);
	}

	while (1) {
		struct ts_sample *sample = NULL;
		ts_read(dev, sample, 1);
	}

	ts_close(dev);
	
	return 0;
}
