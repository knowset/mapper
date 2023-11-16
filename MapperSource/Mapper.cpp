#include "Mapper.hpp"
#include "Types/Geojson.hpp"
#include "napi.h"

#include <cassert>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

Napi::Object Mapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(
        env, "Mapper", { InstanceMethod("findRoutes", &Mapper::FindRoutes) });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Mapper", func);
    return exports;
}

Mapper::Mapper(Napi::CallbackInfo const& info)
    : Napi::ObjectWrap<Mapper>(info)
{
    Napi::Env env = info.Env();

    // 1. assert and parse geojson object
    assert(info.Length() == 1 && info[0].IsObject());

    parse_geojson_napi_object(info[0].As<Napi::Object>());
}

Napi::Value Mapper::FindRoutes(Napi::CallbackInfo const& info)
{
    assert(info.Length() > 1 && info[0].IsArray());

    auto positions = extract_linestring(info[0]);

    for (auto pos : positions) {
        std::cout << pos->lat() << "\t" << pos->lng() << "\n";
    }
 
    std::cout << "FINDROUTES\n";
    return info.Env().Undefined();
}

Napi::Value Mapper::CreateNewItem(Napi::CallbackInfo const& info)
{
    // Retrieve the instance data we stored during `Init()`. We only stored the
    // constructor there, so we retrieve it here to create a new instance of the
    // JS class the constructor represents.
    Napi::FunctionReference* constructor = info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({ Napi::Number::New(info.Env(), 42) });
}

// For debugging
static int count_coord = 0;
// For debugging
static void count_coord_fc() { std::cout << count_coord << "<<<<<\n"; }

void Mapper::parse_geojson_napi_object(Napi::Object const& network)
{

    if (!network.Has("type"))
        return;

    if (network.Get("type").ToString().Utf8Value() != geojson_objects_types_to_string(FEATURECOLLECTION)) {
        return;
    }

    if (!network.Has("features"))
        return;

    Napi::Value features_value = network.Get("features");

    if (!features_value.IsArray())
        return;

    Napi::Array features = features_value.As<Napi::Array>();

    if (features.Length() == 0)
        return;

    //  network = {
    //      type: "FeatureCollection",
    //      features: [...]
    //  }
    auto const start { std::chrono::steady_clock::now() };

    for (size_t i = 0; i < features.Length(); i++) {
        Napi::Value feature_as_napi_value = features[i];

        if (!feature_as_napi_value.IsObject())
            continue;

        //  feature = {
        //      type: "Feature",
        //      geometry: [Object],
        //      properties: [Object]
        //  }
        Napi::Object feature_as_napi_object = feature_as_napi_value.As<Napi::Object>();

        Napi::Value feature_type = feature_as_napi_object.Get("type");
        Napi::Value feature_geometry_as_napi_value = feature_as_napi_object.Get("geometry");

        if (feature_type.IsUndefined() || feature_geometry_as_napi_value.IsUndefined())
            continue;

        if (feature_type.ToString().Utf8Value() != "Feature")
            continue;

        if (!feature_geometry_as_napi_value.IsObject())
            continue;

        //  feature.geometry = {
        //      type: "Point" | "LineString" | "Polygon" |
        //            "MultiPoint" | "MultiLineString" |
        //            "MultiPolygon",
        //      coordinates: [] | [][] | [][][]
        //  }
        Napi::Object feature_geometry_as_napi_object = feature_geometry_as_napi_value.As<Napi::Object>();

        Napi::Value feature_geometry_type = feature_geometry_as_napi_object.Get("type");
        Napi::Value feature_geometry_coordinates_as_napi_value = feature_geometry_as_napi_object.Get("coordinates");

        if (feature_geometry_type.IsUndefined() || feature_geometry_coordinates_as_napi_value.IsUndefined())
            continue;

        switch (string_to_geojson_objects_types(
            feature_geometry_type.ToString().Utf8Value())) {
        case GeoJsonObjectsTypes::POINT:
            // std::cout << "Point\n";
            break;
        case GeoJsonObjectsTypes::LINESTRING: {
            auto ls = extract_linestring(feature_geometry_coordinates_as_napi_value);
            m_graph.append_nodes(ls);
            break;
        }
        case GeoJsonObjectsTypes::MUTLILINESTRING:
            // std::cout << "MultiLineString\n";
            break;
        default:
            break;
        }
    }

    auto const end { std::chrono::steady_clock::now() };
    std::chrono::duration<double> const es { end - start };
    std::cout << "time = " << es.count() << "\n";

    count_coord_fc();
    std::cout << m_graph.nodes().size() << "<<\n";

    // int i = 0;
    // for (auto const& [key, value] : m_graph.nodes()) {
    //     std::cout << '[' << key.first << "," << key.second
    //               << "] = " << value->children().size() << "\n";
    //     if (i == 10)
    //         break;
    //     i++;
    // }
}

std::vector<Position*> Mapper::extract_linestring(Napi::Value const& coordinates)
{
    if (!coordinates.IsArray())
        return {};

    Napi::Array coordinates_as_napi_array = coordinates.As<Napi::Array>();

    if (coordinates_as_napi_array.Length() == 0)
        return {};

    std::vector<Position*> positions(coordinates_as_napi_array.Length());

    // EXAMPLE:
    //
    //  "coordinates": [
    //          [37.7647266, 61.3744261],
    //          [37.7656584, 61.3741973],
    //          [37.7686768, 61.3744038],
    //      ]
    for (size_t i = 0; i < coordinates_as_napi_array.Length(); i++) {
        Napi::Value position_as_napi_value = coordinates_as_napi_array[i];

        if (!position_as_napi_value.IsArray())
            continue;

        //  "coordinates": [
        //      --> [37.7647266, 61.3744261],
        Napi::Array position_as_napi_array = position_as_napi_value.As<Napi::Array>();

        if (position_as_napi_array.Length() < 2)
            continue;

        Napi::Value lat_as_napi_value = position_as_napi_array.Get("0");
        Napi::Value lng_as_napi_value = position_as_napi_array.Get("1");

        if (lat_as_napi_value.IsUndefined() || lng_as_napi_value.IsUndefined())
            continue;

        // std::cout << lat_as_napi_value.ToNumber().FloatValue() << "\t" <<  lng_as_napi_value.ToNumber().FloatValue() << "<<\n";
        // if (count_coord > 10) return;

        positions[i] = new Position(
            lng_as_napi_value.ToNumber(),
            lat_as_napi_value.ToNumber());

        // count_coord++;
    }

    /*     m_graph.append_nodes(positions); */

    return positions;
}
