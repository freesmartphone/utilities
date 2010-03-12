/**
 * (C) 2010 Simon Busch <morphis@gravedo.de>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **/

using Gee;

static Preboot preboot = null;

public class BootConfiguration 
{
    private string _title;
    private string _kernel_name;
    private string _kernel_cmdline;
    
    public BootConfiguration()
    {
    }
}

public class Preboot
{
    private Evas.Canvas _evas;
    private EcoreEvas.Window _window;
    private string _theme_path;
    private Edje.Object _mainmenu;
    
    public Evas.Canvas evas {
        get { return evas; }
    }
    
    public EcoreEvas.Window window {
        get { return _window; }
    }
    
    public string theme_path {
        get { return _theme_path; }
    }
    
    private void read_configuration()
    {
        FsoFramework.SmartKeyFile key_file = new FsoFramework.SmartKeyFile();
        /* FIXME read configuration */
    }
    
    public Preboot() {
        _theme_path = Config.PACKAGE_DATADIR + "/themes/default.edj";
        FsoFramework.theLogger.info(@"using '$(_theme_path)' as theme");

        /* create a window */
        _window = new EcoreEvas.Window( "software_x11", 0, 0, 320, 480, null );
        window.title_set( "preboot" );
        window.show();
        _evas = window.evas_get();
        
        /* create our main menu edje object */
        _mainmenu = new Edje.Object( _evas );
        _mainmenu.file_set( _theme_path, "mainmenu" );
        _mainmenu.resize( 320, 480 );
        _mainmenu.layer_set( 0 );
        _mainmenu.show();
    }
}

public static int main( string[] args)
{
	/* init */
	Ecore.init();
	EcoreEvas.init();
	Edje.init();

	preboot = new Preboot();

	message( "-> mainloop" );
	Ecore.MainLoop.begin();
	message( "<- mainloop" );

	/* shutdown */
	Edje.shutdown();
	EcoreEvas.shutdown();

	return 0;
}
