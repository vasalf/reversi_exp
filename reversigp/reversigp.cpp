#include <iostream>
#include <random>

#include <gothello/gothello.h>
#include <gothello/strategy.h>
#include <gothello/scoring.h>
#include <gothello/player.h>
#include <genetics/genetics.h>
#include <strategy/best_scoring.h>

const int sz = 100;

int main() {

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
        p->push_back(player);
    }
    
    gothello::genetics_engine eng;
    eng.add_population<strategy_t>(p);

    while (true) {
        eng.play_tournaments();
        std::cout << "played tournaments" << std::endl;
        eng.new_generation();
        std::cout << "next generation" << std::endl;
    }
    
}
