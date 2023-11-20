#pragma once

#include <map>
#include <string>
#include <vector>

#include "Node.hpp"

class Graph {
public:
    std::map<std::pair<double, double>, Node*>& nodes(void)
    {
        return m_nodes;
    }

    void append_nodes(std::vector<Node*>& nodes);

private:
    std::map<std::pair<double, double>, Node*> m_nodes;
};
