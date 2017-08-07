#include <gothello/elo.h>
#include <cmath>
#include <cassert>

namespace gothello {

elo_t new_ratings(elo_t p, elo_t a, score_t win, score_t draw, score_t result) {
    (void)draw;
    assert(2 * draw == win);

    double expectation = win / (1 + std::pow(10.0, (double)(a - p) / 400.0));
    return p + 25 * (result - expectation);
}

}
