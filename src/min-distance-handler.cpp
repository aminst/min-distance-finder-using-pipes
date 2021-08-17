#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> parsed_line = parse_line(argv[1], "#");
    string trait_line = parsed_line[0];
    string file_path = parsed_line[1];
    string named_pipe = parsed_line[2];
    
    ifstream users_file(file_path);
    string file_line;

    bool first_time = true;
    double min;
    string min_line;
    while(getline(users_file, file_line))
    {
        if (first_time)
        {
            min = get_distance(file_line, trait_line);
            min_line = file_line;
            first_time = false;
        }
        else
        {
            double distance = get_distance(file_line, trait_line);
            if (distance < min)
            {
                min = distance;
                min_line = file_line;
            }
        }
    }


    int write_fifo = open(&named_pipe[0], O_WRONLY);
    char buff[1024] = {0};
    string message = file_path + "#" + to_string(min) + "#" + min_line;
    strcpy(buff, message.c_str());
    write(write_fifo, buff, 1024);

    close(write_fifo);
    exit(EXIT_SUCCESS);
    return 0;
}