#pragma once

#include <cstddef>
#include <utility>
#include <algorithm>
#include <cassert>
#include <random>

#include <genetics/genotype.h>

namespace genetics {

class bitwise_crossingover {

    template<class rnd_eng>
    std::pair<std::size_t, std::size_t> gen_range(rnd_eng &rnd, std::size_t n) {
        assert(n > 0);
        std::uniform_int_distribution<std::size_t> dist(0, n - 1);
        std::size_t l = dist(rnd), r = dist(rnd);
        if (l > r)
            std::swap(l, r);
        return std::make_pair(l, r);
    }
    
public:

    genotype operator()(const genotype &a, const genotype& b) {
        assert(a.size() == b.size());

        genotype res = a;
        
        std::random_device rd;
        std::mt19937 rnd(rd());
        
        std::size_t len = 0;
        while (2 * len < a.size()) {
            std::pair<std::size_t, std::size_t> p = gen_range(rnd, a.size());
            res.cross_over(b, p.first, p.second);
            len += p.second - p.first + 1;
        }

        return res;
    }
};

}
