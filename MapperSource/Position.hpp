#pragma once

#include <memory>

// radius of the earth in meters
#define EARTHRADIUS 6'371'000

class Position {

public:
    static double radians_to_length(double);
    static double degrees_to_coordinate(double);
    static double distance(Position&, Position&);

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
