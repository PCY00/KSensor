/*function write(data) {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
    });
}

port.on('open', () => {
    console.log('Serial port open');

    // 명령줄에서 입력된 값을 받아옴
    const inputData = process.argv[2];
    
    // 입력값이 없으면 프로그램 종료
    if (!inputData) {
        console.log('No input value provided. Exiting...');
        process.exit(1);
    }

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
const write = (port, data) => {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
    });
};

port.on('open', () => {
    console.log('Serial port open');

    // 명령줄에서 입력된 값을 받아옴
    const inputData = process.argv[2];
    
    // 입력값이 없으면 프로그램 종료
    if (!inputData) {
        console.log('No input value provided. Exiting...');
        process.exit(1);
    }

    console.log('Got input:', inputData);
    
    setTimeout(() => {
        console.log('send');
        write(inputData);
    }, 5000);
});

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});
*/

//mi.js
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 }); // Create the port object
const parser = new ReadlineParser();
port.pipe(parser);

const inputData = process.argv[2];

function write(data) {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
        
        // 작업이 끝나면 ma.js로 돌아가기
        process.send({ done: true });
    });
}

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});

port.on('open', () => {
    console.log('Serial port opened');
    
    if (inputData === '1') {
        setTimeout(() => {
            console.log('Sending data');
            write('ok');
        }, 5000);
    } else if (inputData === '0') {
        setTimeout(() => {
            console.log('Sending data');
            write('no');
        }, 5000);
    }
});

port.on('data', () => {
    console.log('Work completed. Exiting...');
    process.exit(0);
});
