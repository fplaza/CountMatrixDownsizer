#ifndef SAMPLE_DOWNSIZER_HH
#define SAMPLE_DOWNSIZER_HH

/*
 * Copyright 2016-2017 Enterome
 */ 

#include "Matrix.hh"

struct DownsizingStats
{
    std::string sample_name;
    unsigned long target_read_count;
    unsigned long num_repetitions;
    unsigned long initial_read_count;
    bool success;
    double mean_gene_count;
    double stddev_gene_count;
};

class SampleDownsizer
{
    public:
        SampleDownsizer(Matrix<float>& count_matrix, const unsigned long target_read_count, const unsigned long num_repetitions, const int seed);
        DownsizingStats downsize(const size_t curr_sample);

    private:
        struct Impl;
        Impl* impl_;
};

#endif // SAMPLE_DOWNSIZER_HH

