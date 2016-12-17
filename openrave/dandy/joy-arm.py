from __future__ import print_function
import json
import serial
import time, redis, math

ser = serial.Serial('/dev/ttyACM0', 57600)
time.sleep(2)
r_server = redis.Redis('localhost')

def denorm_dyna(q):
  return (q + math.pi/2) * 2048 / math.pi + 1023

def main():
  buffer = ""

  print('partito')
  t0 = 0
  z = 0.5

  while True:
    s = ser.readline()
    try:
      payload = json.loads(s)

      # discard too fast joystick positions
      # if payload.has_key('x') and time.time() - t0 < 0.1:
      #   continue
      # t0 = time.time()

      if payload.has_key('x') and payload.has_key('y'):
        print(payload)
        x = payload['x'] / 1024.0 * 2.0 - 1
        y = payload['y'] / 1024.0 * 2.0 - 1
        r_server.set('xy', json.dumps({'x': x, 'y': y, 'z': z}))
        # print('joystik norm.', {'x': x, 'y': y, 'z': z})
      else:
        print('altro', payload)
        continue

    except ValueError:
      print('10 garbage received')
      continue

    q0 = json.loads(r_server.get('q'))['q0']
    q1 = json.loads(r_server.get('q'))['q1']
    q2 = json.loads(r_server.get('q'))['q2']

    # print('angoli', int(denorm_dyna(q0)), int(denorm_dyna(q1)), int(denorm_dyna(q2)))
    # ser.write('angle_A:%(angle)s\n' % {'angle': int(denorm_dyna(q0))})
    # ser.write('angle_B:%(angle)s\n' % {'angle': int(denorm_dyna(q1))})
    # ser.write('angle_C:%(angle)s\n' % {'angle': int(denorm_dyna(q2))})

if __name__ == "__main__":
  main()



# print('inWaiting', ser.inWaiting())

# buffer += ser.read(ser.inWaiting())
# print('buffer', len(buffer))
# while True:
#   if buffer.count('\n') == 0:
#     print("no cr")
#     break
#   i = buffer.index('\n')
#   s = buffer[:i]
#   buffer = buffer[i + 1:]
