#include <iostream>
#include <random>
#include <cstdio>
#include <algorithm>
#include <sstream>

#include <gothello/gothello.h>
#include <gothello/strategy.h>
#include <gothello/scoring.h>
#include <gothello/player.h>
#include <gothello/sigsegv.h>
#include <gothello/rsf.h>
#include <genetics/genetics.h>
#include <strategy/best_scoring.h>

const int sz = 100;

int main() {
    utils::sigsegv_handler sh;

    typedef reversi::scoring::weighted_quater<double> scoring_t;
    typedef gothello::scoring::weighted_quater<double> scoring_w_t;
    typedef gothello::strategy<reversi::player::best_scoring, scoring_w_t> strategy_t;
    typedef gothello::player_phenotype<strategy_t> phenotype_t;
    typedef std::shared_ptr<gothello::player> player_ptr;
    typedef gothello::player_factory<strategy_t> factory_t;

    auto p = std::make_shared<genetics::population<gothello::phenotype_ptr, factory_t> >();

    for (int i = 0; i < sz; i++) {
        scoring_t scoring;
        scoring.gen_random<std::mt19937>();
        scoring_w_t scoring_w(scoring);
        strategy_t strategy(scoring);
        gothello::phenotype_ptr phenotype = std::make_shared<phenotype_t>(strategy);
        phenotype->get_ratings() = 1500;
        genetics::genotype genotype = strategy.get_genotype();
        player_ptr player = std::make_shared<gothello::player>(genotype, phenotype);

        std::ostringstream ss;
        ss << "/tmp/G0PBSWQN" << i << ".rsf";
        write_rsf(ss.str(), player);
        
        p->push_back(player);
    }
    
    gothello::genetics_engine eng;
    eng.add_population<strategy_t>(p);

    int ng = 1;
    while (true) {
        eng.play_tournaments();
        std::cout << "played tournaments" << std::endl;
        eng.new_generation();
        double s = 0, n = 0;
        gothello::elo_t mn = std::numeric_limits<gothello::elo_t>::max();
        gothello::elo_t mx = std::numeric_limits<gothello::elo_t>::min();
        for (const auto &storer : eng) {
            n += storer->size();
            for (const auto &strategy : *storer) {
                s += strategy->get_phenotype()->get_ratings();
                mn = std::min(mn, strategy->get_phenotype()->get_ratings());
                mx = std::max(mx, strategy->get_phenotype()->get_ratings());
            }
        }
        printf("new generation (ng=%d, elo: min=%d, max=%d, avg=%.02f)\n", ng, mn, mx, s / n);
        ++ng;
    }
    
}
