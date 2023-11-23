#pragma once

#include <napi.h>
#include <future>

#include "Graph.hpp"

class Mapper : public Napi::ObjectWrap<Mapper> {
public:

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Mapper(Napi::CallbackInfo const& info);
    // NOTE: mb add CreateNewItem method
    static Napi::Value CreateNewItem(Napi::CallbackInfo const& info);

private:
    Napi::Value FindRoutes(Napi::CallbackInfo const& info);

    void parse_geojson_napi_object(Napi::Object const&, const Napi::Env&);

public: 
    static std::vector<Node*> extract_linestring(Napi::Value const&, Properties*, const Napi::Env&);
    Properties* extract_properties(Napi::Value const&);

    std::vector<std::future<void>> m_futures;

    Graph m_graph;
};
