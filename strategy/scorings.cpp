#include <strategy/scorings.h>

namespace reversi {
namespace scoring {

int by_score::operator()(const field& field) {
    return field.score().first - field.score().second;
}

}
}
