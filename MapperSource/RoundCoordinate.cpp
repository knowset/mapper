#include <napi.h>

#include <cassert>
#include <cmath>

static Napi::Value RoundCoordinate(const Napi::CallbackInfo &info)
{
    // export function roundCoordinate(
    //     coordinate : Coordinates) : Coordinates | undefined;

    assert(info.Length() == 2 && info[0].IsArray() && info[1].IsNumber());

    Napi::Array coordinate = info[0].As<Napi::Array>();
    Napi::Number num = info[1].As<Napi::Number>();

    if (coordinate.Length() != 2)
    {
        return info.Env().Undefined();
    }

    Napi::Float64Array coordinate_result = Napi::Float64Array::New(info.Env(), coordinate.Length());

    for (size_t i = 0; i < coordinate.Length(); i++)
    {
        Napi::Value c = coordinate[i];
        if (c.IsNumber())
        {
            coordinate_result[i] = std::round(c.As<Napi::Number>().FloatValue() / num.FloatValue()) * num.FloatValue();
        }
    }

    return coordinate_result;
}
