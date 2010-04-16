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
        private TimeoutSource timeoutSource;

        const string text_format = "list_text_%i";
        const string desc_format = "list_desc_%i";
        const string list_format = "list_bg_%i";

        const string kexec_load = "kexec";
        const string kexec_boot = "kexec --exec";

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
		_mainmenu.file_set( _controller.themePath, "main" );
		_mainmenu.resize( 320, 480 );
		_mainmenu.layer_set( 0 );
		_mainmenu.show();
	}

        public void connectCallbacks() {
                _mainmenu.signal_callback_add("mouse,clicked,1", "list_bg_*", onItemSelected);
                _mainmenu.signal_callback_add("mouse,clicked,1", "boot_rect", onItemSelected);
                if (controller.bootTimeout > 0) {
                    debug(@"connecting timoout $(controller.bootTimeout)");
                    timeoutSource = new TimeoutSource.seconds(1);
                    timeoutSource.set_callback(onBootTimeout);
                    timeoutSource.attach(MainContext.default());
                }  
        }
	
	public void bindBootConfiguration(Gee.AbstractList<BootConfiguration> bootConfigurations) {
                int i = 0;
                foreach(var config in bootConfigurations) {
                        //we only have 2 entries atm
                        if(i>=2)
                             continue;
                        debug(@"handling $(config.name) %s: $(config.description)", text_format.printf(i));
                       _mainmenu.part_text_set(text_format.printf(i), config.name);
                       _mainmenu.part_text_set(desc_format.printf(i), config.description);
                       i ++;
                }
                _mainmenu.signal_emit(list_format.printf(controller.defaultSelection), "mouse,clicked,1");
	}

        private void onItemSelected( Edje.Object obj, string emission, string source) {
                debug(@"$(source) clicked");
                if(timeoutSource != null) {
                    debug("removing source");
                    timeoutSource.destroy();
                    timeoutSource = null;
                    _mainmenu.part_text_set("info_text", "");
                }
        }
        private void bootKernel() {
                _mainmenu.part_text_set("info_text", "Starting kernel");
        }

        private void loadKernel() {
                _mainmenu.part_text_set("info_text", "Loading kernel");
        }

        private bool onBootTimeout() {
                debug(@"timeout: $(controller.bootTimeout)");
                controller.bootTimeout --;
                _mainmenu.part_text_set("info_text", @"Booting in $(controller.bootTimeout) seconds");
                if(controller.bootTimeout == 0) {
                        debug("Timeout reached");
                        loadKernel();
                        bootKernel();
                        return false;
                }
                return true;
        }
}

public class MainController {
	private MainView _mainView;
	private string _themePath;
	private string _configPath;


	public Gee.AbstractList<BootConfiguration> configurations {
            default = new Gee.ArrayList<BootConfiguration>();
            get;
            set;
        }
	public string themePath {
		get { return _themePath; }
	}
        public int bootTimeout {
                get; set; 
        }
        public int defaultSelection {
                get; set;
        }
	
	public MainController() {
		_mainView = new MainView();
		_mainView.controller = this;
		
		_themePath = Config.PACKAGE_DATADIR + "/themes/default.edj";
		_configPath = "/etc/preboot.conf";
	}
	
	public void loadConfiguration() {
		FsoFramework.SmartKeyFile sf = new FsoFramework.SmartKeyFile();

		if (sf.loadFromFile(_configPath)) {
			var sections = sf.sectionsWithPrefix("boot.");
			foreach(var section in sections) {
                                debug(@"Section: $(section)");
				BootConfiguration bootConfig = new BootConfiguration();
				bootConfig.name = sf.stringValue(section, "title", "<unknown>");
				bootConfig.description = sf.stringValue(section, "description", "");
				bootConfig.kernel = sf.stringValue(section, "kernel", "");
				bootConfig.root_fs = sf.stringValue(section, "root_fs", "");
				bootConfig.image_path = sf.stringValue(section, "image_path", "");
				configurations.add(bootConfig);
			}

                        bootTimeout = sf.intValue("general", "boot_timeout", -1);
                        defaultSelection = sf.intValue("general", "default", 0);
		}
                else
                {
                     debug(@"Cannot load config: $(_configPath)");
                }
		_mainView.bindBootConfiguration(configurations);
	}
	
	public void init() {
		Ecore.init();
		EcoreEvas.init();
		Edje.init();

		_mainView.create();
                loadConfiguration();
                _mainView.connectCallbacks();
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
        /* glib/ecore integration */
        GLib.MainLoop gmain = new GLib.MainLoop( null, false );
        if ( Ecore.MainLoop.glib_integrate() )
        {
                debug( "GLib mainloop integration successfully completed" );
        }
        else
        {
                critical( "Could not integrate glib mainloop. This library needs ecore compiled with glib mainloop support" );
                return -1;
        }

	var controller = new MainController();
	controller.init();
	controller.run();
	controller.shutdown();
	return 0;
}

} // namespace
