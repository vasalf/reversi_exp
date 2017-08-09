#include <fstream>
#include <sstream>

#include <gothello/tournament.h>
#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <reversi/game_controller.h>

namespace gothello {

static inline void write_size_t(std::size_t s, std::ofstream &ofs) {
    ofs.write(reinterpret_cast<char *>(&s), sizeof(std::size_t));
}

void game_info::write_rgf(std::string filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        throw rgf_exception("Could not open file " + filename + " for writing");

    write_size_t(black->get_phenotype()->name().size(), ofs);
    ofs.write(black->get_phenotype()->name().c_str(), black->get_phenotype()->name().size());
    write_size_t(white->get_phenotype()->name().size(), ofs);
    ofs.write(white->get_phenotype()->name().c_str(), white->get_phenotype()->name().size());

    write_size_t(record.size(), ofs);
    for (std::pair<int, int> p : record) {
        write_size_t(p.first, ofs);
        write_size_t(p.second, ofs);
    }

    ofs.close();
}

void tournament::prepare() {
    for (std::shared_ptr<player> p : *this) {
        start_elo_.push_back(p->get_phenotype()->get_ratings());
    }
}

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

void tournament::write_games(std::string dir_name) const {
    std::size_t i = 1;
    for (const game_info &gi : games_) {
        std::ostringstream ss;
        ss << dir_name << "/GM" << i << ".rgf";
        gi.write_rgf(ss.str());
        i++;
    }
}

void tournament::write_elo_changes() const {
    //TODO
}

}
