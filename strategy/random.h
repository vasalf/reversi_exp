#pragma once

#include <reversi/field.h>
#include <reversi/player/player.h>

#include <random>

namespace reversi {
namespace player {

class random : public player {
    std::mt19937 rnd;
public:
    random(char who)
        : player(who), rnd(std::random_device()()) {}
    virtual ~random() = default;

    virtual std::pair<int, int> make_turn(const field &f) override;
};

}
}
