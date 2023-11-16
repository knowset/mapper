#pragma once

#include <iostream>
#include <vector>

#include "Types/Geojson.hpp"

class Node {
public:
    Node() = delete;
    Node(Position* position)
        : m_pos(position)
    {
    }

    Position& position(void) { return *m_pos; }
    std::vector<Node*>& children(void) { return m_children; }
    void append_child(Node* child) { m_children.push_back(child); }

private:
    Position* m_pos;
    std::vector<Node*> m_children;
};

//  Node
//      Pos
//      Children -> Node* []
//      Options
