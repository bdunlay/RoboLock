var net = require('net');
var http = require('http');
var url = require('url');
var fs = require('fs');



//var fd = fs.openSync('./images/photo.jpg', 'w+');



function writeData(fd, buffer, length) {
  fs.writeSync(fd, buffer, 0, length, null);
}

// TCP SERVER (ROBOLOCK INTERFACE
var tcp_server = net.createServer(function(c) { //'connection' listener
  console.log('[Server] Connected');

  c.on('end', function() {
    console.log('[Server] Disconnected');
  });

  c.on('data', function(data) {
    console.log("[RoboLock] "+data.toString());
   });  

});

var registration_id;

// HTTP SERVER (MOBILE INTERFACE)
var http_server = http.createServer(function (req, res) {
  //res.writeHead(200, {'Content-Type': 'text/html'});
 // res.end("<h1>RoboLock!</h1>");
});

http_server.on('request', function(req, res) {

console.log(req);

  var request = url.parse(req.url, true);
  var action = request.pathname;
  var q = request.query;

  if (action == '/logo.jpg') {
     var img = fs.readFileSync('./images/logo.jpg');
     res.writeHead(200, {'Content-Type': 'image/jpg' });
     res.end(img, 'binary');
  } else if (action == '/register') {

    registration_id = q.id;

     res.writeHead(200, {'Content-Type': 'text/plain' });
     res.end('Registration successful ' + registration_id + "\n");
     console.log("ID: " + registration_id);
  } else if (action == '/hello') {

     res.writeHead(200, {'Content-Type': 'text/plain' });
     res.end('Registration successful ' + registration_id + "\n");
     console.log("Hello!");
  }

  else { 
     res.writeHead(200, {'Content-Type': 'text/plain' });
     res.end('Hello World \n');
  }

});


http_server.listen(8080);


tcp_server.listen(9090, function() { //'listening' listener
  console.log('[Server] Bound');
});
