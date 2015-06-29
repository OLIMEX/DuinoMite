##How to upload these files##

It depends on the specific file. There are two types of files - there is a file that contains only the the fimrware of the board (DMbasic) - it can be uploaded via the mini USB using the HID bootloader software.

To upload the other two of the files (either only a bootloader or bootloader + firmware) you need a PIC32-compatible programmer. If you lack such you can try to recover your DUINOMITE board with the DM basic files located in the HID-BOOTLODER directory.

##Which files are suitalbe for my board?##

If you have DUINOMITE or DUINOMITE-MINI or DUINOMITE-MEGA get DuinoMite-Mega-Mini_xxx.hex, where XXX shows wheter

If you have DUINOMITE EMEGA then get DuinoMite-eMega_xxx.hex

