import json
import serial
from inclusion import *
import time

SPOT = 60
DIAGONAL_SPOT = 100
CENTER = 512

maps = [
  [(0,0+DIAGONAL_SPOT), (0,1024-DIAGONAL_SPOT), (CENTER-SPOT,CENTER+SPOT), (CENTER-SPOT,CENTER-SPOT)],
  [(0+DIAGONAL_SPOT,1024), (1024-DIAGONAL_SPOT,1024), (CENTER+SPOT,CENTER+SPOT), (CENTER-SPOT,CENTER+SPOT)],
  [(1024,1024-DIAGONAL_SPOT), (1024,0+DIAGONAL_SPOT), (CENTER+SPOT,CENTER-SPOT), (CENTER+SPOT,CENTER+SPOT)],
  [(1024-DIAGONAL_SPOT,0), (0+DIAGONAL_SPOT,0), (CENTER-SPOT,CENTER-SPOT), (CENTER+SPOT,CENTER-SPOT)]
]
lines = {
  0: [(CENTER,CENTER), (0,CENTER)],
  1: [(CENTER,CENTER), (CENTER,1024)],
  2: [(CENTER,CENTER), (1024,CENTER)],
  3: [(CENTER,CENTER), (CENTER,0)]
}

ser = serial.Serial('/dev/ttyACM0', 115200)
time.sleep(2)
obj = {'minimo': 1000000000, 'massimo': 0}

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
        print payload

    except ValueError:
      count += 1
      if count % 10 == 0:
        # None
        print '10 garbage received'


def analyze_position(payload):
  P0 = (payload['x'], payload['y'])

  zone = 4
  for i in range(len(maps)):
    if in_polygon(P0, maps[i]):
      zone = i
      break

  if zone == 4:
    # None
    print P0, 'SPOT'
  else:
    speed = point_to_line(P0, lines[zone][0], lines[zone][1])
    direction = is_left(P0, lines[zone][0], lines[zone][1]) < 0
    if zone >= 2:
      direction = not direction

    obj['minimo'] = min(obj['minimo'], int(round(speed/8)))
    obj['massimo'] = max(obj['massimo'], int(round(speed/8)))
    # print obj

    ser.write('speed_A:%(speed)s\n' % {'speed': int(round(speed))+1})  # TODO better map from speed to Dynamixel speed
    ser.write('direction_A:%(direction)s\n' % {'direction': {False: 'CCW', True: 'CW'}[direction]})

    print P0, ['A', 'B', 'C', 'D'][zone], speed, {True: 'forward', False: 'backward'}[direction]

if __name__ == "__main__":
  main()
