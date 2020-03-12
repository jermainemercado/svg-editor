'use strict'

// C library API
const ffi = require('ffi-napi');
let svgLib = ffi.Library('./parser/libsvgparse', {
  "imageToJSON" : [ "string" , ["string"] ],
  "titleDescToJSON" : [ "string" , ["string"] ],
  "circListToJSON" : [ "string" , ["string"] ],
  "rectListToJSON" : [ "string" , ["string"] ],
  "pathListToJSON" : [ "string" , [ "string" ] ], 
  "groupListToJSON" : [ "string" , [ "string" ] ]
});

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

app.get('/getSVGFiles', function(req , res){
  let arrayFiles = uploadFiles();
  let svgFiles = [];
  for (let i in arrayFiles) {
    let svgFile = JSON.parse(svgLib.imageToJSON("./uploads/" + arrayFiles[i]));

    svgFiles.push(svgFile);
  }
  res.send(svgFiles);
});

app.get('/parseTitleDesc', function(req, res){
  let file = req.query.sendFile;
  let jsonObject = JSON.parse(svgLib.titleDescToJSON("./uploads/" + file));
  
  res.send(jsonObject);
});

app.get('/getComponents', function(req, res) {
  let file = req.query.sendFile;
  let rectangles = JSON.parse(svgLib.rectListToJSON("./uploads/" + file));
  let circles = JSON.parse(svgLib.circListToJSON("./uploads/" + file));
  let paths = JSON.parse(svgLib.pathListToJSON("./uploads/" + file));
  let groups = JSON.parse(svgLib.groupListToJSON("./uploads/" + file));

  for (let i = 0; i < rectangles.length ; i++) {
      console.log(rectangles[i]);
  }
  for (let i = 0; i < circles.length ; i++) {
      console.log(circles[i]);
  }
  for (let i = 0; i < paths.length ; i++) {
      console.log(paths[i]);
  }
  for (let i = 0; i < groups.length ; i++) {
      console.log(groups[i]);
  }
  res.send({
    rectangles: rectangles,
    circles: circles,
    paths: paths,
    groups: groups,
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);



function uploadFiles() {
  var fileList = [];
  var files = fs.readdirSync('./uploads');

  for (var i in files)
  {
    var name = files[i];
    var check = name.includes(".svg");

    if (check == true) {
      var jsonObject = JSON.parse(svgLib.imageToJSON("./uploads/" + name));
      if (jsonObject != null) {
        fileList.push(name);
      }
    }
  }
  return fileList;
}
