#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cassert>
#include <type_traits>
#include <iterator>
#include <numeric>

#include <genetics/genotype.h>
#include <genetics/merge_processor.h>
#include <genetics/mutation_processor.h>

namespace genetics {

template<class phenotype>
class basic_individual {
    struct info {
        std::string name = "";
        std::size_t generation;
    };
    
private:
    genotype genotype_;
    info info_;
    
public:    
    basic_individual(const genotype& genotype)
        : genotype_(genotype) {}
    virtual ~basic_individual() = default;

    genotype &get_genotype() { return genotype_; }
    const genotype &get_genotype() const { return genotype_; }

    info &get_info() { return info_; }
    const info &get_info() const { return info_; }

    virtual phenotype &get_phenotype() = 0;
    virtual const phenotype &get_phenotype() const = 0;
};

template<class phenotype>
using basic_individual_ptr = std::shared_ptr<basic_individual<phenotype> >;

template<class phenotype, class phenotype_factory_t,
         class merge_processor = bitwise_crossingover,
         class mutation_processor = random_bit_flip>
class population : public std::vector<basic_individual_ptr<phenotype> > {
    phenotype_factory_t pf_;
    merge_processor cp_;
    mutation_processor mp_;

    template<class fitness_t, class rnd_eng>
    typename std::enable_if<std::is_integral<fitness_t>::value, fitness_t>::type gen_in_range(rnd_eng &eng, fitness_t start, fitness_t end) {
        assert(start <= end);
        std::uniform_int_distribution<fitness_t> dist(start, end);
        return dist(eng);
    }

    template<class fitness_t, class rnd_eng>
    typename std::enable_if<std::is_floating_point<fitness_t>::value, fitness_t>::type gen_in_range(rnd_eng &eng, fitness_t start, fitness_t end) {
        assert(start <= end);
        std::uniform_real_distribution<fitness_t> dist(start, end);
        return dist(eng);
    }
    
public:
    typedef basic_individual_ptr<phenotype> stored_t;
    
    population(phenotype_factory_t pf = phenotype_factory_t(),
               merge_processor cp = merge_processor(),
               mutation_processor mp = mutation_processor())
        : std::vector<basic_individual_ptr<phenotype> >(),
        pf_(pf), cp_(cp), mp_(mp){}

    /** This function does one iteration of lifecycle.
      * New individuals are placed into the end of population
      *
      * @param alivers               number of individuals that alives through 
      *                              current population
      * @param new_size              size of the new population
      * @param mutation_frequency    number of mutations in one genotype is
      *                              generated by uniform distribution in range
      *                              [0; mutation_frequency)
      * @param fitness_calculator    functor that calculates fitness of an
      *                              individual.
      * 
      * @return                      an iterator to first newly generated 
      *                              individual
      */
    template<class fitness_t, class fitness_p, class rnd_gen = std::mt19937>
    typename std::vector<basic_individual_ptr<phenotype> >::iterator new_generation(std::size_t alivers, std::size_t new_size,
                            double mutation_frequency,
                            fitness_p &fitness_calculator) {
        assert(alivers <= this->size());
        assert(alivers <= new_size);

        std::sort(this->begin(), this->end(),
                  [&fitness_calculator](const stored_t& a, const stored_t& b) -> bool {
                      return fitness_calculator(a->get_phenotype()) > fitness_calculator(b->get_phenotype());
                  });

        std::vector<fitness_t> probabilities;
        for (auto & individual : *this) {
            probabilities.push_back(fitness_calculator(individual->get_phenotype()));
        }
        std::vector<fitness_t> partial_sums = { 0 };
        std::partial_sum(probabilities.begin(), probabilities.end(),
                         std::back_inserter(partial_sums));

        std::random_device rd;
        rnd_gen rnd(rd());
        
        std::vector<stored_t> new_individuals;
        for (std::size_t i = 0; i != (std::size_t)(new_size - alivers); i++) {
            fitness_t ff = gen_in_range<fitness_t>(rnd, 0, partial_sums.back());
            fitness_t fm = gen_in_range<fitness_t>(rnd, 0, partial_sums.back());
            std::size_t f_i = std::prev(std::upper_bound(partial_sums.begin(), partial_sums.end(), ff)) - partial_sums.begin();
            std::size_t m_i = std::prev(std::upper_bound(partial_sums.begin(), partial_sums.end(), fm)) - partial_sums.begin();

            genotype individual_g = cp_((*(this->begin() + f_i))->get_genotype(),
                                        (*(this->begin() + m_i))->get_genotype());
            stored_t individual = pf_(individual_g);

            std::size_t mutations = gen_in_range<double>(rnd, 0, mutation_frequency);
            for (std::size_t m = 0; m != mutations; m++)
                mp_(rnd, individual->get_genotype());

            new_individuals.push_back(individual);
        }

        while (this->size() > alivers)
            this->pop_back();
        std::copy(new_individuals.begin(), new_individuals.end(),
                  std::back_inserter(*this));
        
        return this->begin() + alivers;
    }
};

}
