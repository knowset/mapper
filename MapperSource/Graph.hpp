#pragma once

#include <map>
#include <string>
#include <vector>

#include "Node.hpp"
#include "Position.hpp"

class Graph {
public:
    std::map<Position, Node*>& nodes(void)
    {
        return m_nodes;
    }

    void append_nodes(std::vector<Node*>& nodes);
/*     void append_node(Node* node); */

private:
    std::map<Position, Node*> m_nodes;
};
