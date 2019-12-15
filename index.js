'use strict'

const EventEmitter = require('events').EventEmitter
const addon = require('bindings')('dump1090')
const emitter = new EventEmitter()

emitter.on('start', () => {
    console.log('### START ...')
})
emitter.on('data', (evt) => {
    console.log(evt);
})

emitter.on('end', () => {
    console.log('### END ###')
})

addon.callEmit(emitter.emit.bind(emitter))
