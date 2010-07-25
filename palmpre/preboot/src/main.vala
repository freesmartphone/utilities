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
    
private bool have_x = false;
private bool debug = false;

public class BootConfiguration {
	public string title { get; set; default = ""; }
	public string description { get; set; default = ""; }
	public string kernel { get; set; default = ""; }
	public string cmdline_append {get; set; default = ""; }
    public string cmdline_remove { get; set; default = ""; }
	public string image_path { get; set; default = ""; }
	public string extra_args { get; set; default = ""; }
}

public class MainView {
	private Evas.Canvas _evas;
	private EcoreEvas.Window _window;
	private Edje.Object _mainmenu;
	private string _theme_path;
	private TimeoutSource timeoutSource;
	private bool first = true;
	private TimeoutSource shutdownSource;
	private int _selected;
    private string _engine;
    private int _width = 320;
    private int _height = 480;

	const string text_format = "list_text_%i";
	const string desc_format = "list_desc_%i";
	const string list_format = "list_bg_%i";

	const string kexec_load = "kexec";
	const string kexec_boot = "kexec --exec";

	public int selected {
		get { return _selected; }
		set {
			FsoFramework.theLogger.info(@"selected: $value / $(controller.num_items)");
			if (value != _selected) {
				if (value < 0)
					_selected = controller.num_items - 1;
				else if (value >= controller.num_items)
					_selected = 0;
				else
					_selected = value;
				_mainmenu.signal_emit("mouse,up,1", list_format.printf(selected));
			}
		}
	}

	public Evas.Canvas evas {
		get { return _evas; }
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
        create();
	}

	public void create() {
        
        if ( have_x )
		{
			_engine = "software_x11";
		}
        else
		{
			var fd = Posix.open( "/dev/fb0", Posix.O_RDWR );
			if ( fd == -1 )
			{
				FsoFramework.theLogger.warning( @"Can't open /dev/fb0: $(strerror(errno))" );
				Posix.exit( -1 );
			}
			
			_engine = "fb";
		}
        
		/* create a window */
		_window = new EcoreEvas.Window( _engine, 0, 0, _width, _height, null );
		window.title_set( "preboot" );
		window.show();
		_evas = window.evas_get();

		/* create our main menu edje object */
		_mainmenu = new Edje.Object( _evas );
		_mainmenu.file_set( _controller.themePath, "main" );
		_mainmenu.resize( 320, 480 );
		_mainmenu.layer_set( 0 );
		_mainmenu.show();
        
        /* connect callbacks */
        _mainmenu.signal_callback_add("mouse,up,1", "list_bg_*", onItemSelected);
		_mainmenu.signal_callback_add("mouse,up,1", "boot_rect", onItemSelected);
	}
    
    public void connectTimeouts() {
        /* connect timeout sources */
		if (controller.bootTimeout > 0) {
			FsoFramework.theLogger.info(@"Connect timeout handler with timeout: $(controller.bootTimeout)");
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
		if(condition == IOCondition.IN) {
			int used = keyUsed(source, controller.shutdown_key);
			if(used == 1 && shutdownSource == null) {
				shutdownSource = new TimeoutSource.seconds(3);
				shutdownSource.set_callback(shutdown);
				shutdownSource.attach(MainContext.default());
			} else if (used == 0) {
				shutdownSource.destroy();
				shutdownSource = null;
			}
		}
		return true;
	}

	private void onItemSelected( Edje.Object obj, string emission, string source) {
		FsoFramework.theLogger.info(@"$(source) clicked");
		if(!first) {
			if(timeoutSource != null) {
				FsoFramework.theLogger.info("removing source");
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
		first = false;
	}

    private string removePartFromCmdline(string cmdline, string part_name)
    {
        string result = "";
        string[] cmdline_parts = cmdline.split(" ");
        
        foreach (string part in cmdline_parts)
        {
            if (!part.has_prefix(part_name))
            {
                result += part;
            }
        }
        
        return result;
    }

    private string[] createLoadCmd(string kernel, string? cmdline_append, string? cmdline_remove, string? extra_args=null) {
		string[] params = new string[0];
		FsoFramework.theLogger.info(@"kexec: $(controller.kexec)");
		params += controller.kexec;
		params += "--load";
        
        /* load default cmdline and use it as base */
        var base_cmdline = "";
        if (FsoFramework.FileHandling.isPresent("/proc/cmdline"))
        {
            base_cmdline = FsoFramework.FileHandling.read("/proc/cmdline");
        }
        
        /* process cmdline */
        string[] parts = cmdline_remove.split(" ");
        FsoFramework.theLogger.debug(@"Have $(parts.length) parts to remove from cmdline");
        foreach (string part in parts)
        {
            FsoFramework.theLogger.debug(@"Removing $part from cmdline ...");
            base_cmdline = removePartFromCmdline(base_cmdline, part);
        }
        
		if(cmdline_append != null) {
            var composed_cmdline = @"$(base_cmdline) $(cmdline_append)";
			FsoFramework.theLogger.info(@"adding cmdline: '$(composed_cmdline)'");
			params += @"--command-line=\"$(composed_cmdline)\"";
		}
        
		if(extra_args != null)
			params += extra_args;
		params += kernel;
		return params;
	}

    private void loadKernel() {
		string out, err;
		int status;
		_mainmenu.part_text_set("info_text", "Loading kernel");
		var config = controller.configurations[selected];
		FsoFramework.theLogger.info(@"cmdline_append: $(config.cmdline_append) cmdline_remove: $(config.cmdline_remove) args: $(config.extra_args) kernel: $(config.kernel)");
		var cmd = createLoadCmd(config.kernel, config.cmdline_append, config.cmdline_remove, config.extra_args);

		try
		{
            var composed = "";
            foreach (var str in cmd)
            {
                composed += @"$str ";
            }
            FsoFramework.theLogger.debug(@"load kernel command: $composed");
            
            if (!debug)
            {
                return;
            }
            
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

	private void bootKernel() {
		string out, err;
		int status;
		_mainmenu.part_text_set("info_text", "Starting kernel");
		FsoFramework.theLogger.debug("Executing kernel");
		try
		{
            if (!debug)
            {
                return;
            }
            
            Process.spawn_sync(null, {"kexec", "--exec"}, null,0, null, out out, out err, out status);
            
			/* If we get here we should log it */
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

	private bool onBootTimeout() {
		FsoFramework.theLogger.info(@"timeout: $(controller.bootTimeout)");
		controller.bootTimeout --;
		_mainmenu.part_text_set("info_text", @"Booting in $(controller.bootTimeout) seconds");
		if(controller.bootTimeout == 0) {
			FsoFramework.theLogger.info("Timeout reached");
			loadKernel();
			bootKernel();
			return false;
		}
		return true;
	}

	private void up() {
		selected --;
	}

	private void down() {
		selected ++;
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
	private string _themePath = "";
	private string _configPath = "";
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
            FsoFramework.theLogger.info(@"reading config file from '$(_configPath)'");
            
			var sections = sf.sectionsWithPrefix("boot.");
			foreach(var section in sections) {
				BootConfiguration bootConfig = new BootConfiguration();
				bootConfig.title = sf.stringValue(section, "title", "<unknown>");
				bootConfig.description = sf.stringValue(section, "description", "");
				bootConfig.kernel = sf.stringValue(section, "kernel", "");
				bootConfig.cmdline_append = sf.stringValue(section, "cmdline_append", "");
                bootConfig.cmdline_remove = sf.stringValue(section, "cmdline_remove", "");
				bootConfig.image_path = sf.stringValue(section, "image_path", "");
				bootConfig.extra_args = sf.stringValue(section, "extra_args", "");
				configurations.add(bootConfig);
				num_items++;
			}

			bootTimeout = sf.intValue("general", "boot_timeout", -1);
			_mainView.selected = sf.intValue("general", "default", 0);
            
            debug = sf.boolValue("general", "debug_mode", false);
            if (debug)
            {
                FsoFramework.theLogger.info("DEBUG MODE ENABLED !!!");
            }

			kexec = sf.stringValue("general", "kexec", "/sbin/kexec");

			FsoFramework.theLogger.info("Setting up up key");
			var up = sf.stringListValue("general", "up_key", {"event0", Linux.Input.KEY_UP.to_string()});

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
		_mainView.create();
		loadConfiguration();
		_mainView.connectTimeouts();
	}

	public void shutdown() {
	}

}

public static int main( string[] args) {
    Ecore.init();
    Evas.init();
    Edje.init();
    EcoreEvas.init();

    have_x = Ecore.X.init() > 0;
    
	/* glib/ecore integration */
	GLib.MainLoop gmain = new GLib.MainLoop( null, false );
	if ( Ecore.MainLoop.glib_integrate() )
	{
		FsoFramework.theLogger.info( "GLib mainloop integration successfully completed" );
	}
	else
	{
		FsoFramework.theLogger.info( "Could not integrate glib mainloop. This library needs ecore compiled with glib mainloop support" );
		return -1;
	}

	var controller = new MainController();
	controller.init();
	
    Ecore.MainLoop.begin();
    
	controller.shutdown();
    
    Ecore.X.shutdown();
    EcoreEvas.shutdown();
    Edje.shutdown();
    Evas.shutdown();
    Ecore.shutdown();
    
	return 0;
}

} // namespace
