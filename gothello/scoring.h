#pragma once

#include <strategy/scorings.h>
#include <genetics/genotype.h>

namespace gothello {
namespace scoring {

template<typename score_type>
class weighted_quater {
    reversi::scoring::weighted_quater<score_type> s_;

    // tss is for transcrption start site 
    score_type transcript(const genetics::genotype &g, std::size_t tss) {
        char data[sizeof(score_type)];
        for (std::size_t j = 0; j != sizeof(score_type); j++) {
            for (std::size_t k = 0; k != 8; k++) {
                data[j] |= g[tss + j * 8 + k] << k;
            }
        }
        return *reinterpret_cast<score_type*>(data);
    }

    void reverse_transcript/*lol*/(genetics::genotype &g, std::size_t tss, score_type w) {
        char *data = reinterpret_cast<char*>(&w);
        for (std::size_t j = 0; j != sizeof(score_type); j++) {
            for (std::size_t k = 0; k != 8; k++) {
                g[tss + j * 8 + k] = (data[j] >> k) & 1;
            }
        }
    }
    
public:
    typedef reversi::scoring::weighted_quater<score_type> wrapped_t;
    
    weighted_quater(reversi::scoring::weighted_quater<score_type> s)
        : s_(s) {}

    weighted_quater(const genetics::genotype &g) {
        std::size_t start = 0;
        for (std::size_t i = 0; i != wrapped_t::half_field; i++) {
            for (std::size_t j = 0; j != wrapped_t::half_field; j++) {
                s_[i][j] = transcript(g, start);
                start += 8 * sizeof(score_type);
            }
        }
    }

    virtual ~weighted_quater() = default;

    const reversi::scoring::weighted_quater<score_type> &wrapped() const {
        return s_;
    }

    reversi::scoring::weighted_quater<score_type> &wrapped() {
        return s_;
    }

    genetics::genotype get_genotype() {
        genetics::genotype ret(8 * sizeof(score_type) * wrapped_t::half_field * wrapped_t::half_field);
        std::size_t start = 0;
        for (std::size_t i = 0; i != wrapped_t::half_field; i++) {
            for (std::size_t j = 0; j != wrapped_t::half_field; j++) {
                reverse_transcript(ret, start, s_[i][j]);
                start += 8 * sizeof(score_type);
            }
        }
        return ret;
    }
};

}
}
