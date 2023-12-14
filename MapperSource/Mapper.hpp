#pragma once

#include <napi.h>

#include "Graph.hpp"

class Mapper : public Napi::ObjectWrap<Mapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Mapper(Napi::CallbackInfo const& info);
    static Napi::Value CreateNewItem(Napi::CallbackInfo const& info);

private:
    void AddGeoJson(Napi::CallbackInfo const& info);
    Napi::Value FindRoutes(Napi::CallbackInfo const& info);

    void parse_geojson_napi_object(Napi::Object const&, Napi::Env const&);

public:
    static std::vector<Node*> extract_linestring(Napi::Value const&, Properties*, Napi::Env const&);
    Properties* extract_properties(Napi::Value const&);

    Graph m_graph;
};
