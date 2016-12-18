import serial, time

ser = serial.Serial('/dev/ttyACM0', 57600)
time.sleep(2)
t0 = time.time()
angle = 2047

while True:
  s = ser.readline()
  if s != '{"mx64": "."}\r\n' and s != '\n':
    print repr(s)

  # if time.time() - t0 > 1:
  #   ser.write('angle_C:' + str(angle) + '\n')
  #   t0 = time.time()
  #
  #   angle += 100
  #   if angle > 3413:
  #     angle = 2047


# {"key": "angle_C", "value": "2910" }
# {"mx64": "."}
# {"key": "angle_A", "value": "2831" }

# {"key": "angle_C", "value": "782" }
