import json
import serial
from inclusion import *

SPOT = 60
CENTER = 512

maps = [
  [(0,0), (0,1024), (CENTER-SPOT,CENTER+SPOT), (CENTER-SPOT,CENTER-SPOT)],
  [(0,1024), (1024,1024), (CENTER+SPOT,CENTER+SPOT), (CENTER-SPOT,CENTER+SPOT)],
  [(1024,1024), (1024,0), (CENTER+SPOT,CENTER-SPOT), (CENTER+SPOT,CENTER+SPOT)],
  [(1024,0), (0,0), (CENTER-SPOT,CENTER-SPOT), (CENTER+SPOT,CENTER-SPOT)]
]
lines = {
  0: [(CENTER,CENTER), (0,CENTER)],
  1: [(CENTER,CENTER), (CENTER,1024)],
  2: [(CENTER,CENTER), (1024,CENTER)],
  3: [(CENTER,CENTER), (CENTER,0)]
}

ser = serial.Serial('/dev/ttyACM0', 115200)
count = 0

while True:
  s = ser.readline()
  print s
  try:
    payload = json.loads(s)
    P0 = (payload['x'], payload['y'])

    zone = 4
    for i in range(len(maps)):
      if in_polygon(P0, maps[i]):
        zone = i
        break

    if zone == 4:
      print 'SPOT'
    else:
      speed = point_to_line(P0, lines[zone][0], lines[zone][1])
      direction = is_left(P0, lines[zone][0], lines[zone][1]) < 0
      if zone >= 2:
        direction = not direction

      print ['A', 'B', 'C', 'D'][zone], speed, {True: 'forward', False: 'backward'}[direction]

  except ValueError:
    count += 1
    if count % 10 == 0:
      print '10 garbage received'
