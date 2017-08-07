#pragma once

#include <vector>
#include <string>
#include <map>
#include <cmath>

#include <gothello/elo.h>
#include <gothello/tournament.h>
#include <gothello/player.h>
#include <genetics/genetics.h>

namespace gothello {

class genetics_engine {
public:
    struct {
        std::size_t ntournaments = 5;
        std::size_t tournament_size = 20;
        double alivers = 0.8;
        double mutation_frequency = 4.0;
    } config;

    class basic_population_storer {
    public:
        virtual ~basic_population_storer() = default;

        typedef typename std::vector<genetics::basic_individual_ptr<phenotype_ptr> >::iterator iterator;
        
        virtual iterator new_generation(const genetics_engine &eng) = 0;
        virtual std::size_t size() const = 0;
        virtual genetics::basic_individual_ptr<phenotype_ptr> operator[](std::size_t i) = 0;
        virtual iterator begin() = 0;
        virtual iterator end() = 0;
    };

    template<class strategy_t>
    class population_storer : public basic_population_storer {
        std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population_;
    public:
        population_storer(std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population)
            : population_(population) {}
        virtual ~population_storer() = default;

        typedef basic_population_storer::iterator iterator;
        
        virtual iterator new_generation(const genetics_engine &eng) override {
            player_fitness pf;
            return population_->template new_generation<elo_t, player_fitness>(std::ceil(eng.config.alivers * population_->size()),
                                                                               population_->size(),
                                                                               eng.config.mutation_frequency,
                                                                               pf);
        }
        
        virtual std::size_t size() const override {
            return population_->size();
        }

        virtual genetics::basic_individual_ptr<phenotype_ptr> operator[](std::size_t i) override {
            return population_->at(i);
        }

        virtual iterator begin() override {
            return population_->begin();
        }

        virtual iterator end() override {
            return population_->end();
        }
    };

private:
    std::vector<std::shared_ptr<basic_population_storer> > populations_;
    
public:
    genetics_engine() {}

    template<class strategy_t>
    void add_population(std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population) {
        populations_.push_back(std::make_shared<population_storer<strategy_t> >(population));
    }
    
    void play_tournaments();

    void /* Here comes the young, the */ new_generation();

    typedef std::vector<std::shared_ptr<basic_population_storer> >::const_iterator iterator;

    iterator begin() const { return populations_.begin(); }
    iterator end() const { return populations_.end(); }

    std::size_t size() const { return populations_.size(); }
    const std::shared_ptr<basic_population_storer>& operator[](std::size_t i) const { return populations_[i]; };
};
    
}
