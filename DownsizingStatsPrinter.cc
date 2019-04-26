/*
 * Copyright 2016-2017 Enterome
 */ 

#include "DownsizingStatsPrinter.hh"
#include <fstream>
#include <iomanip>

const char DownsizingStatsPrinter::HEADER[] = 
"sample_name	initial_read_count	target_read_count	downsizing_num_rep	initial_gene_count	final_mean_gene_count	final_stddev_gene_count\n";

void DownsizingStatsPrinter::print(const std::vector<DownsizingStats>& samples_downsizing_stats, const std::string& output_file)
{
    std::ofstream ofs;
    ofs.open(output_file.c_str(), std::ios_base::app);

    ofs << HEADER;

    for (std::vector<DownsizingStats>::const_iterator downsizing_stats = samples_downsizing_stats.begin(); downsizing_stats < samples_downsizing_stats.end(); ++downsizing_stats)
    {
        ofs << downsizing_stats->sample_name << 
            '\t' << downsizing_stats->initial_read_count <<
            '\t' << downsizing_stats->target_read_count << 
            '\t' << downsizing_stats->num_repetitions <<
            '\t' << downsizing_stats->initial_gene_count;

        if (downsizing_stats->success)
        {
            ofs << '\t' << std::fixed << std::setprecision(2) << downsizing_stats->mean_gene_count << 
                '\t' << std::setprecision(2) << downsizing_stats->stddev_gene_count << '\n';
        }
        else
        {
            ofs << "\tNA\tNA\n";
        }
    }

    ofs.close();
}
