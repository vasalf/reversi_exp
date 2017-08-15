#pragma once

#include <memory>
#include <string>

#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <genetics/genetics.h>

namespace gothello {

class basic_player_phenotype {
    elo_t ratings_;
    std::string name_;
    std::vector<elo_t> ratings_history_;
public:
    virtual ~basic_player_phenotype() = default;

    elo_t & get_ratings() { return ratings_; }
    const elo_t & get_ratings() const { return ratings_; }
    
    std::string &name() { return name_; }
    const std::string &name() const { return name_; }
    
    std::vector<elo_t> &ratings_history() { return ratings_history_; }
    const std::vector<elo_t> &ratings_history() const { return ratings_history_; }
    
    virtual std::shared_ptr<reversi::player::player> get_strategy(char who) = 0;
    virtual std::string description() const = 0;
};
typedef std::shared_ptr<basic_player_phenotype> phenotype_ptr;

template<class impl_strategy>
class player_phenotype : public basic_player_phenotype {
    std::shared_ptr<impl_strategy> impl_;
public:
    player_phenotype(const genetics::genotype &g)
        : impl_(std::make_shared<impl_strategy>(g)) {}
    player_phenotype(const impl_strategy &s)
        : impl_(std::make_shared<impl_strategy>(s)) {}
    virtual ~player_phenotype() = default;

    virtual std::shared_ptr<reversi::player::player> get_strategy(char who) override {
        return impl_->create_player(who);
    }

    virtual std::string description() const override {
        return impl_strategy::description();
    }
};

struct player_fitness {
    elo_t operator()(const phenotype_ptr& ph) {
        return ph->get_ratings();
    }
};
    
class player : public genetics::basic_individual<phenotype_ptr> {
    phenotype_ptr phenotype_;

public:
    player(const genetics::genotype& genotype,
           phenotype_ptr phenotype)
        : genetics::basic_individual<phenotype_ptr>(genotype),
        phenotype_(phenotype) {}
    virtual ~player() = default;

    virtual phenotype_ptr &get_phenotype() { return phenotype_; }
    virtual const phenotype_ptr &get_phenotype() const { return phenotype_; }
};

template<class strategy>
class player_factory {
public:
    std::shared_ptr<player> operator()(const genetics::genotype &g) {
        return std::make_shared<player>(g, std::make_shared<player_phenotype<strategy> >(g));
    }
};

}
