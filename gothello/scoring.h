#pragma once

#include <strategy/scorings.h>
#include <genetics/genotype.h>

namespace gothello {
namespace scoring {

template<typename score_type>
class weighted_quater {
    reversi::scoring::weighted_quater<score_type> s_;

public:
    typedef reversi::scoring::weighted_quater<score_type> wrapped_t;
    
    weighted_quater(reversi::scoring::weighted_quater<score_type> s)
        : s_(s) {}
    weighted_quater(genetics::genotype g) {
        // TODO
    }
    virtual ~weighted_quater() = default;

    const reversi::scoring::weighted_quater<score_type> &wrapped() const {
        return s_;
    }

    reversi::scoring::weighted_quater<score_type> &wrapped() {
        return s_;
    }

    genetics::genotype get_genotype() {
        // TODO
        return genetics::genotype(0);
    }
};

}
}
