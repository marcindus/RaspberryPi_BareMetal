Frame Buffer sample
===================

*** temporary(2020/09/10) ***
Now tuning to new compiler and rpi4!
This code work on RPi1/3 only.

This is Frame Buffer sample code.  

Connect HDMI to 1080p/24bit color display.

If you can run this program,  
you'll see color pattern.  
(This pattern is Japanese broadcast standard ARIB STD-B28.)

-----

For Raspberry Pi Zero/1/2/3 Bare Metal code by AZO.  

How to make:  
1. install "arm-none-eabi-gcc".  
2. Open "Makefile " with texteditor, and change options.  
BOARD=RPZERO/RP1/RP2/RP3  
3. make  
4. If successfully, output "kernel.img"(RPZero/1) or "kernel7.img"(RP2/3).  

How to run:  
Put follow files to FAT SD card.  
* "bootcode.bin"  
* "start.elf"  
* "config.txt" &lt;- config_rp01.txt or config_rp23.txt  
* "kernel.img"(RPZero/1) or "kernel7.img"(RP2/3)  

Firmware:  
Grub "bootcode.bin" and "start.elf" at  
https://github.com/raspberrypi/firmware/tree/master/boot  

Reference
---------
- PeterLemon/RaspberryPi: Raspberry Pi Bare Metal Assembly Programming https://github.com/PeterLemon/RaspberryPi  

