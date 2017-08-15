#include <fstream>
#include <sstream>

#include <gothello/tournament.h>
#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <reversi/game_controller.h>

namespace gothello {

template<class integral>
static inline void write_integral(integral s, std::ofstream &ofs) {
    ofs.write(reinterpret_cast<char *>(&s), sizeof(integral));
}

void game_info::write_rgf(std::ofstream &ofs) const {
    write_integral<std::size_t>(black->get_phenotype()->name().size(), ofs);
    ofs.write(black->get_phenotype()->name().c_str(), black->get_phenotype()->name().size());
    write_integral<std::size_t>(white->get_phenotype()->name().size(), ofs);
    ofs.write(white->get_phenotype()->name().c_str(), white->get_phenotype()->name().size());

    write_integral<std::size_t>(record.size(), ofs);
    for (std::pair<int, int> p : record) {
        write_integral<char>(p.first, ofs);
        write_integral<char>(p.second, ofs);
    }
}

void tournament::prepare() {}

const score_t win_score = 2;
const score_t draw_score = 1;
const score_t lose_score = 0;

void tournament::play() {
    for (std::size_t i = 0; i != size(); i++) {
        for (std::size_t j = 0; j != size(); j++) {
            if (i != j) {
                std::shared_ptr<reversi::player::player> black = (*(begin() + i))->get_phenotype()->get_strategy('b');
                std::shared_ptr<reversi::player::player> white = (*(begin() + j))->get_phenotype()->get_strategy('w');

                reversi::game_controller gc(*black, *white);
                while (!gc.get_game().end())
                    gc.next_turn();
                games_.push_back(game_info(*(begin() + i), *(begin() + j), gc.get_game()));

                score_t score_black;
                score_t score_white;
                if (gc.get_game().current_position().score().first > gc.get_game().current_position().score().second) {
                    score_black = win_score;
                    score_white = lose_score;
                } else if (gc.get_game().current_position().score().first == gc.get_game().current_position().score().second) {
                    score_black = draw_score;
                    score_white = draw_score;
                } else {
                    score_black = lose_score;
                    score_white = win_score;
                }

                elo_t elo_black = new_ratings((*(begin() + i))->get_phenotype()->get_ratings(),
                                              (*(begin() + j))->get_phenotype()->get_ratings(),
                                              win_score, draw_score, score_black);
                elo_t elo_white = new_ratings((*(begin() + j))->get_phenotype()->get_ratings(),
                                              (*(begin() + i))->get_phenotype()->get_ratings(),
                                              win_score, draw_score, score_white);
                (*(begin() + i))->get_phenotype()->get_ratings() = elo_black;
                (*(begin() + j))->get_phenotype()->get_ratings() = elo_white;
            }
        }
    }
}

void tournament::write_games(std::string filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        throw rgf_exception("Could not open file " + filename + " for writing.");
    
    for (const game_info &gi : games_) {
        gi.write_rgf(ofs);
    }

    ofs.close();
}

void tournament::write_elo_changes() const {
    for (std::size_t i = 0; i != size(); i++) {
        elo_t new_elo = (*(begin() + i))->get_phenotype()->get_ratings();
        (*(begin() + i))->get_phenotype()->ratings_history().push_back(new_elo);
    }
}

}
