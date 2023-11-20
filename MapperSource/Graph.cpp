#include "Graph.hpp"
#include "Node.hpp"
#include "Position.hpp"

#include <cmath>
#include <functional>
#include <iostream>
#include <utility>

void Graph::append_nodes(std::vector<Node*>& positions)
{

    // FIXME: round_coordinate(position) -> std::pair<double, double>
    // round_coordinate(position);
    if (positions.size() == 0)
        return;

    Node* last_node = nullptr;
    Node* next_node = nullptr;

    for (size_t i = 0; i < positions.size(); i++) {
        if (auto& position = m_nodes[Node::round_coordinate(positions[i]->position())]) {
            // FIXME: проверить массив children
            if (i == 0) {
                last_node = position;
                next_node = positions[i + 1];
                position->append_child(next_node);
            } else if (i == positions.size() - 1) {
                position->append_child(last_node);
            } else {
                position->append_child(last_node);
                next_node = positions[i + 1];
                position->append_child(next_node);
            }
        } else {
            if (i == 0) {
                last_node = positions[i];
                next_node = positions[i + 1];
                position = last_node;
                position->append_child(next_node);
            } else if (i == positions.size() - 1) {
                // FIXME: add this condition
                position = positions[i];
            } else {
                position = next_node;
                position->append_child(last_node);
                last_node = position;
                next_node = positions[i];
                position->append_child(next_node);
            }
        }
    }
}
