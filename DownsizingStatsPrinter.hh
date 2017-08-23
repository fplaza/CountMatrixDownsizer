/*
 * Copyright 2016-2017 Enterome
 */ 

#ifndef DOWNSIZING_STATS_PRINTER_HH
#define DOWNSIZING_STATS_PRINTER_HH

#include "SampleDownsizer.hh"
#include <vector>
#include <string>

class DownsizingStatsPrinter
{
    public:
        static void print(const std::vector<DownsizingStats>& samples_downsizing_stats, const std::string& output_file);
    private:
        static const char HEADER[];
};

#endif // DOWNSIZING_STATS_PRINTER_HH
