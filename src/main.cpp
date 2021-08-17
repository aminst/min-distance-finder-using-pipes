#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    string traits_file_path = argv[1];
    string users_dir_path = argv[2];

    vector<string> named_pipes;
    mkdir("pipes", 0777);

    ifstream traits_file(argv[1]);
    string traits_line;
    
    while(getline(traits_file, traits_line))
    {
        int unnamed_piped_fd[2];
        if (pipe(unnamed_piped_fd) == -1)
            exit(EXIT_FAILURE);
        
        int pid = fork();
        if (pid == -1)
            exit(EXIT_FAILURE);
        else if (pid > 0)
        {
            const char *named_pipe = ("pipes/" + to_string(pid)).c_str(); 
            string named_pipe_str(named_pipe);
            named_pipes.push_back(named_pipe_str);
            mkfifo(named_pipe, 0666); 

            char buff[1024] = {0};
            string message = traits_line + "#" + named_pipe_str + "#" + users_dir_path;
            strcpy(buff, message.c_str());
            write(unnamed_piped_fd[1], buff, 1024);
        }
        else if (pid == 0)
        {
            char buff[1024] = {0};
            read(unnamed_piped_fd[0], buff, 1024);
            string got_message(buff);
            if (execlp("./user-handler.out", "user-handler.cpp", got_message.c_str(), NULL) == -1)
            {
                cout << "Exec Failed" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    ofstream results_file;
    results_file.open("results.csv");
    for (size_t i = 0; i < named_pipes.size(); i++)
    {
        char buff[1024];
        int fd = open(named_pipes[i].c_str(), O_RDONLY);
        read(fd, buff, 1024);
        unlink(named_pipes[i].c_str());
        
        vector<string> parsed = parse_line(buff, "#");
        string file_name = parsed[0];
        string line = parsed[1];
        string formatted_file_name = parse_line(file_name, "/").back();
        formatted_file_name.erase(formatted_file_name.find(".csv"), formatted_file_name.length());
        results_file << formatted_file_name << "," << line << endl;
        
        close(fd);
    }
    results_file.close();

    return 0;
}