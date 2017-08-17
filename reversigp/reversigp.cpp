#include <iostream>
#include <random>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <limits>

#include <gothello/gothello.h>
#include <gothello/elo.h>
#include <gothello/sigsegv.h>

#include <gflags/gflags.h>

DEFINE_string(params, "", "Path to JSON file with launch params");

bool is_non_empty([[gnu::unused]] const char *flagname, const std::string &value) {
    return value != "";
}

DEFINE_validator(params, &is_non_empty);

DEFINE_bool(continue, false, "Continue an interrupted launch");

int main(int argc, char *argv[]) {
    utils::sigsegv_handler sh;

    gflags::SetUsageMessage("libgothello interface to generate reversi algorithms");
    gflags::SetVersionString("1.0.0");

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    gothello::engine eng(FLAGS_params, FLAGS_continue);

    gflags::ShutDownCommandLineFlags();

    while (true) {
        eng.play_tournaments();
        std::cout << "played tournaments" << std::endl;

        std::size_t total_size = 0;
        gothello::elo_t sum_elo = 0;
        gothello::elo_t min_elo = std::numeric_limits<gothello::elo_t>::max();
        gothello::elo_t max_elo = std::numeric_limits<gothello::elo_t>::min();
        for (auto storer : eng.eng())
            for (auto p : *storer) {
                total_size++;
                sum_elo += p->get_phenotype()->get_ratings();
                min_elo = std::min(min_elo, p->get_phenotype()->get_ratings());
                max_elo = std::max(max_elo, p->get_phenotype()->get_ratings());
            }
        printf("generation %lu (elo min=%d max=%d avg=%.02f)\n",
               eng.eng().current_generation_number(),
               min_elo, max_elo, (double)sum_elo / total_size);
        
        eng.write_population_old_json();
        eng.write_rsf();
        eng.new_generation();
        eng.write_population_new_json();
        eng.finalize_generation();
    }
    
    return 0;
}
