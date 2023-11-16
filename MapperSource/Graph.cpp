#include "Graph.hpp"
#include "Node.hpp"
#include "Types/Geojson.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <utility>

static std::pair<double, double> round_coordinate(Position& position)
{
    // position.lat() = std::round(position.lat() / 1e-5) * 1e-5;
    // position.lng() = std::round(position.lng() / 1e-5) * 1e-5;
    return std::make_pair(std::round(position.lat() / 1e-5) * 1e-5, std::round(position.lng() / 1e-5) * 1e-5);
}

void Graph::append_nodes(std::vector<Position*>& positions)
{

    // FIXME: round_coordinate(position) -> std::pair<double, double>
    // round_coordinate(position);
    if (positions.size() == 0)
        return;

    Node* last_node = nullptr;
    Node* next_node = nullptr;

    for (size_t i = 0; i < positions.size(); i++) {
        if (auto& position = m_nodes[round_coordinate(*positions[i])]) {
            // FIXME: проверить массив children
            if (i == 0) {
                last_node = position;
                next_node = new Node(positions[i + 1]);
                position->append_child(next_node);
            } else if (i == positions.size() - 1) {
                position->append_child(last_node);
            } else {
                position->append_child(last_node);
                next_node = new Node(positions[i + 1]);
                position->append_child(next_node);
            }
        } else {
            if (i == 0) {
                last_node = new Node(positions[i]);
                next_node = new Node(positions[i + 1]);
                position = last_node;
                position->append_child(next_node);
            } else if (i == positions.size() - 1) {
                // FIXME: add this condition
                position = new Node(positions[i]);
            } else {
                position = next_node;
                position->append_child(last_node);
                last_node = position;
                next_node = new Node(positions[i]);
                position->append_child(next_node);
            }
        }
    }
}
