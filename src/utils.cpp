#include "utils.hpp"

using namespace std;

vector<string> parse_line(string line, string delimiter)
{
    vector<string> result;

    int start = 0;
    int end = line.find(delimiter);
    result.push_back(line.substr(start, end - start));
    while (end != -1) {
        start = end + delimiter.size();
        end = line.find(delimiter, start);
        result.push_back(line.substr(start, end - start));
    }

    return result;
}

double get_distance(string first, string second)
{
    vector<string> first_parsed = parse_line(first, ",");
    vector<string> second_parsed = parse_line(second, ",");

    const int CHARACTERISITCS_COUNT = 5;
    int sum = 0;
    for (int i = 0; i < CHARACTERISITCS_COUNT; i++)
    {
        sum += pow(stoi(first_parsed[i]) - stoi(second_parsed[i]), 2);
    }
    return sqrt(sum);
}