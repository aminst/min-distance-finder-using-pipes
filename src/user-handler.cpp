#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "utils.hpp"

using namespace std;


int get_file_number(string file_name)
{
    string formatted_file_name = parse_line(file_name, "/").back();
    formatted_file_name.erase(formatted_file_name.find(".csv"), formatted_file_name.length());
    return stoi(parse_line(formatted_file_name, "-").back());
}

int main(int argc, char *argv[])
{
    vector<string> named_pipes;
    mkdir("pipes2", 0777);

    vector<string> parsed_argv = parse_line(argv[1], "#");
    string trait_line = parsed_argv[0];
    string named_pipe = parsed_argv[1];
    string users_dir = parsed_argv[2];

    if (auto dir = opendir(&users_dir[0])) {
        while (auto f = readdir(dir)) {
            if (!f->d_name || f->d_name[0] == '.')
                continue;
            
            string file_path = users_dir + "/" + f->d_name;
            int unnamed_piped_fd[2];
            if (pipe(unnamed_piped_fd) == -1)
                exit(EXIT_FAILURE);

            int pid = fork();
            if (pid == -1)
                exit(EXIT_FAILURE);
            else if (pid > 0)
            {
                const char *named_pipe = ("pipes2/" + to_string(pid)).c_str(); 
                string named_pipe_str(named_pipe);
                named_pipes.push_back(named_pipe_str);
                mkfifo(named_pipe, 0666);

                char buff[1024] = {0};
                string message = trait_line + "#" + file_path + "#" + named_pipe_str;
                strcpy(buff, message.c_str());
                write(unnamed_piped_fd[1], buff, 1024);
            }
            else if (pid == 0)
            {
                char buff[1024] = {0};
                read(unnamed_piped_fd[0], buff, 1024);
                string got_message(buff);
                if (execlp("./min-distance-handler.out", "min-distance-handler.cpp", got_message.c_str(), NULL) == -1)
                {
                    cout << "Exec Failed" << endl;
                    exit(EXIT_FAILURE);
                }                
            }
        }
        closedir(dir);
    }

    vector<double> mins;
    vector<string> min_file_names;
    vector<string> min_lines;
    for (size_t i = 0; i < named_pipes.size(); i++)
    {
        char buff[1024];
        int fd = open(named_pipes[i].c_str(), O_RDONLY);
        read(fd, buff, 1024);
        unlink(named_pipes[i].c_str());
        
        vector<string> parsed = parse_line(buff, "#");
        string file_name = parsed[0];
        string min_line = parsed[2];
    
        mins.push_back(stod(parsed[1]));
        min_file_names.push_back(file_name);
        min_lines.push_back(min_line);
        close(fd);
    }
    
    int min_index = 0;
    for (size_t i = 1; i < mins.size(); i++)
    {
        if (mins[i] < mins[min_index])
        {
            min_index = i;
        }
        else if (mins[i] == mins[min_index])
        {
            int current_file_number = get_file_number(min_file_names[i]);
            int min_file_number = get_file_number(min_file_names[min_index]);
            if (current_file_number < min_file_number)
            {
                min_index = i;
            }
        }
    }

    int write_fifo = open(&named_pipe[0], O_WRONLY);
    char buff[1024] = {0};
    string message = min_file_names[min_index] + "#" + min_lines[min_index];
    strcpy(buff, message.c_str());
    write(write_fifo, buff, 1024);
    close(write_fifo);

    return 0;
}