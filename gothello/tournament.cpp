#include <gothello/tournament.h>
#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <reversi/game_controller.h>

namespace gothello {

const score_t win_score = 2;
const score_t draw_score = 1;
const score_t lose_score = 0;

void tournament::play() {
    for (std::size_t i = 0; i != size(); i++) {
        for (std::size_t j = i + 1; j != size(); j++) {
            std::shared_ptr<reversi::player::player> black = (*(begin() + i))->get_phenotype()->get_strategy();
            std::shared_ptr<reversi::player::player> white = (*(begin() + j))->get_phenotype()->get_strategy();

            reversi::game_controller gc(*black, *white);
            while (!gc.get_game().end())
                gc.next_turn();

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
