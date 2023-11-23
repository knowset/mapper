#pragma once

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

#include "Position.hpp"

using Properties = std::map<std::string, std::string>;

class Node;

class NodeWrapper {
public:
    NodeWrapper() = delete;
    NodeWrapper(Node* node)
        : m_node(node)
    {
    }
    Node& node(void) { return *m_node; }
    NodeWrapper& previous_node(void) { return *m_previous_node; }
    void set_previous_node(NodeWrapper* node_wrapper)
    {
        m_previous_node = node_wrapper;
    }

private:
    Node* m_node = nullptr;
    NodeWrapper* m_previous_node = nullptr;
};

//  Node
//      Pos
//      Children -> Node* []
//      Properties
class Node {

public:
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

            // std::cout << node->position().lat() << " - " << node->position().lng() << " # " << distance_node_to_goal << "\n";

            if (min_distance > distance_node_to_goal) {
                min_distance = distance_node_to_goal;
                nearest_node = node;
            }
        }

        return nearest_node;
    }
    static NodeWrapper* choose_nearest_node(std::vector<NodeWrapper*>& reachable, NodeWrapper* end)
    {
        double min_distance = std::numeric_limits<double>::max();
        NodeWrapper* nearest_node;

        // https://habr.com/ru/articles/444828/
        for (NodeWrapper* node_wrapper : reachable) {
            // cost_start_to_node = node.cost
            // cost_node_to_goal = estimate_distance(node, goal_node)
            // total_cost = cost_start_to_node + cost_node_to_goal
            double distance_node_to_goal = Position::distance(node_wrapper->node().position(), end->node().position());
            // std::cout << node->position().lat() << " - " << node->position().lng() << " # " << distance_node_to_goal << "\n";

            if (min_distance > distance_node_to_goal) {
                min_distance = distance_node_to_goal;
                nearest_node = node_wrapper;
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
    Node(Node const& node)
        : m_pos(node.m_pos)
        , m_children(node.m_children)
        , m_properties(node.m_properties)
    {
    }
    ~Node()
    {
        std::cout << "NODE DESTR\n";
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

    bool operator==(Node& other)
    {
        if (*this->m_pos == *other.m_pos) {
            return true;
        }
        return false;
    }

private:
    Position* m_pos;
    std::vector<Node*> m_children;
    std::map<std::string, std::string>* m_properties;
};

// FIXME: come up with a more appropriate name for this class
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
