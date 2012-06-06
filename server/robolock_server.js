var net = require('net');
var http = require('http');
var url = require('url');
var fs = require('fs');



function notifyRobolock(type, message) {
	try {

		connection.write(type+"/"+message+"\0");
		console.log("NotifyRobolock: "+type+message);
	} catch (err) {
		console.log("ROBOLOCK DOWN?");
	}
}


function notifyPhone() {
	var CD2M_command = fs.readFileSync('./notify.txt').toString();
	var sys = require('sys')
	var exec = require('child_process').exec;
	function puts(error, stdout, stderr) { sys.puts(stdout) }
	exec(CD2M_command, puts);
	//C2DM Code
	console.log("NOTIFYING PHONE!");
}

function notifyPhonePhoto() {
	var CD2M_command = fs.readFileSync('./photo.txt').toString();
	var sys = require('sys')
	var exec = require('child_process').exec;
	function puts(error, stdout, stderr) { sys.puts(stdout) }
	exec(CD2M_command, puts);
	//C2DM Code
	console.log("NOTIFYING PHONE OF PHOTO!");
}


var FILE_OPEN = false;

var connection;
var startIndex = 0;


var codesReady = 0;

//TCP SERVER
var tcp_server = net.createServer(function(c) { //'connection' listener
	console.log('[TCP-Server] Connected');

	connection = c;

	c.on('end', function() {
		console.log('[TCP-Server] Disconnected');
	});

	c.on('data', function(data) {


		console.log("[TCP REQUEST] ");
		console.log(data);
		console.log("\n");

		var splitData = data.toString().split("/", 2);


		var header = splitData[0];
		if (splitData[1] != null)
			var payload = data.slice((header.length + 1));

		console.log("-------------");
		console.log(header);
		console.log(payload);
		console.log("-------------\n\n\n");

		switch(header /* payload type (photo, codes) */) {

			case "photo":

				if (FILE_OPEN) {

					if (payload == "END") {
						fs.closeSync(fd);
						FILE_OPEN = false;
						startIndex = 0;
						notifyPhonePhoto();
					console.log("CLOSING FILE");

					} else {
						startIndex += fs.writeSync(fd, payload, 0, payload.length, startIndex);
						console.log("WRITING FILE");
					//fs.writeSync(fd, buffer, offset, length, position)
					}

				} else {
					console.log("OPENING FILE");
				//	notifyPhone();
					fd = fs.openSync('./images/photo.jpg', 'w+', 0666);
					startIndex += fs.writeSync(fd, payload, 0, payload.length, startIndex);
					FILE_OPEN = true;

					console.log("WRITING FILE");

				}

				console.log(startIndex+ "\n")
			 //notifyRobolock("OK", "Received last chunk");


			break;

			case "codes":
			console.log("CODES...")
			console.log(data.toString());
			 fd = fs.openSync('./text/codes.txt', 'w+', 0666);
			 fs.writeSync(fd, payload.toString(), 0, 'utf8');
			 fs.closeSync(fd);
			 codesReady = 1;

     //notifyRobolock("OK", "Received codes");

			break;

			default:
     notifyRobolock("ERROR", "Unknown Payload Type");
			break;

		}

	});  

});























// HTTP SERVER 
var http_server = http.createServer(function (req, res) {
	//res.writeHead(200, {'Content-Type': 'text/html'});
 // res.end("<h1>RoboLock!</h1>");
});

http_server.on('request', function(req, res) {

	console.log("[HTTP REQUEST] ");
	console.log(req);
	console.log("\n");
	var request = url.parse(req.url, true);
	var action = request.pathname;
	var q = request.query;

	/* request a photo 
	 *
	 * parameters: ondemand
	 */
	if (action == '/photo') {
	 var img;
	 try {
		img = fs.readFileSync('./images/photo.jpg');
		console.log("Sending image photo.jpg")
	 } 
	 catch(err) {
		console.log("Error; Sending image logo.jpg")
		img = fs.readFileSync('./images/logo.jpg')
		console.log("Read photo error: " + err);
	 }

	 res.writeHead(200, {'Content-Type': 'image/jpg' });
	 res.end(img, 'binary');

	/* registering with C2DM 
	 *
	 * parameters: id
	 *
	 * note: not currently used
	 */
	} else if (action == '/takephoto') {

		notifyRobolock("IMG/");
		res.writeHead(200, {'Content-Type': 'text/plain' });
		res.end('Taking Photo \n');
		console.log("Take Photo!");

	/* unlock door 
	 *
	 *
	 */
	} else if (action == '/register') {

		var registration_id = q.id;
		res.writeHead(200, {'Content-Type': 'text/plain' });
		res.end('Registration successful ' + registration_id + "\n");
		console.log("ID: " + registration_id);

	/* unlock door 
	 *
	 *
	 */
	} else if (action == '/unlock') {

		notifyRobolock("OK/");
		res.writeHead(200, {'Content-Type': 'text/plain' });
		console.log("unlocking");
		res.end("Unlocking");

	/* send text message to door 
	 *
	 * parameters: msg
	 *
	 * note: URI-encode the message client-side
	 */
	} else if (action == '/greeting') {

		var message = decodeURIComponent(q.msg);

		res.writeHead(200, {'Content-Type': 'text/plain' });
		res.end("Text sent\n");

		console.log("texting robolock " + message);
		notifyRobolock("TXT/", message + "\0");

	/* set new code 	 
	 *
	 * parameters: code
	 */
	} else if (action == '/setcode') {

		var code = q.code;
		console.log("setting code "+ code);
		notifyRobolock("SET/" + code);
		res.writeHead(200, {'Content-Type': 'text/plain' });
		res.end("Setting code" + code);

	/* get all active codes 
	 *
	 * parameters: [none]
	 */
	} else if (action == '/getcodes') {

		notifyRobolock("GET/");
		console.log("getting codes"); 


			setTimeout(function() {

				res.writeHead(200, {'Content-Type': 'text/plain' });

				if (codesReady) {
					var codes = fs.readFileSync("./text/codes.txt");
					res.end(codes);
					codesReady = 0;

				} else {
						res.end("No Codes Set\n");
					}
				}, 1000);

			console.log("hi");




	/* invalidate a particular code 
	 *
	 * parameters: code
	 */
	} else if (action == '/invalidate') {

		var code = q.code;
		notifyRobolock("DEL/"+ code);
		console.log("deleting code" + code); 
		res.writeHead(200, {'Content-Type': 'text/plain' });
		res.end("Invalidating Code " + code);

	} else { 

	 res.writeHead(200, {'Content-Type': 'text/plain' });
	 res.end('Hello World \n');

	}
});









http_server.listen(8080, function() {
	console.log('[HTTP-Server] Bound on port 8080');
});


tcp_server.listen(9090, function() { //'listening' listener
	console.log('[TCP-Server] Bound on port 9090');
});


