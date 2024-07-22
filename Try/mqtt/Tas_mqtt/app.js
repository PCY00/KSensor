/*
 * Created by J. Yun, SCH Univ. (yun@sch.ac.kr), 2019/3/1
 * - use DFR0238 RGB LED breakout with 'onoff' Node.js module
 * - use tas_sample created by I.-Y. Ahn, KETI
*/

var net = require('net');
var util = require('util');
var fs = require('fs');
var xml2js = require('xml2js');
var cp = require('child_process');

var wdt = require('./wdt');
var ledm = require('./ledm');

var useparentport = '';
var useparenthostname = '';

var upload_arr = [];
var download_arr = [];

var conf = {};

var status = 0;

//serialport setting
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const ports = [
    new SerialPort({ path: '/dev/ttyACM4', baudRate: 9600 }), 
    new SerialPort({ path: '/dev/ttyACM5', baudRate: 9600 }),
    new SerialPort({ path: '/dev/ttyACM6', baudRate: 9600 })
];
const parsers = ports.map(port => new ReadlineParser());

ports.forEach((port, index) => {
    port.pipe(parsers[index]);

    port.on('open', () => {
        console.log(`Serial port ${index} opened`);
        if (index === 0) status = 1;
    });

    parsers[index].on('data', data => {
        console.log(`Got word from Arduino ${index}:`, data);
    });
});

// This is an async file read
fs.readFile('conf.xml', 'utf-8', function (err, data) {
    if (err) {
        console.log("FATAL An error occurred trying to read in the file: " + err);
        console.log("error : set to default for configuration")
    }
    else {
        var parser = new xml2js.Parser({explicitArray: false});
        parser.parseString(data, function (err, result) {
            if (err) {
                console.log("Parsing An error occurred trying to read in the file: " + err);
                console.log("error : set to default for configuration")
            }
            else {
                var jsonString = JSON.stringify(result);
                conf = JSON.parse(jsonString)['m2m:conf'];

                useparenthostname = conf.tas.parenthostname;
                useparentport = conf.tas.parentport;

                if (conf.upload != null) {
                    if (conf.upload['ctname'] != null) {
                        upload_arr[0] = conf.upload;
                    }
                    else {
                        upload_arr = conf.upload;
                    }
                }

                if (conf.download != null) {
                    if (conf.download['ctname'] != null) {
                        download_arr[0] = conf.download;
                    }
                    else {
                        download_arr = conf.download;
                    }
                }
            }
        });
    }
});

var tas_state = 'init';
var upload_client = null;
var t_count = 0;
var tas_download_count = 0;

function on_receive(data) {
    if (tas_state == 'connect' || tas_state == 'reconnect' || tas_state == 'upload') {
        var data_arr = data.toString().split('<EOF>');
        if (data_arr.length >= 2) {
            for (var i = 0; i < data_arr.length - 1; i++) {
                var line = data_arr[i];
                var sink_str = util.format('%s', line.toString());
                var sink_obj = JSON.parse(sink_str);

                if (sink_obj.ctname == null || sink_obj.con == null) {
                    console.log('Received: data format mismatch');
                }
                else {
                    if (sink_obj.con == 'hello') {
                        console.log('Received: ' + line);

                        if (++tas_download_count >= download_arr.length) {
                            tas_state = 'upload';
                        }
                    }
                    else {
                        for (var j = 0; j < upload_arr.length; j++) {
                            if (upload_arr[j].ctname == sink_obj.ctname) {
                                console.log('ACK : ' + line + ' <----');
                                break;
                            }
                        }

                        for (var j = 0; j < download_arr.length; j++) {
                            if (download_arr[j].ctname == sink_obj.ctname) {
                                g_down_buf = JSON.stringify({id: download_arr[j].ctname, con: sink_obj.con});
                                console.log(g_down_buf + ' <----');
                                control_led(sink_obj.con);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

function control_led(comm_num) {
    const inputData = comm_num;

    if (status === 1) {
        function write(data) {
            ports.forEach((port, index) => {
                port.write(data, err => {
                    if (err) {
                        return console.log(`Error on write to port ${index}: `, err.message);
                    }
                    console.log(`Message written to port ${index}`);
                });
            });
        }

        setTimeout(() => {
            console.log('Sending data');
            write(inputData);
        }, 3000);
    }
}

function tas_watchdog() {
    if (tas_state == 'init') {
        upload_client = new net.Socket();

        upload_client.on('data', on_receive);

        upload_client.on('error', function(err) {
            console.log(err);
            tas_state = 'reconnect';
        });

        upload_client.on('close', function() {
            console.log('Connection closed');
            upload_client.destroy();
            tas_state = 'reconnect';
        });

        if (upload_client) {
            console.log('tas init ok');
            tas_state = 'init_thing';
        }
    }
    else if (tas_state == 'init_thing') {
        // init things
        control_led('0');

        tas_state = 'connect';
    }
    else if (tas_state == 'connect' || tas_state == 'reconnect') {
        upload_client.connect(useparentport, useparenthostname, function() {
            console.log('upload Connected');
            tas_download_count = 0;
            for (var i = 0; i < download_arr.length; i++) {
                console.log('download Connected - ' + download_arr[i].ctname + ' hello');
                var cin = {ctname: download_arr[i].ctname, con: 'hello'};
                upload_client.write(JSON.stringify(cin) + '<EOF>');
            }

            if (tas_download_count >= download_arr.length) {
                tas_state = 'upload';
            }
        });
    }
}

// Every 3 seconds, check if the TAS is not working
wdt.set_wdt(require('shortid').generate(), 3, tas_watchdog);
