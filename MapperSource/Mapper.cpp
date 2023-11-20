#include "Mapper.hpp"
#include "Node.hpp"
#include "Position.hpp"
#include "Types/Geojson.hpp"
#include "napi.h"

#include <cassert>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
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

    parse_geojson_napi_object(info[0].As<Napi::Object>(), info.Env());
}

Napi::Value Mapper::FindRoutes(Napi::CallbackInfo const& info)
{
    // 1
    assert(info.Length() > 1 && info[0].IsArray());

    auto positions = extract_linestring(info[0], {}, info.Env());

    if (positions.size() < 2) {
        return Napi::Array::New(info.Env(), 0);
    }

    // FIXME: There can be more than 2 paths, do it through a 'for' loop
    auto start = m_graph.nodes()[Node::round_coordinate(positions[0]->position())];
    auto end = m_graph.nodes()[Node::round_coordinate(positions[1]->position())];

    std::cout << Position::distance(positions[0]->position(), positions[1]->position()) << " #pos\n";

    std::cout << start->children().size() << "##########\n";

    // FIXME: The point can be very far from the graph node.
    //        If the point is not found on the graph, then
    //        iterate the graph to find the nearest point
    if (start) {
        std::cout << "Start point found in graph\n";
    }

    if (end) {
        std::cout << "End point found in graph\n";
    }

    std::vector<Node*> reachable = { start };

    auto test_node = Node::choose_nearest_node(start->children(), end);
    
    std::cout << test_node->position().lat() << " - " << test_node->position().lng() << " # test_node\n";

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

void Mapper::parse_geojson_napi_object(Napi::Object const& network, Napi::Env const& env)
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

    Napi::Object features = features_value.As<Napi::Object>();

    //  network = {
    //      type: "FeatureCollection",
    //      features: [...]
    //  }
    auto const start { std::chrono::steady_clock::now() };

    for (auto feature : features) {
        Napi::HandleScope scope(env);

        Napi::Object feature_as_napi_object = static_cast<Napi::Value>(feature.second).As<Napi::Object>();
        //  feature = {
        //      type: "Feature",
        //      geometry: [Object],
        //      properties: [Object]
        //  }
        Napi::Value feature_type = feature_as_napi_object.Get("type");
        Napi::Value feature_geometry_as_napi_value = feature_as_napi_object.Get("geometry");
        Napi::Value feature_properties_as_napi_value = feature_as_napi_object.Get("properties");

        // std::cout << feature_type.ToString().Utf8Value() << "\n";

        if (feature_type.IsUndefined() || feature_geometry_as_napi_value.IsUndefined())
            continue;

        if (feature_type.ToString().Utf8Value() != "Feature")
            continue;

        if (!feature_geometry_as_napi_value.IsObject())
            continue;

        Properties* properties = extract_properties(feature_properties_as_napi_value);

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

        // switch (string_to_geojson_objects_types(
        //     feature_geometry_type.ToString().Utf8Value())) {
        // case GeoJsonObjectsTypes::LINESTRING: {
        //     auto ls = extract_linestring(feature_geometry_coordinates_as_napi_value, properties, env);
        //     m_graph.append_nodes(ls);
        //     break;
        // }
        // case GeoJsonObjectsTypes::MUTLILINESTRING:
        //     // FIXME: add definition here
        //     break;
        // default:
        //     break;
        // }
        auto type_as_std_string = feature_geometry_type.ToString().Utf8Value();
        if (type_as_std_string == "LineString") {
            auto ls = extract_linestring(feature_geometry_coordinates_as_napi_value, properties, env);
            m_graph.append_nodes(ls);
        } else if (type_as_std_string == "MultiLineString") {
            continue;
        } else {
            continue;
        }
    }

    auto const end { std::chrono::steady_clock::now() };

    std::chrono::duration<double> const es { end - start };
    std::cout << "time = " << es.count() << "\n";

    // std::cout << m_graph.nodes().size() << "<<\n";
    //
    // int i = 0;
    // for (auto const& [key, value] : m_graph.nodes()) {
    //     std::cout << '[' << key.first << "," << key.second
    //               << "] = " << value->options().at("OSM_TYPE") << "\n";
    //     if (i == 5)
    //         break;
    //     i++;
    // }
}

std::vector<Node*> Mapper::extract_linestring(Napi::Value const& feature_geometry_coordinates_as_napi_value,
    Properties* properties, Napi::Env const& env)
{
    if (!feature_geometry_coordinates_as_napi_value.IsArray())
        return {};

    Napi::Array coordinates_as_napi_array = feature_geometry_coordinates_as_napi_value.As<Napi::Array>();

    // if (coordinates_as_napi_array.Length() == 0)
    //     return {};
    size_t position_length = coordinates_as_napi_array.As<Napi::Array>().Length();

    std::vector<Node*> positions(position_length);

    //  "coordinates": [
    //          [37.7647266, 61.3744261],
    //          [37.7656584, 61.3741973],
    //          [37.7686768, 61.3744038],
    //      ]
    for (size_t i = 0; i < position_length; i++) {
        Napi::Object position_as_napi_object = static_cast<Napi::Value>(coordinates_as_napi_array[i]).As<Napi::Object>();
        //  "coordinates": [
        //      --> [37.7647266, 61.3744261],
        Napi::Value lat_as_napi_value = position_as_napi_object.Get("0");
        Napi::Value lng_as_napi_value = position_as_napi_object.Get("1");

        if (lat_as_napi_value.IsUndefined() || lng_as_napi_value.IsUndefined())
            continue;

        positions[i] = new Node(new Position(
                                    lng_as_napi_value.ToNumber(),
                                    lat_as_napi_value.ToNumber()),
            properties);
    }
    // m_graph.append_nodes(positions);
    return positions;
}

Properties* Mapper::extract_properties(Napi::Value const& properties_as_napi_value)
{
    if (!properties_as_napi_value.IsObject()) {
        return {};
    }
    Napi::Object properties_as_napi_object = properties_as_napi_value.As<Napi::Object>();

    Properties properties;

    for (auto property : properties_as_napi_object) {
        std::string key = static_cast<Napi::Value>(property.first).ToString().Utf8Value();
        std::string value = static_cast<Napi::Value>(property.second).ToString().Utf8Value();
        if (value.length() > 0) {
            properties[key] = value;
        }
    }

    return new Properties(properties);
}
