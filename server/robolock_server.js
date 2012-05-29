var net = require('net');
var http = require('http');
var url = require('url');
var fs = require('fs');



//var fd = fs.openSync('./images/photo.jpg', 'w+');
function sendData(type, message) {
  connection.write(type+"/"+message+"\0");
  console.log(type+"/"+message);
}

// var cd2m_fd = fs.openSync("cd2m.txt", "r");

 

function notifyPhone() {
  var CD2M_command = fs.readFileSync('./cd2m.txt').toString();
  var sys = require('sys')
  var exec = require('child_process').exec;
  function puts(error, stdout, stderr) { sys.puts(stdout) }
  exec(CD2M_command, puts);
  //C2DM Code
  console.log("NOTIFYING PHONE!");

}

var FILE_OPEN = false;

var connection;
var startIndex = 0;

// TCP SERVER (ROBOLOCK INTERFACE
var tcp_server = net.createServer(function(c) { //'connection' listener
  console.log('[TCP-Server] Connected');

  connection = c;

  c.on('end', function() {
    console.log('[TCP-Server] Disconnected');
  });

  c.on('data', function(data) {

    var splitData = data.toString().split("/", 2);


    var header = splitData[0];
    if (splitData[1] != null)
      var payload = data.slice((header.length + 1));

    console.log(header);

    // photo:0:500:1500, codes

    // example:
    // type:start_byte:end_byte:total_bytes/payload

    console.log("-------------");
    console.log(header);
    console.log(payload);
    console.log("-------------\n");

    switch(header /* payload type (photo, codes) */) {

      case "photo":

        if (FILE_OPEN) {

          if (payload == "END") {
            fs.closeSync(fd);
            FILE_OPEN = false;
            startIndex = 0;

          console.log("CLOSING FILE");

          } else {
            startIndex += fs.writeSync(fd, payload, 0, payload.length, startIndex);
            console.log("WRITING FILE");
          //fs.writeSync(fd, buffer, offset, length, position)
          }

        } else {
          console.log("OPENING FILE");
          notifyPhone();
          fd = fs.openSync('./images/photo.jpg', 'w+', 0666);
          startIndex += fs.writeSync(fd, payload, 0, payload.length, startIndex);
          FILE_OPEN = true;

          console.log("WRITING FILE");

        }

        console.log(startIndex+ "\n")
      //  sendData("OK", "Received last chunk");


      break;

      case "codes":

      fd = fs.openSync('./text/codes.txt', 'w+', 0666);
      fs.writeSync(fd, payload, 0, 'utf8');
      fs.closeSync(fd)

      sendData("OK", "Received codes");

      break;

      default:
//      sendData("ERROR", "Unknown Payload Type");
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

    sendData("OK");

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


