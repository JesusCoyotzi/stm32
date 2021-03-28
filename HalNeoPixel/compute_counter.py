import argparse
import os

def parse_inputs():
  parser=argparse.ArgumentParser(description="Frequency and counter for Neopixels")
  parser.add_argument('pwm',type=float,help='PWM frequency')
  parser.add_argument('clk',type=float,help='CLK frequency')
  parser.add_argument('--prescaler',type=float,help='Prescaler', default=0)
  parser.add_argument('--on',type=float,help='On time required', default=0.9e-6)
  parser.add_argument('--off',type=float,help='Off time required', default=0.35e-6)
  args = parser.parse_args()
  return args

def compute_pwm(f_clk,prescaler,counter):
  f_timer=f_clk/float(prescaler+1)
  f_pwm=f_timer/float(counter+1)
  return f_pwm

def get_counter(f_clk,prescaler,f_pwm):
  f_escaled = f_clk/(prescaler+1)
  counter = f_escaled/f_pwm
  return counter - 1 

if __name__=="__main__":
  arg = parse_inputs()
  counter=get_counter(arg.clk,arg.prescaler,arg.pwm)
  f_pwm=compute_pwm(arg.clk,arg.prescaler,counter)
  t_per_tick = 1 / f_pwm  
  t_per_count = t_per_tick / (counter )
  
  print("PWM frequency {}".format(f_pwm))
  print("Required counter {}".format(counter))
  print("Time per tick {}".format(t_per_tick))
  print("Time per count {}".format(t_per_count))
  
  if arg.on or arg.off:
    count_on = (arg.on ) / t_per_count;
    count_off = ( arg.off ) / t_per_count;
    print("On count {}".format(round(count_on)))
    print("Off count {}".format(round(count_off)))
  
