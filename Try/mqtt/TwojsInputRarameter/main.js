const readline = require('readline');

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
});
