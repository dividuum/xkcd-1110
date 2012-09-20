import os
from flask import Flask, Response
app = Flask(__name__)

BLACK = '\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01\x08\x00\x00\x00\x00:~\x9bU\x00\x00\x00\x01sRGB\x00\xae\xce\x1c\xe9\x00\x00\x00\tpHYs\x00\x00\x0b\x13\x00\x00\x0b\x13\x01\x00\x9a\x9c\x18\x00\x00\x00\x07tIME\x07\xdc\t\x14\x03\x15\x1e\xb4\xbf\xb1c\x00\x00\x00\x19tEXtComment\x00Created with GIMPW\x81\x0e\x17\x00\x00\x00\nIDAT\x08\xd7c`\x00\x00\x00\x02\x00\x01\xe2!\xbc3\x00\x00\x00\x00IEND\xaeB`\x82'
WHITE = '\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x01\x00\x00\x00\x01\x08\x00\x00\x00\x00:~\x9bU\x00\x00\x00\x01sRGB\x00\xae\xce\x1c\xe9\x00\x00\x00\tpHYs\x00\x00\x0b\x13\x00\x00\x0b\x13\x01\x00\x9a\x9c\x18\x00\x00\x00\x07tIME\x07\xdc\t\x13\x0c\x12\x1emuX \x00\x00\x00\x19tEXtComment\x00Created with GIMPW\x81\x0e\x17\x00\x00\x00\nIDAT\x08\xd7c\xf8\x0f\x00\x01\x01\x01\x00\x1b\xb6\xeeV\x00\x00\x00\x00IEND\xaeB`\x82'

@app.route("/converted/<int:zoom>-<int:x>-<int:y>.png")
def tile(zoom, x, y):
    name = 'converted/%d-%d-%d.png' % (zoom, x, y)
    if os.path.exists(name):
        f = file(name, "rb")
        body = f.read()
        f.close()
    else:
        if y >= 2 ** (zoom-1):
            body = BLACK
        else:
            body = WHITE
    return Response(body, mimetype='image/png', headers={
        # 'Etag': '"%d:%d:%d"' % (zoom, x, y),
        'Expires': 'Thu, 31 Dec 2037 23:55:55 GMT'
    })

@app.route("/leaflet-hash.js")
def plugin_hash():
    f = file("leaflet-hash.js", "rb")
    body = f.read()
    f.close()
    return Response(body, mimetype='application/x-javascript', headers={
        'Expires': 'Thu, 31 Dec 2037 23:55:55 GMT'
    })

@app.route("/intro.png")
def intro():
    f = file("intro.png", "rb")
    body = f.read()
    f.close()
    return Response(body, mimetype='image/png', headers={
        'Expires': 'Thu, 31 Dec 2037 23:55:55 GMT'
    })

@app.route("/favicon.ico")
def favicon():
    f = file("favicon.ico", "rb")
    body = f.read()
    f.close()
    return Response(body, mimetype='image/png', headers={
        'Expires': 'Thu, 31 Dec 2037 23:55:55 GMT'
    })

@app.route("/")
def index():
    return Response("""
<html>
  <head>
    <link rel="stylesheet" href="http://cdn.leafletjs.com/leaflet-0.4.4/leaflet.css" />
    <script src="http://cdn.leafletjs.com/leaflet-0.4.4/leaflet.js"></script>
    <script src="/leaflet-hash.js"></script>
    <style type="text/css">
        .twitter-share-button {
            position:absolute;bottom:30px;right:0;z-index:10000px;
        }
    </style>
  </head>
  <body style='background-color:#FFF;padding:0;margin:0;'>
    <div id='map' style='height:100%;'></div>
    <img id='intro' style='position: absolute;left:50%; margin-left: -268px;z-index:100000;top:20px;width:536px;height215px;:' src='/intro.png'>
    <a href="https://twitter.com/share" class="twitter-share-button" data-url="http://xkcd-map.rent-a-geek.de" data-via="dividuum" data-hashtags="xkcd">Tweet</a>
    <script>
      var map = L.map('map').setView([1.1,0.2], 10);

      if (window.location.hash.length > 0) {
          document.getElementById('intro').style.display = 'none';
      } else {
          map.on('mousedown', function() {
              document.getElementById('intro').style.display = 'none';
          });
      }

      var hash = new L.Hash();
      hash.init(map);

      var layer_url = '/converted/{z}-{x}-{y}.png';

      L.tileLayer(layer_url, {
        maxZoom: 10,
        noWrap: true,
        attribution: '<span style="font-size: 15px;">A zoomable visualization of <a href="http://xkcd.com/1110/">xkcd - Click and Drag</a>. This visualization was created by <a href="https://twitter.com/dividuum">@dividuum</a></span>',
      }).addTo(map);

      !function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");
    </script>
  </body>
</html>
    """, mimetype='text/html')
    
if __name__ == "__main__":
    # app.run(debug=True)
    app.run()
