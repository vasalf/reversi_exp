#pragma once

#include <random>
#include <cassert>

#include <genetics/genotype.h>

namespace genetics {

class random_bit_flip {
public:
    template<class rnd_eng>
    void operator()(rnd_eng &rnd, genotype &g) {
        assert(g.size() > 0);
        std::uniform_int_distribution<std::size_t> dist(0, g.size() - 1);
        std::size_t i = dist(rnd);
        if (i < g.size())
            g[i] = !g[i];
    }
};

}
