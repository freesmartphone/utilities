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

#define HID_CONFIG_FILE 			"/etc/hidd/HidPlugins.xml"
#define HID_DEVICE_TOUCHPANEL 		0
#define LOG_ERROR (message) fprintf(stderr, "%s: %s", __FUNCTION__, message)

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

int main(int argc, char *argv[])
{
	int rc;
	hid_plugin_settings_t *settings;
	hid_handle_t *tp;
	struct input_event ev;
	
	/* open touchpanel device */
	hidd_init(settings, HID_CONFIG_FILE);
	tp = hid_handle_open(settings, HID_DEVICE_TOUCHPANEL);
	
	while(1) {
		rc = hid_handle_event_read(tp, &ev, 1);
	}
	
	/* close all stuff */
	hid_handle_close(tp);
	hid_deinit(settings);
	
	return 0;
}
