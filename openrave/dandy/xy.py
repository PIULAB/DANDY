from __future__ import division
import redis, json
import pyglet

r_server = redis.Redis('localhost')
window = pyglet.window.Window()
label = pyglet.text.Label('Mouse (x,y)', font_name='Times New Roman',
                          font_size=18, x=window.width // 2, y=window.height // 2,
                          anchor_x='center', anchor_y='center')
x = y = z = 0

@window.event
def on_draw():
    window.clear()
    label.draw()

@window.event
def on_mouse_motion(xm, ym, dx, dy):
    global x, y, z
    x = xm/window.width*2.0-1
    y = ym/window.height*2.0-1
    # z = ((abs(x) - .5) * 2) ** 2
    # z = -0.3  # for dandy.robot
    r_server.set('xy', json.dumps({'x': x, 'y': y, 'z': z}))
    label.text = '(x,y,z) = (%.3f, %.3f, %.3f)' % (x, y, z)

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
