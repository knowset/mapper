#include "Mapper.hpp"
#include "Node.hpp"
#include "Position.hpp"
#include "Types/Geojson.hpp"
#include "napi.h"

#include <cassert>
#include <future>
#include <iostream>
#include <limits>
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

struct Ways {
    NodeWrapper* highway = nullptr;
    // Node* footway = nullptr;
};

// "HIGHWAY":"bridleway"
// "HIGHWAY":"construction"
// "HIGHWAY":"cycleway"
// "HIGHWAY":"footway"
// "HIGHWAY":"path"
// "HIGHWAY":"pedestrian"
// "HIGHWAY":"primary"
// "HIGHWAY":"proposed"
// "HIGHWAY":"raceway"
// "HIGHWAY":"residential"
// "HIGHWAY":"road"
// "HIGHWAY":"secondary"
// "HIGHWAY":"service"
// "HIGHWAY":"steps"
// "HIGHWAY":"tertiary"
// "HIGHWAY":"track"
// "HIGHWAY":"trunk"
// "HIGHWAY":"unclassified"

Napi::Value Mapper::FindRoutes(Napi::CallbackInfo const& info)
{
    // 1
    assert(info.Length() > 1 && info[0].IsArray());

    auto positions = extract_linestring(info[0], {}, info.Env());

    if (positions.size() < 2) {
        return Napi::Array::New(info.Env(), 0);
    }

    // FIXME: There can be more than 2 paths, do it through a 'for' loop
    // auto start = m_graph.nodes()[positions[0]->position()];
    // auto end = m_graph.nodes()[positions[1]->position()];

    auto find_nearest_point = [&](Node* point) {
        Ways w_point;
        double min_dist_highway_point = std::numeric_limits<double>::max();
        double min_dist_footway_point = std::numeric_limits<double>::max();

        for (auto const& graph_node : m_graph.nodes()) {
            auto type = graph_node.second->options()["HIGHWAY"];
            if (type == "footway" || type == "steps") {
                // double dist = Position::distance(point->position(), graph_node.second->position());
                // if (dist < min_dist_footway_point) {
                //     w_point.footway = graph_node.second;
                //     min_dist_footway_point = dist;
                // }
            } else if (type != "footway") {
                double dist = Position::distance(point->position(), graph_node.second->position());
                if (dist < min_dist_highway_point) {
                    w_point.highway = new NodeWrapper(graph_node.second);
                    min_dist_highway_point = dist;
                }
            }
        }
        return w_point;
    };

    Ways w_start = find_nearest_point(positions[0]);
    Ways w_end = find_nearest_point(positions[1]);

    std::cout << "START: " << w_start.highway->node().position().lat() << " - " << w_start.highway->node().position().lng() << "\n";
    std::cout << "END: " << w_end.highway->node().position().lat() << " - " << w_end.highway->node().position().lng() << "\n";

    // if (w_start.footway != nullptr)
    //     std::cout << w_start.footway->position().lat() << " - " << w_start.footway->position().lng() << " <<<<\n";
    // std::cout << Position::distance(positions[0]->position(), positions[1]->position()) << " #pos\n";
    //

    if (!w_start.highway) {
        // std::cout << "Start point found in graph\n";
        throw Napi::Error::New(info.Env(), "start_point not found!");
    }

    // GLOBALFIXME:
    //      Определение начальной и конечной точки
    //      1) ищем в Mapper::m_graph точку start
    //      2) если она есть, смотрим ее тип автодорога или тратуар
    //      3) перебором ищем ближайший к этой точке оставшийся тип (автодорога или тратуар)
    //      4) все тоже самое проделываем с точкой end
    //
    //     Нахождение пути
    //     1) используя алгоритм A* дважды ищем путь для двух типов дорог (автодорога или тратуар)
    //     2) возвращаем объект:
    //          ways: {
    //              highway: [number, number][],
    //              footway: [number, number][]
    //          }

    if (!w_end.highway) {
        throw Napi::Error::New(info.Env(), "end_point not found!");
    }

    std::vector<NodeWrapper*> reachable = { new NodeWrapper(&w_start.highway->node()) };
    //
    // // FIXME: can be rewritten to the Graph class
    std::map<Position, NodeWrapper*> explored;
    std::vector<NodeWrapper*> path;

    NodeWrapper* temp = nullptr;

    auto const& get_adjasted_nodes = [&](NodeWrapper* node_wrapper) {
        std::vector<NodeWrapper*> new_reachable;
        // adjacent nodes
        for (auto a_node : node_wrapper->node().children()) {
            if (!explored[a_node->position()]) {
                // FIXME: добавить previous в node_copy | это нужно для построения пути
                NodeWrapper* node_copy = new NodeWrapper(a_node);

                explored[a_node->position()]
                    = new NodeWrapper(&node_wrapper->node());

                new_reachable.push_back(node_copy);
            }
        }
        return new_reachable;
    };

    auto find_in_reachable = [&](NodeWrapper* node) {
        return std::find(reachable.begin(), reachable.end(), node);
    };

    auto remove_from_reachable = [&](NodeWrapper* node) {
        auto it = find_in_reachable(node);
        if (it != reachable.end()) {
            reachable.erase(it);
        }
    };

    auto build_path = [&](NodeWrapper* to_node) {
        std::vector<NodeWrapper*> _path;
        while (to_node) {
            _path.push_back(to_node);
            to_node = &to_node->previous_node();
        }
        return _path;
    };

    while (!reachable.empty()) {
        temp = Node::choose_nearest_node(reachable, w_end.highway);

        if (temp->node() == w_end.highway->node()) {
            std::cout << "end_point found!!!\n";
            path = build_path(temp);
            break;
        }

        remove_from_reachable(temp);
        NodeWrapper* temp_copy = new NodeWrapper(&temp->node());
        explored[temp_copy->node().position()] = temp_copy;

        auto const& new_reachable = get_adjasted_nodes(temp);

        for (auto nr : new_reachable) {
            auto it = find_in_reachable(nr);
            if (it == reachable.end()) {
                nr->set_previous_node(temp);
                reachable.push_back(nr);
            }
        }
    }

    for (auto p : path) {
        std::cout << "NODE: " << p->node().position().lat() << " - " << p->node().position().lng() << "\n";
    }

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
        auto type = properties->operator[]("HIGHWAY");
        if (type != "footway" && type != "step") {

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
