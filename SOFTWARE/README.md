#Here you can find all Duinomite related files.#

##Which prebuilt file should I use to restore the default software?##

The prebuilt files are compatible with either DUINOMITE, DUINOMITE MINI, DUINOMITE MEGA or DUINOMITE EMEGA. The files for the EMEGA boards are different compared to the rest of the DUINOMITE boards due to hardware differnces. Make sure to read the full name of the pre-built file to determine if it is the right one for your board.

There are three files for either EMEGA or the rest of the boards - one contains only the bootloader (without DM basic) - you need a programmer to upload this one; the second file contains only the DM basic - this file can be uploaded via the built-in bootloader using the HID application; the third file contains both the bootloader and DM basic - this file again can be uploaded only via a programmer. Make sure to  read the full name of the pre-built file to determine if it is the right one for your board. 

In most cases you can recover the software of a Duinomite board via built-in bootloader using the file containing the suitable DM basic. Alternatively, you can program a Duinomite via a PIC32-compatible programmer using via MPLAB or MPLABX.

##How to program DM basic project without PIC programmer tool?##

Start the board in bootloader mode: plug an USB cable and reset the board while holding "BUT" button. Doing so the yellow and green LEDs will blink alternatively (this means the bootloader mode is activated). 

Then start the executable HIDBootLoader.exe located in HID-BOOTLOADER folder. Select "Import Firmware Image" (the button with a folder icon or just press ctrl + o) -> select your image. The latest version of the project hex is placed in HID-BOOTLDER folder for both EMEGA and the rest of the DUINOMITE boards.

##How to program DM basic project with PIC programmer tool?##

If the bootloader program is damaged the only way to revert to the default software would be to use a programmer tool. You can program the board with bootloader or botolodaer and firmware hex file at once using PIC-KIT3 (or similar PIC32-compatoble programmer) and MPLAB/MPLABX by importing the desired hex from folder FIRMWARE. By default the board is programmed with bootlaoder and firmware. By default you can use the board without a programmer.

##Where are the DM basic sources?##

These are located in folder DMBasic.