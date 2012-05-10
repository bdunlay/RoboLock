/*
 * defines 
 */

// MASKS (32 bit, even though header is only 24 bits)
var MASK_TYPE         = 0xC0000000;
var MASK_SUBTYPE      = 0x3C000000;
var MASK_PAYLOAD_SIZE = 0x03FFFC00;
var MASK_PAYLOAD_TYPE = 0x00000300;

// TYPES
var TYPE_COMMAND      = 0x0;
var TYPE_NOTIFICATION = 0x1;

// SUBTYPES
var SUBTYPE_INIT        = 0;
var SUBTYPE_OPEN_DOOR   = 1;
var SUBTYPE_LIST_USERS  = 2;
var SUBTYPE_NEW_USER    = 3;
var SUBTYPE_DELETE_USER = 4;
var SUBTYPE_DISABLE     = 5;
var SUBTYPE_TEXT_MSG    = 6;
var SUBTYPE_TAKE_IMG    = 7;

var SUBTYPE_ACCESS_SUCCESS  = 0;
var SUBTYPE_ACCESS_FAILURE  = 1;
var SUBTYPE_ACCESS_REQUEST  = 2;

// HTTP PATHS
var OPEN_DOOR = "/open";
var LIST_USERS = "/list";
var NEW_USER = "/new";
var DELETE_USER = "/delete";
var DISABLE = "/disable";
var TEXT_MESSAGE = "/text"
var TAKE_IMAGE = "/capture";



/*
 * functions
 */


// this pulls out header flags
function parseHeader(x, header) {

      var h = x.readUInt32BE(0);

      header.type = (h&MASK_TYPE>>>30);
      header.subtype = (h&MASK_SUBTYPE)>>>26;
      header.payload_size = (h&MASK_PAYLOAD_SIZE)>>>10;
      header.payload_type = (h&MASK_PAYLOAD_TYPE>>>8);

      return header;
}

// this processes a packet based on its header flags
function process_packet(data) {
  var h = data.slice(0,4);
  var header = {};

  header = parseHeader(h, header);

  console.log(header);

  switch(header.type) {
    // relay notification to mobile device
    case TYPE_NOTIFICATION:
      switch(header.subtype) {
        
        case SUBTYPE_ACCESS_SUCCESS:
        console.log("[notification] - Access success.");
        break;
        
        case SUBTYPE_ACCESS_FAILURE:
        console.log("[notification] - Access failure."); 
        break;
        
        case SUBTYPE_ACCESS_REQUEST:
        console.log("[notification] - Access request.");        
        break;        
        
        default:
        console.log("[notification] - Unknown.");
      }
    break;

    // command from mobile device or to server
    case TYPE_COMMAND:
      switch(header.subtype) {

        case SUBTYPE_INIT:
        console.log("[command] - Init connection.");        
        break;

        case SUBTYPE_OPEN_DOOR:
        console.log("[command] - Open door."); 
        break;
        
        case SUBTYPE_LIST_USERS:
        console.log("[command] - List users."); 
        break;
        
        case SUBTYPE_NEW_USER:
        console.log("[command] - New user."); 
        break;        
        
        case SUBTYPE_DELETE_USER:
        console.log("[command] - Delete user."); 
        break; 
        
        case SUBTYPE_DISABLE:
        console.log("[command] - Disable robolock."); 
        break;                
        
        case SUBTYPE_TEXT_MSG:
        console.log("[command] - Text message."); 
        break;                
        
        case SUBTYPE_TAKE_IMG:
        console.log("[command] - Take image."); 
        break;       
        
        default:
        console.log("[command] - Unknown.");
      }
    break;
  }
}





/* 
 * node.js
 */

var net = require('net');
var http = require('http');
var url = require('url');

// TCP SERVER (ROBOLOCK INTERFACE
var tcp_server = net.createServer(function(c) { //'connection' listener
  console.log('server connected');

  c.on('end', function() {
    console.log('server disconnected');
  });

  c.on('data', function(data) {
        process_packet(data);
    });  

});

// HTTP SERVER (MOBILE INTERFACE)
var http_server = http.createServer(function (req, res) {
  //res.writeHead(200, {'Content-Type': 'text/html'});
 // res.end("<h1>RoboLock!</h1>");
});

http_server.on('request', function(req, res) {
  console.log(req.url);
  res.writeHead(200, {'Content-Type': 'text/html'});


  var url_header = url.parse(req.url, true);
  mobile_id = url_header.query.id;
  
  switch(url_header.pathname) {
    
    case OPEN_DOOR:
    console.log("[mobile] - Open door.")
    break;

    case LIST_USERS:
    console.log("[mobile] - List users.")
    break;

    case NEW_USER:
    console.log("[mobile] - Set new user.")
    break;

    case DELETE_USER:
    console.log("[mobile] - Delete user.")
    break;

    case DISABLE:
    console.log("[mobile] - Disable robolock.")
    break;

    case TEXT_MESSAGE:
    console.log("[mobile] - Send text message.")
    break;

    case TAKE_IMAGE:
    console.log("[mobile] - Take image.")
    break;

    default:
    console.log("[mobile] - Unknown.")
  }
res.end(url_header.pathname);
});


http_server.listen(8080);


tcp_server.listen(9090, function() { //'listening' listener
  console.log('server bound');
});