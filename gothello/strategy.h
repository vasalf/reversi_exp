#pragma once

#include <genetics/genotype.h>

namespace gothello {

template<template<typename> class strategy_t,
         class scoring_t>
class strategy {
    scoring_t s_;
public:
    typedef typename scoring_t::wrapped_t scoring_wrapped_t;
    
    strategy(scoring_t s)
        : s_(s) {}
    strategy(const genetics::genotype &g)
        : s_(g) {}
    virtual ~strategy() = default;

    genetics::genotype get_genotype() {
        return s_.get_genotype();
    }

    std::shared_ptr<strategy_t<scoring_wrapped_t> > create_player(char who) {
        return std::make_shared<strategy_t<scoring_wrapped_t> >(who, s_.wrapped());
    }
};

}
