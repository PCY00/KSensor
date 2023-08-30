//시리얼포트 라이브리? 사용
const { SerialPort } = require('serialport’);
//데이터를 한 줄씩 읽어오고 파싱
const { ReadlineParser } = require('@serialport/parser-readline’);
//시리얼포트 열기
const port = new SerialPort({ path: '/dev/ttyUSB0', baudRate: 9600 });
const parser = new ReadlineParser();
port.pipe(parser);

//아두이노로 메시지를 보내는 함수
function write(data) {
    port.write(data, err => {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('Message written');
    });
}
//포트가 열려있을 때
port.on('open', () => {
    console.log('Serial port open’);
    //입력 받는다
    const rl = require('readline').createInterface({
        input: process.stdin,
        output: process.stdout
    });
    //exit 입력이면 닫기
    rl.question('Enter a value to send: ', data => {
        rl.close();
        console.log('Got input:', data);
        write(data);
    });
});

parser.on('data', data => {
    console.log('Got word from Arduino:', data);
});
