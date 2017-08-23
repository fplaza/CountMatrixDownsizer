/*
 * Copyright 2016-2017 Enterome
 */ 

#include "SampleDownsizer.hh"
#include "randomc.h"
#include <omp.h>
#include <ctime>
#include <cmath>

struct SampleDownsizer::Impl
{
    Impl(Matrix<float>& count_matrix, const unsigned long target_read_count, const unsigned long num_repetitions, const int seed);
    void reset_genes_abundance_profile(std::vector<unsigned long>& genes_abundance_profile);
    void load_inital_genes_abundance_profile();
    unsigned long compute_gene_count();
    void init_reads_pool();
    void run_downsizing_rep();
    void update_final_genes_abundance_profile();
    void store_final_genes_abundance_profile();
    DownsizingStats compute_stats();

    CRandomMersenne mt_;
    Matrix<float>& count_matrix_;
    const unsigned long target_read_count_;
    const unsigned long num_repetitions_;
    const int seed_;
    size_t curr_sample_;
    std::vector<unsigned long> curr_rep_genes_abundance_profile_;
    std::vector<unsigned long> final_genes_abundance_profile_;
    size_t initial_read_count_;
    std::vector<unsigned long> reads_pool_;
    std::vector<unsigned long> gene_count_;
};


SampleDownsizer::Impl::Impl(Matrix<float>& count_matrix, const unsigned long target_read_count, const unsigned long num_repetitions, const int seed)
    :mt_(0),
    count_matrix_(count_matrix),
    target_read_count_(target_read_count),
    num_repetitions_(num_repetitions),
    seed_(seed),
    curr_sample_(0),
    curr_rep_genes_abundance_profile_(count_matrix.nrow()),
    final_genes_abundance_profile_(count_matrix.nrow()),
    initial_read_count_(0),
    reads_pool_(),
    gene_count_(num_repetitions)
{}

void SampleDownsizer::Impl::reset_genes_abundance_profile(std::vector<unsigned long>& genes_abundance_profile)
{
    std::fill(genes_abundance_profile.begin(), genes_abundance_profile.end(), 0);
}

void SampleDownsizer::Impl::load_inital_genes_abundance_profile()
{
    initial_read_count_ = 0;
    for (size_t curr_gene = 0; curr_gene < count_matrix_.nrow(); ++curr_gene)
    {
        curr_rep_genes_abundance_profile_[curr_gene] =
            static_cast<unsigned long>(count_matrix_(curr_gene, curr_sample_)+0.5);

        initial_read_count_ += curr_rep_genes_abundance_profile_[curr_gene];
    }
}

unsigned long SampleDownsizer::Impl::compute_gene_count()
{
    unsigned long gene_count = 0;

    for (std::vector<unsigned long>::iterator gene_abundance = curr_rep_genes_abundance_profile_.begin(); gene_abundance < curr_rep_genes_abundance_profile_.end(); ++gene_abundance)
    {
        if (*gene_abundance > 0)
        {
            ++gene_count;
        }
    }

    return gene_count;
}

void SampleDownsizer::Impl::init_reads_pool()
{
    reads_pool_.resize(initial_read_count_);

    size_t reads_inserted = 0;
    for (size_t curr_gene = 0; curr_gene < count_matrix_.nrow(); ++curr_gene)
    {
        const unsigned long gene_read_count = 
            curr_rep_genes_abundance_profile_[curr_gene];

        for (unsigned long curr_read = 0; curr_read < gene_read_count; ++curr_read)
            reads_pool_[reads_inserted++] = curr_gene;
    }

}

void SampleDownsizer::Impl::run_downsizing_rep()
{
    // Use Knuth's variable names
    const size_t n = target_read_count_;
    const size_t N = initial_read_count_;

    // total input records dealt with
    size_t t = 0;
    // number of items selected so far
    size_t m = 0;

    while (m < n)
    {
        // call a uniform(0,1) random number generator
        const double u = mt_.Random();

        if ( (N - t)*u < n - m )
        {
            curr_rep_genes_abundance_profile_[reads_pool_[t]]++;
            ++m;
        }

        ++t;
    }
}

void SampleDownsizer::Impl::update_final_genes_abundance_profile()
{
    for (size_t curr_gene = 0; curr_gene < count_matrix_.nrow(); ++curr_gene)
    {
        final_genes_abundance_profile_[curr_gene] += 
            curr_rep_genes_abundance_profile_[curr_gene];
    }

}

void SampleDownsizer::Impl::store_final_genes_abundance_profile()
{
    if (num_repetitions_ == 1)
    {
        for (size_t curr_gene = 0; curr_gene < count_matrix_.nrow(); ++curr_gene)
        {
            count_matrix_(curr_gene, curr_sample_) =
                static_cast<unsigned long>(final_genes_abundance_profile_[curr_gene]);
        }
    }
    else
    {
        for (size_t curr_gene = 0; curr_gene < count_matrix_.nrow(); ++curr_gene)
        {
            const double final_mean_gene_abundance_profile =
                static_cast<double>(final_genes_abundance_profile_[curr_gene])/num_repetitions_;

            count_matrix_(curr_gene, curr_sample_) =
                static_cast<unsigned long>(final_mean_gene_abundance_profile + 0.5);
        }
    }
}

DownsizingStats SampleDownsizer::Impl::compute_stats()
{
    unsigned long sum_gene_count = 0;

    for(size_t curr_rep = 0; curr_rep < num_repetitions_; ++curr_rep)
    {
        sum_gene_count += gene_count_[curr_rep];
    }

    const double mean_gene_count = 
        static_cast<double>(sum_gene_count)/num_repetitions_;

    double stddev_gene_count = 0.0;

    for(size_t curr_rep = 0; curr_rep < num_repetitions_; ++curr_rep)
    {
        stddev_gene_count += std::pow(gene_count_[curr_rep]-mean_gene_count, 2);
    }
    
    stddev_gene_count = std::sqrt(stddev_gene_count/num_repetitions_);

    DownsizingStats downsizing_stats = {
        count_matrix_.colnames()[curr_sample_],
        target_read_count_,
        num_repetitions_,
        initial_read_count_,
        initial_read_count_ >= target_read_count_,
        mean_gene_count,
        stddev_gene_count
    };

    return downsizing_stats;
}

SampleDownsizer::SampleDownsizer(Matrix<float>& count_matrix, const unsigned long target_read_count, const unsigned long num_repetitions, const int seed)
    : impl_(new Impl(count_matrix, target_read_count, num_repetitions, seed))
{}

DownsizingStats SampleDownsizer::downsize(const size_t curr_sample)
{
    impl_->curr_sample_ = curr_sample;
    impl_->reset_genes_abundance_profile(impl_->curr_rep_genes_abundance_profile_);
    impl_->reset_genes_abundance_profile(impl_->final_genes_abundance_profile_);
    impl_->load_inital_genes_abundance_profile();

    if (impl_->initial_read_count_ >= impl_->target_read_count_)
    {
        impl_->init_reads_pool();
        impl_->mt_.RandomInit(impl_->seed_ + impl_->curr_sample_);

        for (size_t curr_rep = 0; curr_rep < impl_->num_repetitions_; ++curr_rep)
        {
            impl_->reset_genes_abundance_profile(impl_->curr_rep_genes_abundance_profile_);
            impl_->run_downsizing_rep();
            impl_->gene_count_[curr_rep] = impl_->compute_gene_count();
            impl_->update_final_genes_abundance_profile();
        }
    }

    impl_->store_final_genes_abundance_profile();

    const DownsizingStats downsizing_stats = 
        impl_->compute_stats();

    return downsizing_stats;
}

