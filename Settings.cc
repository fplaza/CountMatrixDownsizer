/*
 * Copyright 2016-2017 Enterome
 */ 

#include "Settings.hh"
#include <iostream>
#include <fstream>
#include <exception>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <omp.h>

Settings Settings::parse(int argc, char* argv[])
{
    Settings settings;

    // Create options decription
    po::options_description all_settings("");
    po::options_description input_settings("[input]");
    po::options_description downsizing_settings("[downsizing]");
    po::options_description output_settings("[output]");
    po::options_description misc_settings("[miscellaneous]");

    input_settings.add_options()
        ("count-matrix", po::value<std::string>(&settings.input.count_matrix_file)->required(), "File storing the count matrix to downsize")
        ;

    downsizing_settings.add_options()
        ("target-read-count", po::value<unsigned long>(&settings.downsizing.target_read_count)->required(), "Target read count in each sample")
        ("num-repetitions", po::value<unsigned long>(&settings.downsizing.num_repetitions)->default_value(1), "Number of downsizing repetitions")
        ("seed", po::value<int>(&settings.downsizing.seed)->default_value(12345), "Seed of the random number generator")
        ("num-threads,t", po::value<int>(&settings.downsizing.num_threads)->default_value(omp_get_max_threads()), "Number of threads to launch")
        ;

    output_settings.add_options()
        ("downsized-matrix", po::value<std::string>(&settings.output.downsized_count_matrix_file)->default_value(""), "Output file storing the downsized count matrix (optional)")
        ("stats", po::value<std::string>(&settings.output.stats_file)->default_value(""), "File storing statistics about the downsizing (optional)")
        ;

    misc_settings.add_options()
        ("help,h", "Display this help and exit.")
        ;

    all_settings.add(input_settings).add(downsizing_settings).add(output_settings).add(misc_settings);

    // Retrieve command line settings
    settings.executable_name=argv[0];
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_settings), vm);

    // Print help
    if (argc == 1 || vm.count("help"))
    {
        std::cout << all_settings << std::endl;
        std::exit(0);
    }

    po::notify(vm);

    check_file_is_readable(settings.input.count_matrix_file);
    check_val_within_bounds("--num-threads", settings.downsizing.num_threads, 1, omp_get_max_threads());
    
    if (settings.output.downsized_count_matrix_file == "" && settings.output.stats_file == "")
    {
        throw (std::invalid_argument("error: no output file"));
    }

    if (settings.output.downsized_count_matrix_file != "")
    {
        check_file_is_writable(settings.output.downsized_count_matrix_file);
    }
    
    if (settings.output.stats_file != "")
    {
        check_file_is_writable(settings.output.stats_file);
    }

    return settings;
}

void Settings::check_file_is_readable(const std::string& filepath)
{
    std::ifstream ifs;
    ifs.open(filepath.c_str());

    if (ifs.good())
    {
        ifs.close();
    }
    else
    {
        throw (std::invalid_argument("error: " + filepath +
                    " cannot be opened. Check that the path is valid and that you have read permissions."));
    }
}



void Settings::check_file_is_writable(const std::string& filepath)
{
    std::ofstream ofs;
    ofs.open(filepath.c_str());

    if (ofs.good())
    {
        ofs.close();
    }
    else
    {
        throw (std::invalid_argument("error: " + filepath +
                    " cannot be created. Check that the path is valid and that you have write permissions."));
    }
}

template <typename T>
void Settings::check_val_within_bounds(const std::string& val_name, T value, T lower, T higher)
{
    if(value < lower || value > higher)
    {
        throw (std::invalid_argument("error: " + val_name + " must be a value within range: " +
                    '[' + boost::lexical_cast<std::string>(lower) + ';' + boost::lexical_cast<std::string>(higher) + ']'));
    }
}

std::ostream& operator<<(std::ostream& os, const Settings& settings)
{
    const char sep[] = "---------------------\n";

    os << sep;
    os << "Settings summary:\n";

    os << "\n[input]\n";
    os << "--count-matrix = " << settings.input.count_matrix_file << '\n';

    os << "\n[downsizing]\n";
    os << "--target-read-count = " << settings.downsizing.target_read_count << '\n';
    os << "--num-repetitions = " << settings.downsizing.num_repetitions << '\n';
    os << "--seed = " << settings.downsizing.seed << '\n';
    os << "--num-threads = " << settings.downsizing.num_threads << '\n';

    os << "\n[output]\n";
    os << "--downsized-matrix = " << settings.output.downsized_count_matrix_file << '\n';
    os << "--stats= " << settings.output.stats_file << '\n';

    os << sep;

    return os;
}

