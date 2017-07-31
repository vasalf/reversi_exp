#pragma once

#include <reversi/field.h>

namespace reversi {
namespace player {

class player {
    char who_;
protected:
    char me() const {
        return who_;
    }
public:
    player(char who)
        : who_(who) {}
    virtual ~player() = default;

    virtual std::pair<int, int> make_turn(const field &f) = 0;
};
 
}    
}
