/*
   Copyright 2010 Michael 'Mickey' Lauer
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
            timeoutWatch = GLib.Timeout.add_seconds( 3, onTimeout );
        }
        else
        {
            GLib.Source.remove( timeoutWatch ); timeoutWatch = 0;
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
public interface FsoUsage : Object {

public abstract void RegisterResource(string name, DBus.ObjectPath path) throws DBus.Error;
public abstract void UnregisterResource(string name) throws DBus.Error;
}

[DBus (name = "org.freesmartphone.Resource")]
public class FsoResource : Object {
	private GPS gps = new GPS();

        public void Enable() throws DBus.Error
	{ 
		stderr.printf("Enabling GPS\n");
		gps.start_stop_gps(true);
	}
	
	public void Disable()throws DBus.Error 
	{
		stderr.printf("Disabling GPS\n");
		gps.start_stop_gps(false);
	}
}





void main() {
    try {
        var conn = DBus.Bus.get(DBus.BusType.SYSTEM);


        dynamic DBus.Object bus = conn.get_object("org.freedesktop.DBus",
                                                  "/org/freedesktop/DBus",
                                                  "org.freedesktop.DBus");
        // try to register service in session bus
        uint reply = bus.request_name("org.freesmartphone.ousaged", (uint) 0);

        var fsoUsage = (FsoUsage) conn.get_object ("org.freesmartphone.ousaged" , "/org/freesmartphone/Usage");
        DBus.ObjectPath path = new DBus.ObjectPath("/org/freesmartphone/Resource/GPS");
   
        var fsoService = new FsoResource();

        conn.register_object("/org/freesmartphone/Resource/GPS",fsoService);
        fsoUsage.RegisterResource("GPS",path);

        // start main loop
        new MainLoop().run();
    } catch (DBus.Error e) {
        stderr.printf("Error: %s\n", e.message);
    }
}
