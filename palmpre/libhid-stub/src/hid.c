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

typedef int hid_plugin_settings_t;
typedef int hid_handle_t;

int 
HidAllocPluginSettings (const char *config_file, hid_plugin_settings_t *settings, 
						int *num_plugins) 
{
	return 0;
}

void 
HidFreePluginSettings (hid_plugin_settings_t **settings, int num_plugins) 
{
}

int 
HidInitPluginTransport (const char *plugin_name, 
						hid_plugin_settings_t *settings, int num_plugins, 
						hid_handle_t **handle)
{
	return 0;
}

int 
HidHandleGetFd(hid_handle_t *handle)
{
	return 0;
}

int 
HidHandleReadInputEvent(hid_handle_t* handle, struct input_event *event, 
						int max_events)
{
	return 0;
}

void 
HidDestroyPluginTransport(hid_handle_t **handle)
{
}

