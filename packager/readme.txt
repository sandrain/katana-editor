
KATANA Fx FloorBoard Copyright (C) 2007~2020 Colin Willcocks (gumtownbassman@yahoo.com).
 All rights reserved.
======================================================================================

KATANA Fx FloorBoard patch editor for the MK1 (original) Boss Katana Amp at FIRMWARE 4 - 
       version 20200503: 03 May 2020.
*****************************************************
YOU MAY FREELY USE THIS SOFTWARE FOR NON-COMERCIAL USE, BUT PLEASE FEEL FREE TO DONATE, 
 JUST KEEP IN MIND THE 10,000's of HOURS SPENT OVER 14 YEARS GETTING IT HERE,
  I WOULD LOVE A BEER !!!. :-)  YOUR CONTRIBUTION WILL HELP WITH FUTURE UPDATES.
  I do this im my own time at my own cost, no one is paying me..
***********************************************************************************
NoTE: If you paid for this software through a 3rd party reseller/retailer, then the reseller has
      illegally sold this software. This software is available for free download at the SourceForge
      sites only, and donations to the author are optional. 


 ********* LATEST FEATURES ADDED****************************************************
 *version
 *20200503 fixed *.tsl file compatibility with Tone Studio, added separate Font scaling setting.
 *
 *20191225 MK1 Firmware 4 feature update.
 *
 *20191119 Mk2 Katana file format patch loading, minor tweaks.
 *
 *20190820 Fixed *.tsl file saving format issues.
 *
 *20190525 Bug fixes.
 *
 *20190202 Bug fix, Main EQ effect not saving in *.tsl file.
 *
 *20190108 Firmware 3 update. new effects and EXP/Panel Knob Assigns, many feature enhancements and bug fixes.
 *
 *20180426 Font scaling dependant on screen DPI, smoothed upscale texturing, midi I/O tweaks for mac.
 *
 *20180304 Fixed patch selection Tree and patch file alignment, added Kat model selection.
 *
 *20180301 All Firmware Version 2 parameters added, GUI refinements, bug fixes.
 *
 *20171222 Version 2 Firmware support.
 *
 *20170803 Completed patch loader and assigns, auto save global assigns added, patch tree and patch write fixes.
 *
 *20170720 Begun work on patch loader and assigns.
 *
 *20170524 Fixed GT-8 file compatibility.
 *
 *20170516 GOT A KATANA !!! Thank You to all who donated, very much appreciated. 
 *         Fixed patch write, channel selection, patch loading, system loading.
 *         
 20170428 removed unused time based BPM effect parameters, minor adjustments to patch file format.
 *
 20170424 added system settings, panel knobs and leds, moved S/R loop to system, restructured system file, glowing Katana logo. 
 *
 20170421 another slight adjustment to sysx patch file format, added 3 option controls associated with Knob Panel LED effects.
          added auto save of last received patch file while in deBug mode.
 *
 20170420 new layout with redundant effects removed, re-aligned sysx file structure to accomodate Katana pnel l.e.d.s
          fixed patch selection and channel naming.
          Still has legacy GT... file conversion for opening/loading GT-100, GT-001, GT-10, GT-10B, GT-8, GT6B, GT-1, ME-80 patch files.
          Patch file writing to Katana should work, still uncertain of file structure, as I do not have a Katana to use, but you can donate
          from the Donate link in the editor help menu if you want to contribute a little toward a Katana, 
          so i can quickly finish this free editor.
 *
 20170414 adjusted patch tree layout and patch request. 
          Removed preamp speaker caninets as they are confirmed not usable.
          Installed system data collection from File menu 'Save System data'. send saved systen file to gumtownbassman@yahoo.com thanks.
 *
 20170412 changed the Patch Tree layout to match the Katana patch structure, and should populate with Katana channel patches.
          Renamed some effect units to match the Katana.
 *
 20170411 modified the system eclusive data format and tweaked the read in of patch data.
 *
 20170408 added sysx ID for connection and fine tuned midi  I/O 
 *
 20170407 initial upload with GT-100 editor ported to send Katana format parameter data, 
          experiment to see if the Katana will respond to extra parameters hidden within the system exclusive data framework 
          which seems to be based on a common model of the GT-100, GT-001, GT1.
 
********* FIRST USE CONNECTION HINTS ************** 
a.) Connect the KATANA via a USB cable, 
ensure the Boss usb driver software is installed..

b.) Start the FxFloorBoard program, for the Menu select Preferences/Midi and select the "Katana" USB midi device for midi input and output.

c.) Select the editor program "Connect" button, it should stay "on" if all connections are correct.

d.) Open the Patch-Tree side panel by clicking your mouse pointer on the 'yellow arrow', 
the Patch-Tree has items which will expand when you click on the [+] symbols.
 When the tree items expand, you will see the patch names appear as the editor requests this information from the GT100.

e.) A single mouse pointer click on the Patch-Tree name will change the Katana to that selected patch only 
- sound played through the KATANA will have the sound of that patch.
 
f.) A double click on the patch name will load the patch information from the Katana into the editor, 
all the patch settings will update the editor GUI.

g.) Any knob turning will change the sound of the effect being adjusted, 
when the desired sound is achieved, you can either save the patch to the Katana or save the patch to file on disk.

h.) To save the patch to the KATANA, select a location with a 'single-click' on the Patch-Tree,
then press the "write/sync" button. 


KATANA FxFloorBoard: 
NOTE: THIS IS AN UN-PAID PROJECT, INVESTING MANY 1000's HOURS OF MY TIME OVER 14 YEARS.
I am not employed by Roland/Boss, nor is Roland/Boss affiliated with the software.
 
I WILL NOT ACCEPT LIABILITY FOR ANY DATA LOSS, TO THE BEST OF MY KNOWLEDGE EVERYTHING SHOULD WORK CORRECTLY. 

This project is a continuing session of evolution.........

IF YOU FIND A BUG OR HAVE SOME ISSUES......
reply email to gumtownbassman@yahoo.com


This program is free software; you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free Software 
Foundation; either version 2 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.

This software is not for sale - if you paid for it, there seller has comitted 
an act of piracy and is subject to software piracy laws. For commercial use, 
a commercial licence must be obtained from the software author.

You should have received a copy of the GNU General Public License along with 
this program; if not, write to the Free Software Foundation, Inc., 675 Mass 
Ave, Cambridge, MA 02139, USA.


-------------------------------------------------------------------------------

To install:

BINARY:
------

WIN: Run KATANAFxFloorBoard.exe installer

OS/X: Open/extract ZIP package and copy to Applications folder or Desktop.

LINUX: Unpackage/decompress to Desktop.


SOURCE:
------

WIN:

OS/X:

LINUX:


-------------------------------------------------------------------------------

CONTACT:
------

Colin Willcocks <gumtownbassman@yahoo.com>


DISCLAIMER:
------

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


LEGAL:
------

KATANAFxFloorBoard Copyright (C)2007~2020 Colin Willcocks <gumtownbassman@yahoo.com>.
based on the GT8FxFloorBoard Copyright (C)2005~2007 Uco Mesdag.

Windows(TM) are Copyright (C)Microsoft Corporation. 
All rights reserved.

