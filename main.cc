/*
 * Copyright 2016-2017 Enterome
 */ 

#include <iostream>
#include <new>
#include <cstdlib>
#include <stdexcept>
#include "Settings.hh"
#include "ExecutionDescription.hh"
#include "MatrixReader.hh"
#include "SampleDownsizer.hh"
#include "ExecutionDescriptionPrinter.hh"
#include "MatrixPrinter.hh"
#include "DownsizingStatsPrinter.hh"
#include "TimeProfiler.hh"

void no_more_memory()
{
    std::cerr << "error: memory allocation failed." << std::endl;
    std::exit (1);
}

int main(int argc, char* argv[])
{
    std::set_new_handler(no_more_memory);

    try
    {
        TimeProfiler time_profiler;
        time_profiler.start_new_timer("Total");

        const Settings& settings = Settings::parse(argc, argv);
        std::cout << settings << std::endl;

        const ExecutionDescription execution_description(settings);

        std::cout << "Reading count matrix..." << std::endl;
        time_profiler.start_new_timer("Reading count matrix");
        Matrix<float> count_matrix = MatrixReader<float>::read(settings.input.count_matrix_file);
        time_profiler.stop_last_timer();
        std::cout << "\rDone. Count matrix has " << count_matrix.ncol() << " samples and " << count_matrix.nrow() << " genes.\n" << std::endl;

        std::cout << "Performing downsizing...\n";
        std::cout << "Progress: 0%" << std::flush;
        time_profiler.start_new_timer("Downsizing count matrix");

        std::vector<DownsizingStats> samples_downsizing_stats(count_matrix.ncol());
        unsigned long progress = 0;
        #pragma omp parallel num_threads(settings.downsizing.num_threads)
        {
            SampleDownsizer sample_downsizer(
                    count_matrix,
                    settings.downsizing.target_read_count,
                    settings.downsizing.num_repetitions,
                    settings.downsizing.seed);

            #pragma omp for schedule(dynamic,1)
            for (size_t curr_sample = 0; curr_sample < count_matrix.ncol(); ++curr_sample)
            {
                samples_downsizing_stats[curr_sample] = sample_downsizer.downsize(curr_sample);

                #pragma omp critical
                {
                    ++progress;
                    const unsigned int perc_progress =
                        static_cast<unsigned int>(100.0*progress/count_matrix.ncol());
                    std::cout << "\rProgress: " << perc_progress << '%' << std::flush;
                }
            }
        }
        time_profiler.stop_last_timer();

        std::cout << "\rProgress: 100%\n" << std::endl;

        if (settings.output.downsized_count_matrix_file != "")
        {
            std::cout << "Writing downsized count matrix..." << std::endl;
            time_profiler.start_new_timer("Writing downsized count matrix");
            ExecutionDescriptionPrinter::print(execution_description, settings.output.downsized_count_matrix_file);
            MatrixPrinter<float>::print(count_matrix, settings.output.downsized_count_matrix_file);
            time_profiler.stop_last_timer();
            std::cout << "\rDone         \n" << std::endl;
        }

        if (settings.output.stats_file != "")
        {
            std::cout << "Writing downsizing statistics..." << std::endl;
            time_profiler.start_new_timer("Writing downsizing statistics");
            ExecutionDescriptionPrinter::print(execution_description, settings.output.stats_file);
            DownsizingStatsPrinter::print(samples_downsizing_stats, settings.output.stats_file);
            time_profiler.stop_last_timer();
            std::cout << "Done\n" << std::endl;
        }


        time_profiler.stop_last_timer();
        std::cout << time_profiler << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }

    std::exit(0);
}
