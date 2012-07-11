/*
 * Copyright (C) 2011-2012 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

using GLib;

[DBus (name = "org.freesmartphone.Device.Audio")]
interface IAudioManager : GLib.Object
{
    [DBus (name = "GetScenario")]
    public abstract string get_scenario () throws GLib.DBusError, GLib.IOError;
    [DBus (name = "SetScenario")]
    public abstract void set_scenario (string scenario) throws GLib.DBusError, GLib.IOError;
}

public void update_scenario()
{
    try
    {
        var manager = Bus.get_proxy_sync<IAudioManager>( BusType.SYSTEM, "org.freesmartphone.odeviced", "/org/freesmartphone/Device" );
        if ( manager == null )
            return;
        var current_scenario = manager.get_scenario();
        manager.set_scenario( current_scenario );
    }
    catch ( Error err )
    {
    }
}

// vim:ts=4:sw=4:expandtab
