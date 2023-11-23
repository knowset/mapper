#pragma once

#include <memory>

// radius of the earth in meters
#define EARTHRADIUS 6'371'000

class Position {

public:
    static std::pair<double, double> round_coordinate(Position&);
    static std::pair<double, double> round_coordinate(double, double);
    static double radians_to_length(double);
    static double degrees_to_coordinate(double);
    static double distance(Position&, Position&);

private:
    double m_lat;
    double m_lng;

public:
    Position(double lat, double lng)
    {
        auto pos = round_coordinate(lat, lng);
        m_lat = pos.first;
        m_lng = pos.second;
    }
    Position(Position const& position)
    {
        m_lat = position.m_lat;
        m_lng = position.m_lng;
    }
    Position& operator=(Position&& pos)
    {
        m_lat = std::move(pos.m_lat);
        m_lng = std::move(pos.m_lng);

        return *this;
    }
    bool operator==(Position const& other)
    {
        if (m_lat == other.m_lat && m_lng == other.m_lng)
            return true;
        return false;
    }
    bool operator<(Position const& rhs) const
    {
        return m_lat < rhs.m_lat && m_lng < rhs.m_lng;
    }

    double& lat(void) { return m_lat; }
    double& lng(void) { return m_lng; }
};
