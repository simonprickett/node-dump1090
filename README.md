# node-dump1090

NodeJS wrapper of dump1090

## Dependencies
- libusb
- librtlsdr

On MacOS with `brew` they can be staisfied just by running: `brew install librtlsdr`

## Usage

1. `npm install --save node-dump1090`
2. Use following code to receive events:
```
const EventEmitter = require('events').EventEmitter;
const dump1090 = require('./node_modules/node-dump1090/build/Release/dump1090.node');
const NativeEmitter = dump1090.NativeEmitter;
const inherits = require('util').inherits;
inherits(NativeEmitter, EventEmitter);

const emitter = new NativeEmitter();

emitter.on('data', (evt) => {
    console.log(evt);
})

emitter.callAndEmit();
```
