/*
 * Copyright 2016-2017 Enterome
 */ 

#include "DownsizingStatsPrinter.hh"
#include <fstream>
#include <iomanip>

const char DownsizingStatsPrinter::HEADER[] = 
"sample_name	is_unique_reads	gene_count_threshold	total_read_count	final_read_count	downsizing_repeat_count	mean_gene_count	stddev_gene_count\n";

void DownsizingStatsPrinter::print(const std::vector<DownsizingStats>& samples_downsizing_stats, const std::string& output_file)
{
    std::ofstream ofs;
    ofs.open(output_file.c_str(), std::ios_base::app);

    ofs << HEADER;

    for (std::vector<DownsizingStats>::const_iterator downsizing_stats = samples_downsizing_stats.begin(); downsizing_stats < samples_downsizing_stats.end(); ++downsizing_stats)
    {
        ofs << downsizing_stats->sample_name << 
            '\t' << "NA\t1" << 
            '\t' << downsizing_stats->initial_read_count <<
            '\t' << downsizing_stats->target_read_count << 
            '\t' << downsizing_stats->num_repetitions;

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
