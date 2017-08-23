#ifndef SETTINGS_HH
#define SETTINGS_HH

/*
 * Copyright 2016-2017 Enterome
 */ 

#include <string>
#include <ostream>

struct Settings
{
    std::string executable_name;

    struct Input
    {
        std::string count_matrix_file;
    };
    Input input;

    struct Downsizing
    {
        unsigned long target_read_count;
        unsigned long num_repetitions;
        int seed;
        int num_threads;
    };
    Downsizing downsizing;

    struct Output
    {
        std::string downsized_count_matrix_file;
        std::string stats_file;
    }; 
    Output output;


    static Settings parse(int argc, char* argv[]);
    static void check_file_is_readable(const std::string& filepath);
    static void check_file_is_writable(const std::string& filepath);
    template <typename T>
    static void check_val_within_bounds(const std::string& val_name, T value, T lower, T higher);
};

std::ostream& operator<<(std::ostream& os, const Settings& settings);

#endif // SETTINGS_HH
