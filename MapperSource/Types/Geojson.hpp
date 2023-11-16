#pragma once

#include <string>
#include <variant>
#include <vector>

//  1.  Points
//  2.  LineString
//  3.  Polygons
//  4.  MultiPoints
//  5.  MultiLineStrings
//  6.  MultiPolygons
//  7.  GeometryCollections
enum GeoJsonObjectsTypes {
    POINT,
    LINESTRING,
    POLYGON,
    MUTLIPOINT,
    MUTLILINESTRING,
    MULTIPOLYGON,
    GEOMETRYCOLLECTION,
    FEATURE,
    FEATURECOLLECTION,
    UNDEFINED
};

class Position {

private:
    double m_lat;
    double m_lng;

public:
    Position(double lat, double lng)
        : m_lat(lat)
        , m_lng(lng)
    {
    }
    Position& operator=(Position&& pos)
    {
        m_lat = std::move(pos.m_lat);
        m_lng = std::move(pos.m_lng);

        return *this;
    }
    Position(Position&) = delete;

    double& lat(void) { return m_lat; }
    double& lng(void) { return m_lng; }
};

struct GeoJsonObject {
    std::string type;
};
// Proreties <<<<
// struct Point {
//     Point() {}
// 	GeoJsonObjectsTypes type = POINT;
// 	Position coordinates;
// };

// struct LineString {
// 	LineString() {}
// 	LineString(size_t coordinates_size) {
// 		m_coordinates = std::vector<Position>(coordinates_size);
// 	}
// 	LineString(std::initializer_list<Position> l) : m_coordinates(l) {
// 		m_coordinates.insert(m_coordinates.end(), l.begin(), l.end());
// 	}
// 	GeoJsonObjectsTypes type = LINESTRING;
// 	std::vector<Position*> m_coordinates;
// };

// struct MultiPoint {
//     MultiPoint() {}
// 	GeoJsonObjectsTypes type = MUTLIPOINT;
// 	std::vector<Position> coordinates;
// };

// struct MultiLineString {
// 	GeoJsonObjectsTypes type = MUTLILINESTRING;
// 	std::vector<std::vector<Position>> coordinates;
// };

// using Geometry = std::variant<Point, MultiPoint, LineString>;

// using Geometry = std::variant<LineString>;
//
// struct Feature {
// 	Feature(Geometry *geometry) : m_geometry(geometry){};
//
// 	GeoJsonObjectsTypes type = FEATURE;
// 	Geometry *m_geometry = nullptr;
// };
//
// struct FeatureCollection {
// 	GeoJsonObjectsTypes type = FEATURECOLLECTION;
// 	std::vector<Feature> features;
// };

inline std::string
geojson_objects_types_to_string(GeoJsonObjectsTypes geo_json_object_type)
{
    switch (geo_json_object_type) {
    case POINT:
        return "Point";
    case LINESTRING:
        return "LineString";
    case POLYGON:
        return "Polygon";
    case MUTLIPOINT:
        return "MultiPoint";
    case MUTLILINESTRING:
        return "MultiLineString";
    case GEOMETRYCOLLECTION:
        return "GeometryCollection";
    case FEATURE:
        return "Feature";
    case FEATURECOLLECTION:
        return "FeatureCollection";
    default:
        break;
    }
    return "";
}

inline GeoJsonObjectsTypes
string_to_geojson_objects_types(std::string geo_json_object_type)
{
    if (geo_json_object_type == "Point")
        return GeoJsonObjectsTypes::POINT;
    else if (geo_json_object_type == "LineString")
        return GeoJsonObjectsTypes::LINESTRING;
    else if (geo_json_object_type == "Polygon")
        return GeoJsonObjectsTypes::POLYGON;
    else if (geo_json_object_type == "MultiPoint")
        return GeoJsonObjectsTypes::MUTLIPOINT;
    else if (geo_json_object_type == "MultiLineString")
        return GeoJsonObjectsTypes::MUTLILINESTRING;
    else if (geo_json_object_type == "MultiPolygon")
        return GeoJsonObjectsTypes::MULTIPOLYGON;
    return GeoJsonObjectsTypes::UNDEFINED;
}
