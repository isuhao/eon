/*
 * file:        FalC.nc
 * description: This abstracts away the flash device
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

includes platform;

configuration FalC {
    provides {
    	interface GenericFlash[uint8_t id];
        interface StdControl;
    }
}

/* Using the separate configuration file lets us #ifdef the wiring for
 *the serial debug console.
 */
implementation {
#ifndef PLATFORM_MICA2_NOR
    components PageNANDM, NANDFlashM;

    GenericFlash = NANDFlashM.GenericFlash;
    NANDFlashM.PageNAND -> PageNANDM.PageNAND;
    StdControl = PageNANDM.StdControl;
#else
    components PageEEPROMC, FlashM;

    GenericFlash = FlashM.GenericFlash;
    FlashM.PageEEPROM -> PageEEPROMC.PageEEPROM[unique("PageEEPROM")];
    StdControl = PageEEPROMC.StdControl;
#endif
}
