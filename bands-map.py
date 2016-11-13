from __future__ import print_function
import json
import serial
from inclusion import *
import time

SPOT = 60
A_BAND = 120
CENTER = 512
B_BAND = 400

maps = [
  [(0,0), (0,B_BAND), (1024,B_BAND), (1024,0)],
  [(0,CENTER-(A_BAND/2)), (0,CENTER+(A_BAND/2)), (1024,CENTER+(A_BAND/2)), (1024,CENTER-(A_BAND/2))]
]

ser = serial.Serial('/dev/ttyACM0', 115200)
time.sleep(2)

def main():
  count = 0
  t0 = 0

  while True:
    s = ser.readline()
    try:
      payload = json.loads(s)

      # discard too fast joystick positions
      if payload.has_key('x') and time.time() - t0 < 0.1:
        continue
      t0 = time.time()

      if payload.has_key('x') and payload.has_key('y'):
        analyze_position(payload)
      else:
        # None
        print(payload)

    except ValueError:
      count += 1
      if count % 10 == 0:
        # None
        print('10 garbage received')


def analyze_position(payload):
  P0 = (payload['x'], payload['y'])

  zone = 2  # MOTOR OFF ZONE
  for i in range(len(maps)):
    if in_polygon(P0, maps[i]):
      zone = i
      break

  print('zone:', zone)

  if zone >= 2:
    ser.write('speed_A:1\n')
    ser.write('speed_B:1\n')
    ser.write('speed_C:1\n')
    ser.write('speed_D:1\n')

  elif zone == 0:
    speed_B = abs(P0[0] - CENTER)
    speed_C = abs(P0[1] - B_BAND/2)
    direction_B = P0[0] - CENTER >= 0
    direction_C = P0[1] - B_BAND/2 >= 0

    ser.write('speed_B:%(speed)s\n' % {'speed': int(round(speed_B/4))+1})
    ser.write('speed_C:%(speed)s\n' % {'speed': int(round(speed_C/4))+1})
    ser.write('direction_B:%(direction)s\n' % {'direction': {False: 'CCW', True: 'CW'}[direction_B]})
    ser.write('direction_C:%(direction)s\n' % {'direction': {False: 'CCW', True: 'CW'}[direction_C]})

  else:
    speed_A = abs(P0[0] - CENTER)
    direction_A = P0[0] - CENTER >= 0

    ser.write('speed_A:%(speed)s\n' % {'speed': int(round(speed_A / 4)) + 1})
    ser.write('direction_A:%(direction)s\n' % {'direction': {False: 'CCW', True: 'CW'}[direction_A]})

if __name__ == "__main__":
  main()
