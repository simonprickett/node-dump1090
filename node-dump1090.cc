#include <napi.h>
#include <iostream>
extern "C" {
  #include "dump1090.h"
}

Napi::Value CallEmit(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Function emit = info[0].As<Napi::Function>();
  modesInitConfig();
  modesInit();
  modesInitRTLSDR();
  pthread_create(&Modes.reader_thread, NULL, readerThreadEntryPoint, NULL);
  pthread_mutex_lock(&Modes.data_mutex);
  emit.Call({Napi::String::New(env, "start")});
  while(1) {
    if (!Modes.data_ready) {
      pthread_cond_wait(&Modes.data_cond,&Modes.data_mutex);
      continue;
    }    
    computeMagnitudeVector();

    Modes.data_ready = 0;  
    pthread_cond_signal(&Modes.data_cond);

    pthread_mutex_unlock(&Modes.data_mutex);
    detectModeS(Modes.magnitude, Modes.data_len/2);
    if ((mstime() - Modes.interactive_last_update) > MODES_INTERACTIVE_REFRESH_TIME) {
      int clen;
      char* aircrafts = aircraftsToJson(&clen);
      emit.Call({Napi::String::New(env, "data"), Napi::String::New(env, std::string(aircrafts, clen))});
      Modes.interactive_last_update = mstime();
    }    

    pthread_mutex_lock(&Modes.data_mutex);
    if (Modes.exit) break;
  }    

  //rtlsdr_close(Modes.dev);
  emit.Call({Napi::String::New(env, "end")});
  return Napi::String::New(env, "OK");
}

// Init
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "callEmit"), Napi::Function::New(env, CallEmit));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
