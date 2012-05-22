var net = require('net');
var http = require('http');
var url = require('url');
var fs = require('fs');



//var fd = fs.openSync('./images/photo.jpg', 'w+');
function sendData(type, message) {
  connection.write(type+"/"+message+"\0");
  console.log(type+"/"+message);
}

function notifyPhone() {

  //C2DM Code

}

var FILE_OPEN = false;

var connection;

// TCP SERVER (ROBOLOCK INTERFACE
var tcp_server = net.createServer(function(c) { //'connection' listener
  console.log('[TCP-Server] Connected');

  connection = c;

  c.on('end', function() {
    console.log('[TCP-Server] Disconnected');
  });

  c.on('data', function(data) {

    var splitData = data.toString().split("/", 2);


    var header = splitData[0].split(":")
    var payload = splitData[1]

    // photo:0:500:1500, codes

    // example:
    // type:start_byte:end_byte:total_bytes/payload

    console.log("-------------");
    console.log(header);
    console.log(payload);
    console.log("-------------\n");

    switch(header[0] /* payload type (photo, codes) */) {

      case "photo":

      if (FILE_OPEN) {

        fs.writeSync(fd, payload, parseInt(header[1]), 'utf8');

        if (header[2] == header[3] /* if last byte written == total bytes */) {
          fs.closeSync(fd);
          notifyPhone();
          FILE_OPEN = false;
        }
      } else {

        fd = fs.openSync('./images/photo.jpg', 'w+', 0666);
        fs.writeSync(fd, payload, parseInt(header[1]), 'utf8');
        FILE_OPEN = true;

      }

      sendData("OK", "Received last chunk");

      break;

      case "codes":

      fd = fs.openSync('./text/codes.txt', 'w+', 0666);
      fs.writeSync(fd, payload, 0, 'utf8');
      fs.closeSync(fd)

      sendData("OK", "Received codes");

      break;

      default:
      sendData("ERROR", "Unknown Payload Type");
      break;

    }

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

  if (action == '/photo') {

   var img = fs.readFileSync('./images/photo.jpg');
   res.writeHead(200, {'Content-Type': 'image/jpg' });
   res.end(img, 'binary');
   console.log("Sent image ./images/photo.jpg")

 } else if (action == '/register') {

   registration_id = q.id;
   res.writeHead(200, {'Content-Type': 'text/plain' });
   res.end('Registration successful ' + registration_id + "\n");
   console.log("ID: " + registration_id);


  }
  // else if (action == '/codes') {

  //    // this needs to contact robolock, download a codes.txt file, and then serve it back to the phone

  //    var codes = fs.readFileSync('./text/codes.txt');
  //    res.writeHead(200, {'Content-Type': 'text/plain' });
  //    res.end(codes);

  // } 

  else if (action == '/unlock') {

    sendData("COMMAND/unlock");

    res.writeHead(200, {'Content-Type': 'text/plain' });
    res.end("Unlocking");

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


