#include <napi.h>

#include "Mapper.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports) 
{
    Mapper::Init(env, exports);
    return exports;
}

NODE_API_MODULE(Mapper, Init);
