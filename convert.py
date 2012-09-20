import os
import re
import Image

DEFAULT_ZOOM = 7

worldmap = {DEFAULT_ZOOM: {}}

os.makedirs("converted")

for name in os.listdir("tiles"):
    m = re.match("^([0-9]+)(s|n)([0-9]+)(w|e).png$", name)
    if m is None:
        print "fail", name
        continue
    y, yy, x, xx = m.groups()
    x, y = int(x), int(y)

    if xx == 'w':
        x  = 64 - x
    else:
        x  = 63 + x

    if yy == 'n':
        y = 64 - y
    else:
        y = 63 + y

    print x, y

    worldmap[DEFAULT_ZOOM][x, y] = name

available = set()

def write_img(im, zoom, x, y):
    im.save('converted/%d-%d-%d.png' % (zoom, x, y))
    print 'save %d %d %d' % (zoom, x, y)
    available.add((zoom, x, y))

def load_img(zoom, x, y):
    if (zoom, x, y) not in available:
        return None
    print 'load %d %d %d' % (zoom, x, y)
    return Image.open('converted/%d-%d-%d.png' % (zoom, x, y))

for (x, y), name in worldmap[DEFAULT_ZOOM].iteritems():
    im = Image.open('tiles/%s' % name)
    for xx in xrange(8):
        for yy in xrange(8):
            part = im.crop((256 * xx, 256 * yy, 256 * (xx+1), 256 * (yy+1)))
            write_img(part, DEFAULT_ZOOM + 3, x * 8 + xx, y * 8 + yy)

for zoom in xrange(DEFAULT_ZOOM+3-1, -1, -1):
    print "zoom", zoom
    for x in xrange(2**zoom):
        for y in xrange(2**zoom):
            if y >= (2**(zoom-1)):
                color = (0,0,0)
            else:
                color = (255, 255, 255)
            im = Image.new("RGB", (512, 512), color)
            found = 0
            for xx in (0, 1):
                for yy in (0, 1):
                    tile = load_img(zoom+1, x*2+xx, y*2+yy)
                    if tile is None:
                        continue
                    im.paste(tile, (256 * xx, 256 * yy))
                    found += 1
            if found > 0:
                im.thumbnail((256, 256), Image.BILINEAR)
                write_img(im, zoom, x, y)
