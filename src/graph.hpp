#ifndef GRAPH
#define GRAPH

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Graph
{
private:
    std::string root;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> nodes;

    // returns nodes in post order and found back edges as pair of nodes
    std::pair<std::vector<std::string>, std::vector<std::pair<std::string, std::string>>> 
    post_order(const std::string &root_node) const;

    std::vector<std::string> 
    critical_path(const std::string &from, const std::string &to) const;

public:
    Graph();
    ~Graph();

    int add_node(const std::string &node);
    int add_edge(const std::string &from, const std::string &to, int weight = 1);

    int remove_node(const std::string &node);
    int remove_edge(const std::string &from, const std::string &to);

    int set_root(const std::string &node);

    void print_RPO() const;
    void print_critical_path(const std::string &from, const std::string &to) const;

    void exec_cmd(const std::string &command);
};

#endif // GRAPH