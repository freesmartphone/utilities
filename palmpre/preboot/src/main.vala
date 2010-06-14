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
	public string cmdline {get; set; }
	public string image_path { get; set; }
	public string extra_args { get; set; }
}

public class MainView {
	private Evas.Canvas _evas;
	private EcoreEvas.Window _window;
	private Edje.Object _mainmenu;
	private string _theme_path;
	private TimeoutSource timeoutSource;
        private TimeoutSource shutdownSource;
	private int selected;

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
		selected = controller.defaultSelection;
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
		_mainmenu.signal_emit("mouse,up,1", list_format.printf(controller.defaultSelection));
		_mainmenu.show();
	}

	public void connectCallbacks() {
		if (controller.bootTimeout > 0) {
			timeoutSource = new TimeoutSource.seconds(1);
			timeoutSource.set_callback(onBootTimeout);
			timeoutSource.attach(MainContext.default());
		}  
		_mainmenu.signal_callback_add("mouse,up,1", "list_bg_*", onItemSelected);
		_mainmenu.signal_callback_add("mouse,up,1", "boot_rect", onItemSelected);
	}
	
	public void bindBootConfiguration(Gee.AbstractList<BootConfiguration> bootConfigurations) {
		int i = 0;
		foreach(var config in bootConfigurations) {
			//we only have 2 entries atm
			if(i>=2)
				continue;
			_mainmenu.part_text_set(text_format.printf(i), config.title);
			_mainmenu.part_text_set(desc_format.printf(i), config.description);
			i ++;
		}
	}

	public bool onUp(IOChannel source, IOCondition condition) {
		if( condition == IOCondition.IN && keyUsed(source, controller.up_key) == 1) {
			up();
		}
		return true;
	}

	public bool onDown(IOChannel source, IOCondition condition) {
		if( condition == IOCondition.IN && keyUsed(source, controller.down_key) == 1) {
			down();
		}
		return true;
	}

	public bool onBoot(IOChannel source, IOCondition condition) {
		if( condition == IOCondition.IN && keyUsed(source, controller.boot_key) == 1) {
			boot();
		}
		return true;
	}

	public bool onShutdown(IOChannel source, IOCondition condition) {
		if( condition == IOCondition.IN) {
                        int used = keyUsed(source, controller.shutdown_key);
                        if(used == 1 && shutdownSource == null) {
			        shutdownSource = new TimeoutSource.seconds(3);
			        shutdownSource.set_callback(shutdown);
			        shutdownSource.attach(MainContext.default());
                        } else if (used == 0) {
                                shutdownSource = null;
                        }
		}
		return true;
	}

	private void onItemSelected( Edje.Object obj, string emission, string source) {
		FsoFramework.theLogger.info(@"$(source) clicked");
		if(timeoutSource != null) {
			FsoFramework.theLogger.debug("removing source");
			timeoutSource.destroy();
			timeoutSource = null;
			_mainmenu.part_text_set("info_text", "");
		}
		if(source.has_prefix("list_bg_")) {
			selected = source.split("_")[2].to_int();
		} else if(source == "boot_rect") {
			loadKernel();
			bootKernel();
		}
	}

	private void bootKernel() {
		string out, err;
		int status;
		_mainmenu.part_text_set("info_text", "Starting kernel");
		FsoFramework.theLogger.debug("Executing kernel");
		try
		{
			Process.spawn_sync(null, {"kexec", "--exec"}, null,0, null, out out, out err, out status);
			//If we get here we shoul log it
			FsoFramework.theLogger.debug(@"Kernel execution exited with: $status");
			FsoFramework.theLogger.debug(@"STDOUT: $(out)");
			Ecore.MainLoop.quit();
		}
		catch (GLib.SpawnError e)
		{
			FsoFramework.theLogger.error(@"boot kernel STDERR: $(err)");
			FsoFramework.theLogger.error(@"");
		}
	}

	private void loadKernel() {
		string out, err;
		int status;
		_mainmenu.part_text_set("info_text", "Loading kernel");
		var config = controller.configurations[selected];
		FsoFramework.theLogger.info(@"cmdline: $(config.cmdline) args: $(config.extra_args) kernel: $(config.kernel)");
		var cmd = createLoadCmd(config.kernel, config.cmdline, config.extra_args);
		foreach (var c in cmd)
			FsoFramework.theLogger.info(@"cmd $c");
		var params = string.joinv(" ", cmd);
		FsoFramework.theLogger.info(@"load cmd: kexec $(params)");
		FsoFramework.theLogger.info("load cmd: kexec %p".printf(params));
		try
		{
			Process.spawn_sync(null, cmd, null,0, null, out out, out err, out status);
			FsoFramework.theLogger.debug(@"Kernel loading exited with: $status");
			FsoFramework.theLogger.debug(@"STDOUT: $(out)");
		}
		catch (GLib.SpawnError e)
		{
			FsoFramework.theLogger.error(@"Kernel loading STDERR: $(err)");
			FsoFramework.theLogger.error(@"$(e.message)");
		}
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

	private string[] createLoadCmd(string kernel, string? cmdline, string? extra_args=null) {
		string[] params = new string[0];
		FsoFramework.theLogger.info(@"kexec: $(controller.kexec)");
		params += controller.kexec;
		params += "--load";
		if(cmdline != null) {
			FsoFramework.theLogger.info(@"adding cmdline: '$(cmdline)'");
			params += "--command-line=" + cmdline;
		}
		if(extra_args != null)
			params += extra_args;
		params += kernel;
		return params;
	}

	private void up() {
		selected --;
		if(selected == -1 )
			selected = controller.num_items -1;
		_mainmenu.signal_emit("mouse,up,1", list_format.printf(selected));
	}

	private void down() {
		selected = (selected + 1) % controller.num_items;
		_mainmenu.signal_emit("mouse,up,1", list_format.printf(selected));
	}

	private void boot() {
		loadKernel();
		bootKernel();
	}

        private bool shutdown() {
                string out, err;
                int status;
                FsoFramework.theLogger.info("Shutting down system");
                try {
                        _mainmenu.part_text_set("info_text", "Shutting down");
                        Process.spawn_sync(null, controller.shutdown_cmd, null, 0, null, out out, out err, out status);
                        controller.shutdown();
                }
                catch (GLib.SpawnError e) {
                        FsoFramework.theLogger.info(@"Shutting down: $(e.message)");
                }
                return false;
        }

	private int keyUsed(IOChannel source, uint16 key) {
		Linux.Input.Event event = {};
		var bytesread = Posix.read(source.unix_get_fd(), &event, sizeof(Linux.Input.Event));

		if(bytesread < sizeof(Linux.Input.Event))
			return -1;

		if(event.type == Linux.Input.EV_KEY && 
			event.code == key) {
			return event.value;
		}
		return -1;
	}
}

public class MainController {
	private MainView _mainView;
	private string _themePath;
	private string _configPath;
	private IOChannel up_channel;
	private IOChannel down_channel;
	private IOChannel boot_channel;
        private IOChannel shutdown_channel;
	const string input_base = "/dev/input";

	public int num_items = 0;
	
	public uint16 up_key {
		get; private set;
	}
	public uint16 down_key {
		get; private set;
	}
	public uint16 boot_key {
		get; private set;
	}

	public uint16 shutdown_key {
		get; private set;
	}

	public string kexec {
		get; private set;
	}

        public string[] shutdown_cmd {
                get; private set;
        }

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
		_configPath = "/boot/preboot.conf";

	}

	private IOChannel setupIOChannel( string input_name, IOFunc func) {
		IOChannel ioc = null;
		try
		{
			ioc = new IOChannel.file(Path.build_filename(input_base, input_name), "r");
			ioc.add_watch( IOCondition.IN, func);

		}
		catch (FileError e) {
			FsoFramework.theLogger.info(@"Cannot setup IOChannel on $(input_name): $(e.message)");
		}
		return ioc;
	}

	public void loadConfiguration() {
		FsoFramework.SmartKeyFile sf = new FsoFramework.SmartKeyFile();

		if (sf.loadFromFile(_configPath)) {
			var sections = sf.sectionsWithPrefix("boot.");
			foreach(var section in sections) {
				BootConfiguration bootConfig = new BootConfiguration();
				bootConfig.title = sf.stringValue(section, "title", "<unknown>");
				bootConfig.description = sf.stringValue(section, "description", "");
				bootConfig.kernel = sf.stringValue(section, "kernel", "");
				bootConfig.cmdline = sf.stringValue(section, "cmdline", "console=ttyS0");
				bootConfig.image_path = sf.stringValue(section, "image_path", "");
				bootConfig.extra_args = sf.stringValue(section, "extra_args", "");
				configurations.add(bootConfig);
				num_items++;
			}

			bootTimeout = sf.intValue("general", "boot_timeout", -1);
			defaultSelection = sf.intValue("general", "default", 0);

			kexec = sf.stringValue("general", "kexec", "/sbin/kexec");

			FsoFramework.theLogger.info("Setting up up key");
			var up = sf.stringListValue("general", "up_key", {"event0", Linux.Input.KEY_UP.to_string()});
			foreach( var u in up )
				FsoFramework.theLogger.info(@"up: $(u)");

			up_key = (uint16)up[1].to_int();
			up_channel = setupIOChannel(up[0],_mainView.onUp);

			FsoFramework.theLogger.info("Setting up down key");
			var down = sf.stringListValue("general", "down_key", {"event0", Linux.Input.KEY_DOWN.to_string()});
			down_key = (uint16)down[1].to_int();
			down_channel = setupIOChannel(down[0],_mainView.onDown);

			FsoFramework.theLogger.info("Setting up boot key");
			var boot = sf.stringListValue("general", "boot_key", {"event0", Linux.Input.KEY_ENTER.to_string()});
			boot_key = (uint16)boot[1].to_int();
			boot_channel = setupIOChannel(boot[0],_mainView.onBoot);

                        var shutdown = sf.stringListValue("general", "shutdown_key", {"event0", Linux.Input.KEY_ESC.to_string()});
                        shutdown_key = (uint16)shutdown[1].to_int();
                        shutdown_channel = setupIOChannel(shutdown[0], _mainView.onShutdown);

                        shutdown_cmd = sf.stringListValue("general", "shutdown", { "shutdown", "-h", "now"});
		}
		else
		{
			FsoFramework.theLogger.error(@"Cannot load config: $(_configPath)");
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
                Ecore.MainLoop.quit();
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
