'use strict'

const EventEmitter = require('events').EventEmitter;
const NativeEmitter = require('bindings')('dump1090').NativeEmitter;
const inherits = require('util').inherits;

inherits(NativeEmitter, EventEmitter);

const emitter = new NativeEmitter();

emitter.on('start', () => {
    console.log('Node-dump1090 entering event loop');
})

emitter.on('data', (evt) => {
    console.log(evt);
    JSON.parse(evt);
})

emitter.callAndEmit();
