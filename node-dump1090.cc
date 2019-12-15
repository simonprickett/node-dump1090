#include <napi.h>
extern "C" {
  #include "dump1090.h"
}

class NativeEmitter : public Napi::ObjectWrap<NativeEmitter> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports) {
          Napi::HandleScope scope(env);

          Napi::Function func = DefineClass(env, "NativeEmitter", {
            InstanceMethod("callAndEmit", &NativeEmitter::CallAndEmit)
          });

          constructor = Napi::Persistent(func);
          constructor.SuppressDestruct();

          exports.Set("NativeEmitter", func);
          return exports;
        }

        NativeEmitter(const Napi::CallbackInfo& info) : Napi::ObjectWrap<NativeEmitter>(info) {
        }
    private:
        static Napi::FunctionReference constructor;
        Napi::Value CallAndEmit(const Napi::CallbackInfo& info) {
          Napi::Env env = info.Env();
          Napi::Function emit = info.This().As<Napi::Object>().Get("emit").As<Napi::Function>();
          modesInitConfig();
          modesInit();
          modesInitRTLSDR();
          pthread_create(&Modes.reader_thread, NULL, readerThreadEntryPoint, NULL);
          pthread_mutex_lock(&Modes.data_mutex);
          emit.Call(info.This(), { Napi::String::New(env, "start") });
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
              std::string json(aircrafts, clen);
              if (json.size() > 3) {
                emit.Call(info.This(), { Napi::String::New(env, "data"), Napi::String::New(env, json) });
              }
              Modes.interactive_last_update = mstime();
            }
            pthread_mutex_lock(&Modes.data_mutex);
            if (Modes.exit) break;
          }
          rtlsdr_close(Modes.dev);
          emit.Call(info.This(), { Napi::String::New(env, "end") });
          return Napi::String::New(env, "OK");
        }
};
Napi::FunctionReference NativeEmitter::constructor;
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    NativeEmitter::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
