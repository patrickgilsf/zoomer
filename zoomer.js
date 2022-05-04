#!/usr/bin/env node

//node HID import
var HID = require('node-hid');
const { exec } = require('child_process');

//Device in for for ymd09
var VENDOR_ID = 0x594D; //22861
var PRODUCT_ID = 0x4409; //17417
var USAGE_PAGE = 0xFF60; //65376
var usagePageNumber = 65376;
var USAGE = 0x61; //97
var deviceList = HID.devices();
// console.log(deviceList);
var path = ''; 

deviceList.forEach(item => {
  if (item.usagePage == USAGE_PAGE) {
    path = item.path;
  }
});
if (path) {
  console.log(`path (${path}) found!`);
  var device = new HID.HID(path);
} else {
  console.log('No controller plugged in');
}




const muteMicStopVid = () => {
    device.write([0x00, 0x01, 0x01, 0x01, 0x01, 0x01])
};
const muteMicStartVid = () => {
    device.write([0x00, 0x02, 0x02, 0x02, 0x02, 0x02])
};
const unmuteMicStopVid = () => {
  device.write([0x00, 0x03, 0x03, 0x03, 0x03, 0x03])
};
const unmuteMicStartVid = () => {
  device.write([0x00, 0x04, 0x04, 0x04, 0x04, 0x04])
};






var checkStatus = function() {
    // console.log("Attempting to fire apple script. If there are multiple attempts, Zoom not be open")
    exec('osascript zoomer.scpt', (error, stdout, stderr) => {
      console.log('Running Apple Script...');
      console.log('apple script status: ',stdout);
      if (error) {
          console.error(`exec error: ${error, stderr}`);
          return;
      }
      var status = JSON.parse(stdout);
      // console.log(status)
      if (status.stat == "closed") {
        console.log("you need to open Zoom");
      } else if (status.stat == "open") {
        console.log("Are you sure you are in a Zoom meeting right now?")
      };
      if (status.stat == "call") {
        // console.log('zoom is open');
        if (status.mute == "muted" && status.video == "stop") {
          console.log('mic and camera red');
          muteMicStopVid();
        } else if (status.mute == "muted" && status.video == "start") {
          console.log('mic red');
          muteMicStartVid();
        } else if (status.mute == "unmuted" && status.video == "stop") {
          console.log('camera red');
          unmuteMicStopVid();
        } else if (status.mute == "unmuted" && status.video == "start") {
          console.log('mic and cam are on');
          unmuteMicStartVid();
        }
      } 
    });
}

if (device) {
  checkStatus();
  setInterval(checkStatus, 3000);
} else {
  console.log(`No device connected, please connect Zoom controller`);
}
