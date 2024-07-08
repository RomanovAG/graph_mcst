#include <functional>
#include "graph.hpp"
#include <iostream>
#include <sstream>

Graph::Graph()
{}

Graph::~Graph()
{}

int Graph::add_node(const std::string &node)
{
    if (this->nodes.contains(node))
    {
        return -1;
    } 

    this->nodes[node] = std::unordered_map<std::string, int>();
    return 0;
}

int Graph::add_edge(const std::string &from, const std::string &to, int weight)
{
    // 2 bits: left - from (node), right - to (node). bit is set if node is not exist
    // if both bits == 1 (flag == 3) then both nodes doesn't exist
    int node_missing_flag = 0;
    if (!this->nodes.contains(from))
    {
        node_missing_flag |= 0b10;
    }
    if (!this->nodes.contains(to))
    {
        node_missing_flag |= 0b01;
    }
    if (node_missing_flag)
    {
        // there are 3 possible outcomes so 2 bits required to create a flag
        return node_missing_flag;
    }

    if (this->nodes.at(from).contains(to))
    {
        return -1;
    }

    if (from == to)
    {
        return -2;
    }

    this->nodes.at(from).insert({to, weight});
    return 0;
}

int Graph::remove_node(const std::string &node)
{
    if (!this->nodes.contains(node)) 
    {
        return -1;
    }

    this->nodes.erase(node);
    for (auto & pair : this->nodes)
    {
        pair.second.erase(node);
    }
    return 0;
}

int Graph::remove_edge(const std::string &from, const std::string &to)
{
    int node_missing_flag = 0;
    if (!this->nodes.contains(from))
    {
        node_missing_flag |= 0b10;
    }
    if (!this->nodes.contains(to))
    {
        node_missing_flag |= 0b01;
    }
    if (node_missing_flag)
    {
        // same logic as in add_edge()
        return node_missing_flag;
    }

    if (!this->nodes.at(from).contains(to))
    {
        return -1;
    }

    this->nodes.at(from).erase(to);
    return 0;
}

int Graph::set_root(const std::string &node)
{
    if (!this->nodes.contains(node)) 
    {
        return -1;
    }

    this->root = node;
    return 0;
}

std::pair<std::vector<std::string>, std::vector<std::pair<std::string, std::string>>> 
Graph::post_order(const std::string &root_node) const
{
    // if bool == true, node is open, otherwise it's closed but still visited since it's in the map
    std::unordered_map<std::string, bool> visited;
    std::vector<std::string> order;
    std::vector<std::pair<std::string, std::string>> back_edges;

    std::function<void(const std::string &)> 
    dfs = [&](const std::string &node) -> void
    {
        visited.insert({node, true});

        for (const auto &adj_node : this->nodes.at(node))
        {
            if (!visited.contains(adj_node.first))
            {
                dfs(adj_node.first);
            }
            else if (visited.at(adj_node.first) == true)
            {
                // if open node encountered then it's back edge
                back_edges.push_back({node, adj_node.first});
            }
        }

        order.push_back(node);
        visited.at(node) = false; // node is closed now
    };

    dfs(root_node);

    return {order, back_edges};
}

void Graph::print_RPO() const
{
    if (this->root.empty())
    {
        throw std::runtime_error("Root is not set");
    }

    const auto pair = this->post_order(this->root);
    const auto &order = pair.first;
    const auto &back_edges = pair.second;
    
    // iterate in reverse order since it's RPO
    for (auto it = order.rbegin(); it != order.rend(); it++)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    for (const auto &edge : back_edges)
    {
        std::cout << "Found back edge: " << edge.first << "->" << edge.second << std::endl;
    }
}

std::vector<std::string> Graph::critical_path(const std::string &from, const std::string &to) const
{
    if (!this->nodes.contains(from) || !this->nodes.contains(to))
    {
        throw std::runtime_error("Wrong start or end node");
    }

    auto pair = this->post_order(from); // returns reversed topological sorted nodes if no cycles found
    if (!pair.second.empty())
    {
        throw std::runtime_error("Cycles detected");
    }
    
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> prev;

    for (const auto &node : this->nodes)
    {
        dist.insert({node.first, -1});
    }
    dist.at(from) = 0;

    for (auto it = pair.first.rbegin(); it != pair.first.rend(); it++)
    {
        const auto &u = *it;
        if (dist.at(u) > -1)
        {
            for (const auto &adj_node : this->nodes.at(u)) 
            {
                if (dist.at(adj_node.first) < dist.at(u) + adj_node.second) 
                {
                    dist.at(adj_node.first) = dist.at(u) + adj_node.second;
                    prev[adj_node.first] = u;
                }
            }
        }
    }

    if (dist.at(to) < 0)
    {
        throw std::runtime_error("Node " + to + " is not reachable from node " + from);
    }
    
    // extracting path
    std::vector<std::string> path;
    for (std::string node = to; node != from; node = prev.at(node)) 
    {
        path.push_back(node);
    }
    path.push_back(from);
    std::reverse(path.begin(), path.end());

    return path;
}

void Graph::print_critical_path(const std::string &from, const std::string &to) const
{
    auto path = this->critical_path(from, to);
    for (size_t i = 0; i < path.size() - 1; i++)
    {
        std::cout << path[i] << "->";
    }
    std::cout << path.back() << std::endl;
}

void Graph::exec_cmd(const std::string &command)
{
    // since methods that modify graph don't write anything to the stream, 
    // lambda functions take care of that by decoding return value

    // command map <"cmd", lambda function>
    std::unordered_map<std::string, std::function<void(std::istringstream &)>>
    cmd_map = 
    {
        {
            "ADD_NODE", [this](std::istringstream &iss) -> void
            {
                std::string node;
                iss >> node;

                if (node.empty())
                {
                    std::cout << "Can't add node with empty name" << std::endl;
                    return;
                }
                if (this->add_node(node))
                {
                    std::cout << "Node \"" << node << "\" is already exist" << std::endl;
                }
            }
        },
        {
            "ADD_EDGE", [this](std::istringstream &iss) -> void
            {
                std::string from, to;
                int weight = 1;
                iss >> from >> to >> weight;

                if (weight < 1)
                {
                    std::cout << "Edge weight must be greater than 0" << std::endl;
                    return;
                }

                int flag = this->add_edge(from, to, weight);
                if (flag < 0)
                {
                    std::cout << "Edge between nodes \"" << from << "\" and \"" << to << "\" is already exist" 
                    << std::endl;
                    return;
                }
                if (flag & 2)
                {
                    std::cout << "Node \"" << from << "\" is not exist" << std::endl;
                }
                if (flag & 1)
                {
                    std::cout << "Node \"" << to << "\" is not exist" << std::endl;
                }
            }
        },
        {
            "REMOVE_NODE", [this](std::istringstream &iss) -> void
            {
                std::string node;
                iss >> node;

                if (this->remove_node(node))
                {
                    std::cout << "Node \"" << node << "\" is not exist" << std::endl;
                }
            }
        },
        {
            "REMOVE_EDGE", [this](std::istringstream &iss) -> void
            {
                std::string from, to;
                iss >> from >> to;

                int flag = this->remove_edge(from, to);
                if (flag < 0)
                {
                    std::cout << "Edge between nodes \"" << from << "\" and \"" << to << "\" is not exist" 
                    << std::endl;
                    return;
                }
                if (flag & 2)
                {
                    std::cout << "Node \"" << from << "\" is not exist" << std::endl;
                }
                if (flag & 1)
                {
                    std::cout << "Node \"" << to << "\" is not exist" << std::endl;
                }
            }
        },
        {
            "ROOT", [this](std::istringstream &iss) -> void
            {
                std::string node;
                iss >> node;

                if (this->set_root(node))
                {
                    std::cout << "Node \"" << node << "\" is not exist" << std::endl;
                }
            }
        },
        {
            "PRINT_RPO", [this](std::istringstream &iss) -> void
            {
                iss.clear();

                try
                {
                    this->print_RPO();
                }
                catch (std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        },
        {
            "FIND_CRITICAL_PATH", [this](std::istringstream &iss) -> void
            {
                std::string from, to;
                iss >> from >> to;

                try
                {
                    this->print_critical_path(from, to);
                }
                catch (std::runtime_error &e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }
    };

    std::istringstream iss(command);
    std::string op;
    iss >> op;

    auto it = cmd_map.find(op);
    if (it == cmd_map.end())
    {
        std::cout << "Unknown command: " << command << std::endl;
        return;
    }
    
    it->second(iss);
}