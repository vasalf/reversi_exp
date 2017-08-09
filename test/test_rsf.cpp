#include <memory>

#include <gtest/gtest.h>

#include <gothello/rsf.h>
#include <gothello/player.h>
#include <gothello/scoring.h>
#include <gothello/strategy.h>
#include <strategy/scorings.h>
#include <strategy/best_scoring.h>
#include <genetics/genotype.h>

bool operator==(const genetics::genotype &g, const genetics::genotype &h) {
    if (g.size() != h.size())
        return false;
    for (std::size_t i = 0; i != g.size(); i++)
        if (g[i] != h[i])
            return false;
    return true;
}

template<typename score_type>
class WQScoring : public ::testing::Test {
public:
    typedef reversi::scoring::weighted_quater<score_type> scoring_t;
    typedef gothello::scoring::weighted_quater<score_type> scoring_w_t;
};

typedef ::testing::Types<int, double> WQTypes;
TYPED_TEST_CASE(WQScoring, WQTypes);

TYPED_TEST(WQScoring, read_write) {
    typedef gothello::strategy<reversi::player::best_scoring, typename TestFixture::scoring_w_t> strategy_w_t;
    typedef gothello::player_phenotype<strategy_w_t> phenotype_t;
    
    std::mt19937 rnd(179);
    
    typename TestFixture::scoring_t scoring;
    scoring.gen_random(rnd);
    typename TestFixture::scoring_w_t scoring_w(scoring);
    strategy_w_t strategy(scoring);
    gothello::phenotype_ptr phenotype = std::make_shared<phenotype_t>(strategy);
    genetics::genotype genotype = strategy.get_genotype();
    std::shared_ptr<gothello::player> player = std::make_shared<gothello::player>(genotype, phenotype);

    gothello::write_rsf("/tmp/test.rsf", player);
    std::shared_ptr<gothello::player> read_player = gothello::read_rsf("/tmp/test.rsf");
    EXPECT_EQ(read_player->get_phenotype()->description(), player->get_phenotype()->description());
    EXPECT_TRUE(read_player->get_genotype() == genotype);
}

    
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
