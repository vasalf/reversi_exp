#pragma once

namespace gothello {

typedef int score_t;
typedef int elo_t;

elo_t new_ratings(elo_t p, elo_t a, score_t win, score_t draw, score_t result);

}
