# Generalities

This small document is written in order to document the design process for the
ARM STM32F103C8 on a bluepill board.  The idea is to be able to learn a little
of ARM while doing some embedded programming

## ToolChain 

Using gnu-noe-eabi ToolChain to build the project and CubeMx for
template generation.  Right now those two work out the box as long as I am able
to get the configuration right.  For debugging I use OpenOCD + gdb-multiarch
there is no printf in the embedded world especially if there is no OS so that
allows me to do some debug.

## Hardware 

STM32F103C8 clone: CK32f103C8 a cheap knock off from mercadolibre
that works like a charm.  Had to do some stuff to get it working.
- Added chip id to OpenOCD config files

ST-Link v2 a simple usb based debugger based around JTAG, again works way better
than expected.  I suspect my part is just a chinesse copy. It works regardless
Weird configuration to get everything running.
- Must select serial wire in CubeMx debug menu.

## Firmware 

Using the basics HAL libraries: propietary and vendor specific but
usefull for all the STM32 family. There is also the libopencm libraries wich
seem a bit more generic. They are meant to work on all ARMS.

## CubeMx

The vendor provided way to load firmware, HAL libraries, and create
boilerplate code very usefull if you do not want to spend 15 hrs reading the
user guide trying to figure out Wich register to set UART baud rate.  Importante
setup
- Under project manager in setup select Makefile to use gcc eabi
- On Pinout config, SYS, select Serial Wire as Debug to make st-link work 

## Debugging

In the case of gbd start by on two terminals:
The openocd server and the gdb
```bash gdb-multiarch build/program.elf #using bin file won't work
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg ```

Then on gdb attach to openocd server and restart, load program and continue ```
target remote :3333
monitor reset halt 
load continue

```

## Programming Little notes for every section i wanted to use

### GPIO 

There is a special register for the STM32 that allow atomic write to th
GPIO.  Seems useful when needing interruptions. But actually uses two registers,
one for setting and another for resetting.

### Timers 

Timers area cool feature and a way to control stuff with time. They
are usually a pain to use and set as they have more levers and buttons than a
plane.  One important note is that if the Time, in this case Timer 1, has a
break and update interrupt both have to be set in order for the insterrupts to
actually work.

As an important note HAL function based on time, used internally the
HAL_GetTick() to do timing and timeout. The output of this is in **miliseconds**

### ADC 

There are three configs DMA, poll and interrupt.  No config for pin for whatever
reason simply selecting channel when configuring the ADC seems to work.

### UART 

This thing is quite messy to say the list. HAL functions do not do what you would 
expect them to do. And there is a discussion about bugs on them so it may be
better to use raw acces to the objects if possible...
There is a whole discussion on the code comments for that one 
