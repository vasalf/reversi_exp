#include <cstddef>
#include <random>
#include <iostream>
#include <iomanip>

#include <gtest/gtest.h>

#include <gothello/scoring.h>
#include <strategy/scorings.h>
#include <genetics/genotype.h>

bool operator==(const genetics::genotype &g1, const genetics::genotype &g2) {
    if (g1.size() != g2.size())
        return false;
    for (std::size_t i = 0; i != g1.size(); i++) {
        if (g1[i] != g2[i])
            return false;
    }
    return true;
}

TEST(GenotypeToPhenotypeConversions, weighted_quater) {
    typedef double score_type;
    typedef reversi::scoring::weighted_quater<score_type> wrapped_t;

    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    
    const std::size_t sz = 8 * sizeof(score_type) * wrapped_t::half_field * wrapped_t::half_field;
    genetics::genotype g(sz);

    for (std::size_t i = 0; i != sz; i++)
        g[i] = dist(rnd);

    genetics::genotype h = gothello::scoring::weighted_quater<score_type>(g).get_genotype();
    
    EXPECT_TRUE(h == g);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
