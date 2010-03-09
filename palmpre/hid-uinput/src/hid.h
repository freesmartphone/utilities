
#ifndef _HID_H_
#define _HID_H_

typedef int hid_plugin_settings_t;
typedef int hid_handle_t;

int HidAllocPluginSettings
	(const char *config_file, hid_plugin_settings_t *settings, int *num_plugins);
void HidFreePluginSettings
	(hid_plugin_settings_t **settings, int num_plugins);
int HidInitPluginTransport
	(const char *plugin_name, hid_plugin_settings_t *settings, int num_plugins, hid_handle_t **handle);
int HidHandleGetFd
	(hid_handle_t *handle);
int HidHandleReadInputEvent
	(hid_handle_t* handle, struct input_event *event, int max_events);

#endif
	
