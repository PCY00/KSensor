/*const readline = require('readline');

//serial setting
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
const parser = new ReadlineParser();
port.pipe(parser);

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

rl.question('Enter a value: ', (value) => {
    rl.close();

    const { fork } = require('child_process');
    const child = fork('mi.js', [value]);

    child.on('close', (code) => {
        console.log(`mi.js process exited with code ${code}`);
    });
});*/

/*const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
const parser = new ReadlineParser();
port.pipe(parser);


function write(data) {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
    });
}

port.on('open', () => {
    const inputData = process.argv[2];
    
    console.log('Serial port open');
    console.log('Got input:', inputData);
    setTimeout(() => {
        console.log('send');
        write(inputData);
    }, 5000);
});

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});*/

/*
const readline = require('readline');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
const parser = new ReadlineParser();
port.pipe(parser);

function write(data) {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
    });
}

function getUserInput(callback) {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.question('Enter a value: ', data => {
        rl.close();
        console.log('Got input:', data);
        callback(data); // 콜백 함수로 입력값 전달
    });
}

port.on('open', () => {
    console.log('Serial port opened');
    
    getUserInput(inputData => {
        if(inputData === 'ok'){
            setTimeout(() => {
                console.log('Sending data');
                write(inputData);
            }, 5000);
        }
    });
});

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});

*/
/*
const readline = require('readline');
const cp = require('child_process');

function getUserInput(callback) {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.question('Enter a value (or type "exit" to quit): ', data => {
        rl.close();
        console.log('Got input:', data);
        callback(data); // 콜백 함수로 입력값 전달
    });
}

function control_led(comm_num) {
    var parent_process = cp.fork("mi.js", [comm_num]);
    parent_process.on('close', function (code) {
        console.log('Child process is exiting with exit code: ' + code);
        
        // 일이 끝나면 다시 입력 받기
        getUserInput(inputData => {
            control_led(inputData); // 다시 control_led 호출
        });
    });
}

getUserInput(inputData => {
    control_led(inputData); // 처음 입력을 받고 시작
});*/

//ma.js
const readline = require('readline');
const cp = require('child_process');

function getUserInput(callback) {
    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout
    });

    rl.question('Enter a value (or type "exit" to quit): ', data => {
        rl.close();
        console.log('Got input:', data);
        callback(data); // 콜백 함수로 입력값 전달
    });
}

function control_led(comm_num, doneCallback) {
    var parent_process = cp.fork("mi.js", [comm_num]);
    parent_process.on('close', function (code) {
        console.log('Child process is exiting with exit code: ' + code);
        doneCallback(); // 작업이 끝나면 콜백 함수 호출
    });
}

function mainLoop() {
    getUserInput(inputData => {
        if (inputData.toLowerCase() === 'exit') {
            console.log('Exiting...');
            process.exit(0); // 프로그램 종료
        } else {
            control_led(inputData, () => {
                mainLoop(); // 작업이 끝난 후에 다시 반복
            });
        }
    });
}

mainLoop(); // 프로그램 시작

