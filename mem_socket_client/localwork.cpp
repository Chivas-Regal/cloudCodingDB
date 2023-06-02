#include "connector.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

int main (int argv, char* argc[]) {
    Connector *conn = new Connector();
    conn->Login("test1", "test1");

    std::cout << "#############################################" << std::endl;
    std::cout << "#    Welcome to cloudcodingDB(localwork)    #" << std::endl;
    std::cout << "#############################################" << std::endl;

    std::vector<std::string> new_var;
    while (true) {
        bool run_flag;
        std::cout << "crdb> ";

        std::string ope;
        std::getline(std::cin, ope);

        std::stringstream ss; ss << ope;
        std::vector<std::string> words;
        std::string word;
        while (ss >> word) 
            words.push_back(word);

        if (words[0] == "new") {


            if (words.size() == 2) {
                run_flag = conn->memAsk<int>(words[1]);
            } else {
                run_flag = conn->memAsk<int>(words[1], std::stoi(words[2]));
            }
            new_var.push_back(words[1]);
            if (run_flag) 
                std::cout << "# OK: create new variable " << words[1] << std::endl;
            else
                std::cout << "# Error: create new variable " << words[1] << std::endl;


        } else if (words[0] == "set") {


            if (words[1].back() != ']') {
                run_flag = conn->memWrite<int>(words[1], std::stoi(words[3]));
            } else {
                std::string var = words[1].substr(0, words[1].find('['));
                std::string idx = words[1].substr(words[1].find('[') + 1);
                idx.pop_back();
                run_flag = conn->memWrite<int>(var, std::stoi(words[3]), std::stoi(idx));
            }
            if (run_flag) 
                std::cout << "# OK: make variable ";
            else
                std::cout << "# Error: make variable ";
            for (int i = 1; i < words.size(); i ++)
                std::cout << words[i];
            std::cout << std::endl;


        } else if (words[0] == "get") {


            if (words[1].back() != ']') {
                std::cout << conn->memRead<int>(words[1]) << std::endl;
            } else {
                std::string var = words[1].substr(0, words[1].find('['));
                std::string idx = words[1].substr(words[1].find('[') + 1);
                idx.pop_back();
                std::cout << conn->memRead<int>(var, std::stoi(idx)) << std::endl;
            }


        } else if (words[0] == "exit") {


            for (auto s : new_var)
                conn->memFree(s);
            conn->Exit();
            std::cout << "# bye~";
            break;


        } else {


            std::cout << "Error syntax!\n";


        }
        std::cout << std::endl;
    }
}