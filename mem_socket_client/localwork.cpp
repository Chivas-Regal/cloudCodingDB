#include "connector.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

int main (int argv, char* argc[]) {
    Connector *conn = new Connector();
    conn->Login(argc[1], argc[2]);

    std::cout << "\n\n";
    std::cout << "#############################################" << std::endl;
    std::cout << "#    Welcome to cloudcodingDB(localwork)    #" << std::endl;
    std::cout << "#############################################" << std::endl;
    std::cout << "\n\n";

    std::vector<std::string> new_var;
    while (true) {
        bool run_flag;
        std::cout << "client> ";

        std::string ope;
        std::getline(std::cin, ope);

        std::stringstream ss; ss << ope;
        std::vector<std::string> words;
        std::string word;
        while (ss >> word) 
            words.push_back(word);

        if (words[0] == "NEW") {


            if (words.size() == 2) {
                run_flag = conn->memAsk<int>(words[1]);
            } else {
                run_flag = conn->memAsk<int>(words[1], std::stoi(words[2]));
            }
            new_var.push_back(words[1]);
            if (run_flag) 
                std::cout << "server> OK";
            else
                std::cout << "server> ERROR";
            std::cout << std::endl;


        } else if (words[0] == "SET") {


            if (words[1].back() != ']') {
                run_flag = conn->memWrite<int>(words[1], std::stoi(words[3]));
            } else {
                std::string var = words[1].substr(0, words[1].find('['));
                std::string idx = words[1].substr(words[1].find('[') + 1);
                idx.pop_back();
                run_flag = conn->memWrite<int>(var, std::stoi(words[3]), std::stoi(idx));
            }
            if (run_flag) 
                std::cout << "server> OK";
            else
                std::cout << "server> ERROR";
            std::cout << std::endl;


        } else if (words[0] == "GET") {


            if (words[1].back() != ']') {
                std::cout << conn->memRead<int>(words[1]) << std::endl;
            } else {
                std::string var = words[1].substr(0, words[1].find('['));
                std::string idx = words[1].substr(words[1].find('[') + 1);
                idx.pop_back();
                std::cout << "server> " << conn->memRead<int>(var, std::stoi(idx)) << std::endl;
            }


        } else if (words[0] == "EXIT") {


            for (auto s : new_var)
                conn->memFree(s);
            conn->Exit();
            std::cout << "server> bye~\n";
            break;


        } else {


            std::cout << "server> Error syntax!\n";


        }
        std::cout << std::endl;
    }
}