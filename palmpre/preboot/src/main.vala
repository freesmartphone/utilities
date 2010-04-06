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

namespace preboot {
	
public class BootConfiguration {
	public string title { get; set; }
	public string description { get; set; }
	public string kernel { get; set; }
	public string root_fs { get; set; }
	public string image_path { get; set; }
	public string name { get; set; }
}

public class MainView {
	private Evas.Canvas _evas;
	private EcoreEvas.Window _window;
	private Edje.Object _mainmenu;
	private string _theme_path;

	public Evas.Canvas evas {
		get { return evas; }
	}
	
	public EcoreEvas.Window window {
		get { return _window; }
	}
    
	public string theme_path {
		get { return _theme_path; }
	}
    
	public MainController controller {
		get; set;
	}

	public MainView() {
		
		FsoFramework.theLogger.info(@"using '$(_theme_path)' as theme");
	}

	public void create() {
		 /* create a window */
		_window = new EcoreEvas.Window( "software_x11", 0, 0, 320, 480, null );
		window.title_set( "preboot" );
		window.show();
		_evas = window.evas_get();
		
		/* create our main menu edje object */
		_mainmenu = new Edje.Object( _evas );
		_mainmenu.file_set( _controller.themePath, "mainmenu" );
		_mainmenu.resize( 320, 480 );
		_mainmenu.layer_set( 0 );
		_mainmenu.show();
	}
	
	public void bindBootConfiguration(GLib.List<BootConfiguration> bootConigurations) {
	}
}

public class MainController {
	private MainView _mainView;
	private string _themePath;
	private string _configPath;
	private Gee.AbstractList<BootConfiguration> configuratons = new Gee.ArrayList<BootConfiguration>();
	
	public string themePath {
		get { return _themePath; }
	}
	
	public MainController() {
		_mainView = new MainView();
		_mainView.controller = this;
		
		_themePath = Config.PACKAGE_DATADIR + "/themes/default.edj";
		_configPath = "/etc/preboot.conf";
	}
	
	public void loadConfiguration() {
		FsoFramework.SmartKeyFile sf = new FsoFramework.SmartKeyFile();
		GLib.List<BootConfiguration> configurations = new GLib.List<BootConfiguration>();
		if (!sf.loadFromFile(_configPath)) {
			var sections = sf.sectionsWithPrefix("boot.");
			foreach(var section in sections) {
				BootConfiguration bootConfig = new BootConfiguration();
				bootConfig.name = sf.stringValue(section, "title", "<unknown>");
				bootConfig.description = sf.stringValue(section, "description", "");
				bootConfig.kernel = sf.stringValue(section, "kernel", "");
				bootConfig.root_fs = sf.stringValue(section, "root_fs", "");
				bootConfig.image_path = sf.stringValue(section, "image_path", "");
				configuratons.add(bootConfig);
			}
		}
		_mainView.bindBootConfiguration(configurations);
	}
	
	public void init() {
		Ecore.init();
		EcoreEvas.init();
		Edje.init();
		
		_mainView.create();
	}
	
	public void run() {
		message( "-> mainloop" );
		Ecore.MainLoop.begin();
		message( "<- mainloop" );
	}
	
	public void shutdown() {
		/* shutdown */
		Edje.shutdown();
		EcoreEvas.shutdown();
	}
}

public static int main( string[] args) {
	var controller = new MainController();
	controller.init();
	controller.run();
	controller.shutdown();
	return 0;
}

} // namespace
