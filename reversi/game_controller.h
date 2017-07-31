#pragma once

#include <reversi/game.h>
#include <reversi/player/player.h>

namespace reversi {

class game_controller {
    game game_;
    player::player &black_;
    player::player &white_;
 public:
    game_controller(player::player &black,
                    player::player &white)
        : black_(black), white_(white) {}

    const game& get_game() const {
        return game_;
    }

    void next_turn() {
        std::pair<int, int> t;
        if (game_.turns() == 'b')
            t = black_.make_turn(game_.current_position());
        else
            t = white_.make_turn(game_.current_position());
        game_.make_turn(t.first, t.second);
    }
};
    
}
