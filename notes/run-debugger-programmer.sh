#On on terminal:
gdb-multiarch -f build/HalI2C.elf 
  target remote :3333
  load
  continue
  #remove all break
  clear
  #To reset and halt
  monitor halt reset

#On another: programmer
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg


