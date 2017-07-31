#pragma once

#include <vector>

#include <reversi/field.h>

namespace reversi {

using game_record = std::vector<std::pair<int, int> >;

class game {
   field current_;
   game_record record_;
   char turn_ = 'b';

public:
   game() {}

   char turns() const {
       return turn_;
   }

   const game_record& record() const {
       return record_;
   }

   const field &current_position() const {
       return current_;
   }

   void make_turn(int i, int j) {
       current_.make_turn(i, j, turn_);
       turn_ = (turn_ == 'w' ? 'b' : 'w');
   }

   bool end() const {
       return current_.score().first + current_.score().second == 64
           || (current_.possible_turns('w') == std::vector<std::pair<int, int> >({ std::make_pair(-1, -1) })
               && current_.possible_turns('b') == std::vector<std::pair<int, int> >({ std::make_pair(-1, -1) }));
   }
};

}
