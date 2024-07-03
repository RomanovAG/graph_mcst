#include "graph.hpp" 
#include <iostream>

int main()
{
    Graph graph;

    std::string command;
    while (std::getline(std::cin, command))
    {
        if (command == "END")
        {
            break;
        }

        graph.exec_cmd(command);
    }
    
    return 0;
}