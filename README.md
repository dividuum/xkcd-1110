xkcd-map.rent-a-geek.de code and info
=====================================

If you have not seen [the site](http://xkcd-map.rent-a-geek.de): It's a
zoomable version of the comic xkcd 1110 (http://xkcd.com/1110/).

Here is the code that runs on xkcd-map.rent-a-geek.de. It includes the
script necessary for converting the 2048x2048 sized xkcd tiles into the
format required for leaflet (the javascript library that renders the map).

Preparations
------------

Get the tiles from xkcd. While you may fetch them yourself, it's much
simpler to use on of the available collections. The easiest way is to 
clone [xkcd_grab](https://github.com/danielribeiro/xkcd_grab).

Put all png files into the tiles/ directory. The run convert.py (make sure
you have PIL (Python Imaging Library) installed.

This will generate 19000 256x256 png files in the directory converted.
These files can be used as a tile layer in leaflef.

The size of the comic requires tiles up to a zoomlevel of 10. This would
require 1.4 million tile files. Since most of the tiles are either white
(top half) of black (bottom half), I created a small flask based server
that returns black or white png images if the corresponding tile file is
not pregenerated.

Setup
-----

Install flask. The easiest way is to create a new virtualenv environment
and install flask in there:

    $ virtualenv env
    $ env/bin/activate
    $ easy_install flask

Then run export.py. Your version of the map is now available on
http://localhost:5000

Scaling
-------

I expected some traffic, so I used several tools I already knew (for
example in building [geolua](http://geolua.com) to make my live easier. 
My server is an Hetzner EQ4 (quadcore i7 with 8gb ram). I already had Nginx 
installed to dispatch to some of my other projects. So I added a new 
virtual host to forward incoming requests to my test openvz container.
I also disabled the access.log.

I installed Varnish in there. If you did now know about
[Varnish](https://www.varnish-cache.org/), be sure to check it out. It's a
very fast reverse proxy. So it sits between the user and your webserver and
handles caching. 

All of the tile images are fully cachable, so I forwarded incoming requests
to varnish. I used the default config of varnish, except forcing cache
lookups for every request. This makes sure varnish tries to cache requests
that include cookie headers (normally not a good idea):

    backend default {
        .host = "127.0.0.1";
        .port = "8000";
    }

    sub vcl_recv {
        return (lookup);
    }

I gave varnish 1G of ram (using `-s malloc,1G`). This was enough to hold
the working set in memory. So most the the requests where handled by
varnish without hitting the python server.

Since a single flask instance would probably not survive the cache warming
phase, I used [gunicorn](http://gunicorn.org/). It is a WSGI server written
in python.

    $ easy_install gunicorn
    $ gunicorn --max-requests=10000 -w 10 export:app

This will start gunicorn. It will fork 10 worker processes each handling
10000 requests before being restarted.


Numbers
-------

The site generated ~600k visits (I don't have exact numbers, since
access.logs where disabled and I only added analytics later).

I estimate the number of total requests received to be around 900 million.

At peak time the 100Mbit connection of my server was saturated. Here is the
output (stripped) of varnishstat (I restarted varnish several times to flush
the tile cache):

        57811856     12473.01     10284.98 Client connections accepted
        57790147     12472.01     10281.11 Client requests received
        57205492     12424.05     10177.10 Cache hits
          433054        34.97        77.04 Cache misses
          403239        47.96        71.74 Backend conn. success
          180453         0.00        32.10 Backend conn. failures
              64         0.00         0.01 Fetch head
          403101        47.96        71.71 Fetch with Length
          100000          .            .   N struct sess_mem
          749561          .            .   N struct sess
          254184          .            .   N struct object
          254255          .            .   N struct objectcore
          253958          .            .   N struct objecthead
               0          .            .   N struct vbe_conn
              71          .            .   N worker threads
             128         0.00         0.02 N worker threads created
            6799         0.00         1.21 N overflowed work requests
            1088         0.00         0.19 N dropped work requests
               1          .            .   N backends
               7          .            .   N expired objects
        25049293          .            .   N LRU moved objects
        57693417     12456.03     10263.91 Objects sent with write
        57809330     12472.01     10284.53 Total Sessions
        57790147     12472.01     10281.11 Total Requests
          151600        12.99        26.97 Total pass
          403165        47.96        71.72 Total fetch
     14264551135   3080996.78   2537724.81 Total header bytes
     35593478205   7790593.82   6332232.38 Total body bytes
        57809314     12472.01     10284.52 Session Closed
            1470         0.00         0.26 Session herd
      2322113128    499462.00    413113.88 SHM records
       231630759     49944.00     41208.11 SHM writes
        15408981      3100.27      2741.32 SHM MTX contention
             830         0.00         0.15 SHM cycles through buffer
          657292        82.93       116.94 SMA allocator requests
          508368          .            .   SMA outstanding allocations
       225152727          .            .   SMA outstanding bytes
       317287631          .            .   SMA bytes allocated
        92134904          .            .   SMA bytes free
          181489         0.00        32.29 SMS allocator requests
        75554336          .            .   SMS bytes allocated
        75862402          .            .   SMS bytes freed
          403228        47.96        71.74 Backend requests made
               1         0.00         0.00 N vcl total
               1         0.00         0.00 N vcl available
               1          .            .   N total active purges
               1         0.00         0.00 N new purges added
        56214387     12138.31     10000.78 HCB Lookups without lock
          431915        33.97        76.84 HCB Lookups with lock
          431912        33.97        76.84 HCB Inserts
            1438         0.00         0.26 Accept failures
               2         0.00         0.00 Connection dropped late
            5621         1.00         1.00 Client uptime

Total traffic so far is around 800GB. Since 10 TB is included for my
server, that's not a problem. 

It would have cost me around $900 had I used cloudfront :-)

I tried [coralcdn](http://www.coralcdn.org/) at peak time for 5% of all
visitors. It generated too many 503 responses so I disabled it again.
