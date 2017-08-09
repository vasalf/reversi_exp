#pragma once

#include <reversi/player/player.h>

namespace reversi {
namespace player {

template<class scoring_scheme>
class best_scoring : public player {
    scoring_scheme scoring_;
public:
    best_scoring(char who, scoring_scheme scoring = scoring_scheme())
        : player(who), scoring_(scoring) {}
    virtual ~best_scoring() = default;

    virtual std::pair<int, int> make_turn(const field &f) override {
        field copy = f;
        auto turns = copy.possible_turns(me());
        return *max_element(turns.begin(), turns.end(), [this, &copy](std::pair<int, int> a, std::pair<int, int> b) {
                field fa = copy; fa.make_turn(a.first, a.second, me());
                field fb = copy; fb.make_turn(b.first, b.second, me());
                if (me() == 'b')
                    return scoring_(fa) < scoring_(fb);
                else
                    return scoring_(fa) > scoring_(fb);
            });
    }

    static std::string description() { return "BS" + scoring_scheme::description(); }
};

}
}
