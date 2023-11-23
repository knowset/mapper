#include <cmath>

#include "Position.hpp"

std::pair<double, double> Position::round_coordinate(Position& position)
{
    return std::make_pair(std::round(position.lat() / 1e-5) * 1e-5, std::round(position.lng() / 1e-5) * 1e-5);
}

std::pair<double, double> Position::round_coordinate(double lat, double lng)
{
    return std::make_pair(std::round(lat / 1e-5) * 1e-5, std::round(lng / 1e-5) * 1e-5);
}

double Position::radians_to_length(double radians)
{
    return radians * EARTHRADIUS;
}

double Position::degrees_to_coordinate(double value)
{
    return value * (M_PI / 180);
}

// https://github.com/Turfjs/turf/blob/master/packages/turf-distance/index.ts#L30
double Position::distance(Position& from, Position& to)
{
    double dLat = degrees_to_coordinate(to.lat() - from.lat());
    double dLng = degrees_to_coordinate(to.lng() - from.lng());
    double lat1 = degrees_to_coordinate(from.lat());
    double lat2 = degrees_to_coordinate(to.lat());

    double a = std::pow(std::sin(dLat / 2), 2) + std::pow(std::sin(dLng / 2), 2) * std::cos(lat1) * std::cos(lat2);

    return radians_to_length(2 * std::atan2(std::sqrt(a), std::sqrt(1 - a)));
}
