/*
 * This file is part of mkdump
 *
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
 */

using GLib;
using FsoFramework;

const string PALMPRE_POWER_ON_NODE = "/sys/user_hw/pins/modem/power_on/level";
const string PALMPRE_BOOT_MODE_NODE = "/sys/user_hw/pins/modem/boot_mode/level";
const string PALMPRE_WAKEUP_MODEM_NODE = "/sys/user_hw/pins/modem/wakeup_modem/level";



public abstract class Modem
{
    public abstract void powerUp();
    public abstract void powerDown();
    public abstract void cycle();
    
    protected void writeToSysfsNode(string node, string val)
    {
        if (FsoFramework.FileHandling.isPresent(node))
        {
            FsoFramework.FileHandling.write(val, node);
        }
        else
        {
            stdout.printf(@"ERROR: sysfs node '$(node)' is not available");
        }
    }
}

public class QualcommPalmPreModem : Modem
{
    public override void powerUp()
    {
        writeToSysfsNode(PALMPRE_POWER_ON_NODE, "1");
    } 
    
    public override void powerDown()
    {
        writeToSysfsNode(PALMPRE_BOOT_MODE_NODE, "0");
        writeToSysfsNode(PALMPRE_POWER_ON_NODE, "0");
    }
    
    public override void cycle()
    {
        writeToSysfsNode(PALMPRE_BOOT_MODE_NODE, "0");
        writeToSysfsNode(PALMPRE_WAKEUP_MODEM_NODE, "0");
        writeToSysfsNode(PALMPRE_POWER_ON_NODE, "0");
        
        Posix.sleep(2);
        
        writeToSysfsNode(PALMPRE_POWER_ON_NODE, "1");
        writeToSysfsNode(PALMPRE_WAKEUP_MODEM_NODE, "1");
    }
}




public int main( string[] args )
{
    string machineType = FsoFramework.Utility.hardware();
    Modem modem = null;
    
    if ( args.length == 1 && args[1].has_prefix( "--h" ) )
    {
        stdout.printf( "Usage:\nmodemctrl [up|down|cycle] - Control the modem.\n" );
        return 0;
    }
    
    switch (machineType)
    {
    case "SirloinOMAP3430board":
        modem = new QualcommPalmPreModem();
        break;
    }
    
    if (modem == null)
    {
        stdout.printf(@"ERROR: modemctrl is currently not supported for this machine '$(machineType)'\n");
        return 0;
    }
    
    var command = args[1] ?? "";
    
    switch (command) 
    {
    case "up":
        FsoFramework.theLogger.info("Power up the modem ...");
        modem.powerUp();
        break;
    case "down":
        FsoFramework.theLogger.info("Power down the modem ...");
        modem.powerDown();
        break;
    case "cycle":
        FsoFramework.theLogger.info("Cyle the modem ...");
        modem.cycle();
        break;
    default: 
        break;
    }

    return 0;
}

