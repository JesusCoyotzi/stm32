# STM32 experiments and snippets

This repo is meant as an archive for different programs made for the STM32 microcontroller, specifically the STM32F103C8 commonly found in the Blue/Black pill boards.

These programs are mostly meant as reference for further use rather than full fledged applications or libraries. Hopefully of use to someone other than myself.

For further references on setting up the environment and running these programs refer to the notes directory of this repo.

## Dependencies
This code was built in a linux environment, i used Ubuntu 20.04. And the repo assumes a linux environment for edition and compiling but should be relatively easily to port to Windows.

Most boilerplate code was done using STM own IDE: STMCubeMx and you will likely require it for these programs to compile. Similarly you will need a arm tool chain and compiler, arm-linux-gnueabi-gcc is what i use. Refer to your linux distro packages to download a toolchain.

Finally you will need a debugger ST-links v2 are what i use for programming and debugging. Both original and knock-off debuggers should work.
For programming and debuggining I ended up using openocd due being more feture rich but you can also use [https://github.com/stlink-org/stlink](stlink) 
