#include <strategy/random.h>

namespace reversi {
namespace player {

std::pair<int, int> random::make_turn(const field &f) {
    auto ts = f.possible_turns(me());
    std::uniform_int_distribution<int> dist(0, ts.size() -1 );
    return ts[dist(rnd)];
}

}
}
