var net = require('net');
var http = require('http');
var url = require('url');
var fs = require('fs');



//var fd = fs.openSync('./images/photo.jpg', 'w+');


function sendPhotoToPhone(fd) {

}

var FILE_OPEN = false;
var fd;

// TCP SERVER (ROBOLOCK INTERFACE
var tcp_server = net.createServer(function(c) { //'connection' listener
  console.log('[TCP-Server] Connected');

  c.on('end', function() {
    console.log('[TCP-Server] Disconnected');
  });

  c.on('data', function(data) {

    var splitData = data.toString().split("/", 2);

    var header = splitData[0].split(":")
    var payload = splitData[1]

    // photo:0:500:1500, codes

    console.log(header + "\n");
    console.log(payload + "\n");


    // switch(header[0] /* payload type (photo, codes) */) {

    //   case "photo":
    //     if (FILE_OPEN) {

    //       fs.write(fd, payload, header[1], 'utf8');
    //       c.write("OK")
          
    //       if (header[2] == header[3] /* if last byte written == total bytes */) {
    //         sendPhotoToPhone(fd);
    //         fd.close();
    //         FILE_OPEN = false;
    //       }
    //     } else {

    //       fd = fs.open('./images/photo.jpg', 'w+', 0666, function() {
    //         FILE_OPEN = true;
    //       });

    //     }


    //     //fs.write(fd, buffer, offset, length, position, [callback])
    //   break;

    //   case "codes":
    //     // read codes
    //     // send to phone
    //   break;



    // }


   // console.log("[RoboLock] "+data.toString());
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
     console.log("[HTTP-Server] Registered ID " + registration_id);
  }

  else { 
     res.writeHead(200, {'Content-Type': 'text/plain' });
     res.end('Hello World \n');
  }

});


http_server.listen(8080, function() {
    console.log('[HTTP-Server] Bound on port 8080');
});


tcp_server.listen(9090, function() { //'listening' listener
  console.log('[TCP-Server] Bound on port 8081');
});
