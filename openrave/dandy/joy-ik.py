from __future__ import division
import redis, json, math, time
import pyglet
import serial

buffer = ""
ser = serial.Serial('/dev/ttyACM0', 57600)
time.sleep(2)

window = pyglet.window.Window()

class point:
  def __init__(self, x, y):
    self.x = float(x)
    self.y = float(y)

  def norm(self):
    return point(self.x/1024*2.0 - 1, self.y/1024*2.0 - 1)

  def denorm(self):
    return point((self.x + 1) * 1024 / 2.0, (self.y + 1) * 1024 / 2.0)


r_server = redis.Redis('localhost')
label = pyglet.text.Label('Mouse (x,y)', font_name='Times New Roman',
                          font_size=18, x=window.width // 2, y=window.height // 2,
                          anchor_x='center', anchor_y='center')
x = y = 0
z = 0.8
q = 0
t0 = 0

def denorm_dyna(q):
  return (q + math.pi/2) * 2048 / math.pi + 1023


@window.event
def on_draw():
    global x, y, z, buffer, t0
    # print ser.read(ser.inWaiting())

    buffer += ser.read(ser.inWaiting())

    window.clear()
    label.draw()

    q0 = json.loads(r_server.get('q'))['q0']
    q1 = json.loads(r_server.get('q'))['q1']
    q2 = json.loads(r_server.get('q'))['q2']

    p0 = point(-0.5,0.2).denorm()
    p1 = point(-0.5 + math.sin(q0), 0.2 + math.cos(q0)).denorm()
    pyglet.graphics.draw(2, pyglet.gl.GL_LINES, ('v2f', (p0.x, p0.y, p1.x, p1.y)))

    p0 = point(0, 0.2).denorm()
    p1 = point(0 + math.sin(q1), 0.2 + math.cos(q1)).denorm()
    pyglet.graphics.draw(2, pyglet.gl.GL_LINES, ('v2f', (p0.x, p0.y, p1.x, p1.y)))

    p0 = point(0.5, 0.2).denorm()
    p1 = point(0.5 + math.sin(q2), 0.2 + math.cos(q2)).denorm()
    pyglet.graphics.draw(2, pyglet.gl.GL_LINES, ('v2f', (p0.x, p0.y, p1.x, p1.y)))

    print 'angoli', int(denorm_dyna(q0)), int(denorm_dyna(q1)), int(denorm_dyna(q2))
    ser.write('angle_A:%(angle)s\n' % {'angle': int(denorm_dyna(q0))})
    ser.write('angle_B:%(angle)s\n' % {'angle': int(denorm_dyna(q1))})
    ser.write('angle_C:%(angle)s\n' % {'angle': int(denorm_dyna(q2))})

    s = cutPayload()
    print s
    if s is None:
      return

    try:
      payload = json.loads(s)

      # discard too fast joystick positions
      if payload.has_key('x'): # and time.time() - t0 > 0.1:
        t0 = time.time()
        print "payload", payload

        if payload.has_key('x') and payload.has_key('y'):
          x = payload['x'] / 1024 * 2.0 - 1
          y = payload['y'] / 1024 * 2.0 - 1
          r_server.set('xy', json.dumps({'x': x, 'y': y, 'z': z}))
          print {'x': x, 'y': y, 'z': z}
          label.text = '(x,y,z) = (%.3f, %.3f, %.3f)' % (x, y, z)
        else:
          print 'no X', payload

    except ValueError:
      print('10 garbage received')


def cutPayload():
    global buffer
    if buffer.count('\n') == 0:
      print "non cr"
      return
    i = buffer.index('\n')
    s = buffer[:i]
    buffer = buffer[i+1:]
    return s

@window.event
def on_text_motion(motion):
    global z
    if motion == pyglet.window.key.MOTION_UP:
        z += 0.01
    if motion == pyglet.window.key.MOTION_DOWN:
        z -= 0.01

    r_server.set('xy', json.dumps({'x': x, 'y': y, 'z': z}))
    label.text = '(x,y,z) = (%.3f, %.3f, %.3f)' % (x, y, z)

pyglet.app.run()
