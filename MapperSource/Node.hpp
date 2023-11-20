#pragma once

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

#include "Position.hpp"

using Properties = std::map<std::string, std::string>;

//  Node
//      Pos
//      Children -> Node* []
//      Properties
class Node {
public:
    static std::pair<double, double> round_coordinate(Position& position)
    {
        // position.lat() = std::round(position.lat() / 1e-5) * 1e-5;
        // position.lng() = std::round(position.lng() / 1e-5) * 1e-5;
        return std::make_pair(std::round(position.lat() / 1e-5) * 1e-5, std::round(position.lng() / 1e-5) * 1e-5);
    }
    static Node* choose_nearest_node(std::vector<Node*>& reachable, Node* end)
    {
        double min_distance = std::numeric_limits<double>::max();
        Node* nearest_node;

        // https://habr.com/ru/articles/444828/
        for (auto node : reachable) {
            // cost_start_to_node = node.cost
            // cost_node_to_goal = estimate_distance(node, goal_node)
            // total_cost = cost_start_to_node + cost_node_to_goal
            double distance_node_to_goal = Position::distance(node->position(), end->position());

            std::cout << node->position().lat() << " - " << node->position().lng() << " # " << distance_node_to_goal << "\n";

            if (min_distance > distance_node_to_goal) {
                min_distance = distance_node_to_goal;
                nearest_node = node;
            }
        }

        return nearest_node;
    }

public:
    Node() = delete;
    Node(Position* position)
        : m_pos(position)
    {
    }
    Node(Position* position, Properties* options)
        : m_pos(position)
        , m_properties(options)
    {
    }
    ~Node()
    {
        delete m_pos;
        delete m_properties;
    }

    Position& position(void)
    {
        return *m_pos;
    }
    std::vector<Node*>& children(void) { return m_children; }
    std::map<std::string, std::string>& options(void)
    {
        return *m_properties;
    }

    void append_child(Node* child) { m_children.push_back(child); }

private:
    Position* m_pos;
    std::vector<Node*> m_children;
    std::map<std::string, std::string>* m_properties;
};
// {
//             "type": "Feature",
//             "geometry": {
//                 "type": "LineString",
//                 "coordinates": [
//                     [36.603329, 61.3755799],
//                     ...
//                  ]
//             },
//             "properties": {
//                 "OSM_TYPE": "way",
//                  ...
//             }
//         },
//
