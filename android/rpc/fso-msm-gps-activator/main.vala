/*
   Copyright 2010-2011 Michael 'Mickey' Lauer
   Copyright 2010 Denis 'GNUtoo' Carikli

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

using GLib;

extern void gps_query_setup();
extern void gps_query_iteration();
//extern void gps_query_shutdown();

public class GPS : Object {
    uint timeoutWatch;

    public GPS(){
        gps_query_setup();
    }

    public bool is_gps_started()
    {
        return ( timeoutWatch > 0 );
    }

    public void start_stop_gps( bool on )
    {
        var running = timeoutWatch > 0;

        if ( running == on )
        {
            return;
        }

        if ( on )
        {
            timeoutWatch = Timeout.add_seconds( 3, onTimeout );
        }
        else
        {
            Source.remove( timeoutWatch ); timeoutWatch = 0;
            //gps_query_shutdown(); //makes the phone crash
        }
    }

    private bool onTimeout()
    {
        gps_query_iteration();
        return true; // call me again
    }

}

[DBus (name = "org.freesmartphone.Usage")]
public interface FsoUsageSync : Object
{
    public abstract void RegisterResource( string name, ObjectPath path ) throws GLib.Error;
    public abstract void UnregisterResource( string name ) throws GLib.Error;
}

[DBus (name = "org.freesmartphone.Resource")]
public class FsoResource : Object
{
	private GPS gps = new GPS();

    public void Enable() throws GLib.Error
	{ 
		stderr.printf( "Enabling GPS\n" );
		gps.start_stop_gps( true );
	}
	
	public void Disable() throws GLib.Error 
	{
		stderr.printf( "Disabling GPS\n" );
		gps.start_stop_gps( false );
	}
}

void main()
{
    try
    {
        var fsoUsage = Bus.get_proxy_sync<FsoUsageSync>( BusType.SYSTEM, "org.freesmartphone.ousaged", "/org/freesmartphone/Usage" );        
        var path = new ObjectPath( "/org/freesmartphone/Resource/GPS" );
   
        var fsoService = new FsoResource();
        var conn = Bus.get_sync( BusType.SYSTEM );
        conn.register_object( "/org/freesmartphone/Resource/GPS", fsoService );
        fsoUsage.RegisterResource( "GPS", path );

        // start main loop
        new MainLoop().run();
    }
    catch ( GLib.Error e )
    {
        stderr.printf( "Error: %s\n", e.message );
    }
}
