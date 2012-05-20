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

notify_phone()

function notify_phone() {






  var querystring = require('querystring');
  var http = require('http');

  var post_domain = 'https://android.apis.google.com';
  var post_port = 449;
  var post_path = '/c2dm/send';

  var post_data = querystring.stringify({
    'registration_id' : 'APA91bEl9okssZ0i0HUxJ-Kus556DK1hp8wUW7KCn11W8J3FvgBDEHrIX-Gqrd19GFfRkikM-ZfCX5Aqv4ZpeGZTs0uU7yc8aylrakJ44ZHELr1U952NgEsVbUM0_cF_a7Uqlo37yD9j_mDUTEg5PGrJgf8zNlk6Bg',
    'collapse_key': 'RoboLock' ,
    'data.message' : 'Hello!'
  });

  var post_options = {
    host: post_domain,
    port: post_port,
    path: post_path,
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Content-Length': post_data.length,
      'Authorization: GoogleLogin auth': 'DQAAAMUAAAAzUIFSh1NzVdjP-V0EMDkx6zeU9tk6Q4EJj22h7-vyIcT_2WVd1CNszxtXB4GzoA8e24Fz4eHZYKCMLnclvGUwu20cg-EvQNUwdLd4O38Xz7q2ZHVDoIO6ERWISwiouxz9TyOheqvd5RaDYDuzkjhfqRzuoMoZyMi3x8bfiNTVOK8pv4GyZx6AhiCusdqj1slzIm0QaSNuE0ilVnYpeLish_CYCpt7CAkdmuhbB60sKNFscYkBqoYN0Q8YXlB6QcYNIDR_-vAcFt53IAT0LIwg'
    }
  };

  var post_req = http.request(post_options, function(res) {
    res.setEncoding('utf8');
    res.on('data', function (chunk) {
      console.log('Response: ' + chunk);
    });
  });

  // write parameters to post body
  post_req.write(post_data);
  post_req.end();











}


http_server.listen(8080);


tcp_server.listen(9090, function() { //'listening' listener
  console.log('[Server] Bound');
});
