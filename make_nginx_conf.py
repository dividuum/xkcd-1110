import os
import subprocess

locations = []
for i in xrange(1, 11):
    pat = subprocess.Popen("./r2r 0 %d" % (2**(i-1)-1), shell=True, stdout=subprocess.PIPE).stdout.read().strip()
    locations.append("""
    location ~ ^/converted/((%d-[0-9]*-%s).png)$ {
        alias %s/$1;
        error_page 404 =200 /white.png;
        expires max;
    }
    """ % (i, pat, os.getcwd()))

print "".join(locations)
