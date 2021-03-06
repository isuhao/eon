/*
 * file:        RootDirectoryC.nc
 * description: Root directory implementation
 *
 * author:      Gaurav Mathur, UMass Computer Science Dept.
 * $Id: StackC.nc,v 1.0 2006/05/01 00:00:00 gmathur Exp $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * Root page implementation
 */

includes app_header;
includes common_header;

configuration RootDirectoryC {
    provides interface RootDirectory;

    uses {
        interface Console;
    }
}

implementation 
{
    components FalC, RootDirectoryM, LedsC, Crc8C;
#ifndef PLATFORM_TELOSB
    components SysTimeC;
#else
    components TimerC;
#endif

    RootDirectoryM.GenericFlash -> FalC.GenericFlash[unique("Flash")];
#ifndef PLATFORM_TELOSB
    RootDirectoryM.SysTime -> SysTimeC;
#else
    RootDirectoryM.LocalTime -> TimerC;
#endif
    RootDirectoryM.Crc8 -> Crc8C;
    RootDirectoryM.Leds -> LedsC;
    RootDirectory = RootDirectoryM.RootDirectory;
    RootDirectoryM.Console = Console;
}
