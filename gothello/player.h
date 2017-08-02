#pragma once

#include <memory>

#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <genetics/genetics.h>

namespace gothello {

class basic_player_phenotype {
    elo_t ratings_;
public:
    virtual ~basic_player_phenotype() = default;

    elo_t & get_ratings() { return ratings_; }
    const elo_t & get_ratings() const { return ratings_; }
    
    virtual std::shared_ptr<reversi::player::player> get_strategy() = 0;
};
typedef std::shared_ptr<basic_player_phenotype> phenotype_ptr;

template<class impl_strategy>
class player_phenotype : public basic_player_phenotype {
    std::shared_ptr<impl_strategy> impl_;
public:
    player_phenotype(const genetics::genotype &g)
        : impl_(std::make_shared<impl_strategy>(g)) {}
    virtual ~player_phenotype() = default;

    virtual std::shared_ptr<reversi::player::player> get_stragegy() override {
        return impl_;
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

}
