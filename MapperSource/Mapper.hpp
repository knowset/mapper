#pragma once

#include <napi.h>

#include "Graph.hpp"

class Mapper : public Napi::ObjectWrap<Mapper> {
  public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	Mapper(const Napi::CallbackInfo &info);
	// NOTE: mb add CreateNewItem method
	static Napi::Value CreateNewItem(const Napi::CallbackInfo &info);

  private:
	Napi::Value FindRoutes(const Napi::CallbackInfo &info);

	void parse_geojson_napi_object(const Napi::Object &);

    std::vector<Position*> extract_linestring(const Napi::Value &);

	// FeatureCollection m_fc;
	Graph m_graph;
};
