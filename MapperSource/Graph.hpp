#pragma once

#include <string>
#include <map>
#include <vector>

#include "Node.hpp"
#include "Types/Geojson.hpp"

class Graph {
public:
    std::map<std::pair<double, double>, Node*>& nodes(void) {
        return m_nodes;
    }

    void append_nodes(std::vector<Position*>& position);

private:
    std::map<std::pair<double, double>, Node*> m_nodes;
};
