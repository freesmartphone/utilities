/*
 * Copyright (C) 2012 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MEerrHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

using GLib;
using Alsa;

PcmDevice pcm;
GLib.MainLoop mainloop;

public static void sighandler( int signum )
{
    Posix.signal( signum, null ); // restore original sighandler
    pcm.close();
    mainloop.quit();
}

public int main(string[] args)
{
    string cardname = "default";
    int err;

    if ( ( err = PcmDevice.open( out pcm, cardname, PcmStream.PLAYBACK ) ) < 0 )
        error( @"Failed to open PCM device $cardname" );

    mainloop = new GLib.MainLoop( null, false );
    Posix.signal( Posix.SIGINT, sighandler );
    Posix.signal( Posix.SIGTERM, sighandler );
    Posix.signal( Posix.SIGBUS, sighandler );
    Posix.signal( Posix.SIGSEGV, sighandler );

    mainloop.run();

    return 0;
}
