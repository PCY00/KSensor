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
port.on('open', () => {
    console.log('Serial port open');

    // 명령줄에서 입력된 값을 받아옴
    const inputData = process.argv[2];
    
    // 입력값이 없으면 프로그램 종료
    if (!inputData) {
        console.log('No input value provided. Exiting...');
        process.exit(1);
    }

    // 입력값을 시리얼 포트로 전송
    console.log('Got input:', inputData);
    //아두이노랑 통신할 때 딜레이가 필요함..
    setTimeout(() => {
        console.log('send');
        write(inputData);
    }, 5000);
});

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});
