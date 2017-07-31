#include <iostream>
#include <string>
#include <sstream>
#include <cstddef>
#include <memory>

#include <reversi/player/player.h>
#include <reversi/field.h>
#include <reversi/game.h>

namespace reversi {
    namespace player {

        class human : public player {
        public:
            human(char who)
                : player(who) {}
            virtual ~human() = default;

            virtual std::pair<int, int> make_turn(const field &f) override {
                int i, j;
                std::cin >> i >> j;
                i--; j--;
                while (!f.can_turn(i, j, me())) {
                    std::cout << "No." << std::endl;
                    std::cin >> i >> j;
                    i--; j--;
                }
                return std::make_pair(i, j);
            }
        };
        
    }
}

std::string sfy(int len, std::size_t n) {
    std::ostringstream ss;
    ss << n;

    std::ostringstream ans;
    for (int i = ss.str().size(); i < len; i++)
        ans << ' ';
    ans << n;

    return ans.str();
}

void print_field(const reversi::field & field) {
    std::cout << "   ";
    for (std::size_t i = 1; i <= reversi::field_size; i++)
        std::cout << i;
    std::cout << std::endl;

    for (std::size_t i = 0; i < reversi::field_size; i++) {
        std::cout << sfy(3, i + 1);
        for (std::size_t j = 0; j < reversi::field_size; j++)
            switch (field[i][j]) {
            case 'w':
                std::cout << '*';
                break;
            case 'b':
                std::cout << '#';
                break;
            default:
                std::cout << field[i][j];
            }
        std::cout << std::endl;
    }
    std::cout << field.score().first << ":" << field.score().second << std::endl;
}

int main() {
    reversi::game g;

    std::unique_ptr<reversi::player::player> black = std::make_unique<reversi::player::human>('b');
    std::unique_ptr<reversi::player::player> white = std::make_unique<reversi::player::human>('w');
    
    while (!g.end()) {
        print_field(g.current_position());
        for (auto p : g.current_position().possible_turns(g.turns())) {
            std::cout << "(" << p.first + 1 << "," << p.second + 1 << ") ";
        }
        std::cout << std::endl;

        std::pair<int, int> t;
        if (g.turns() == 'b')
            t = black->make_turn(g.current_position());
        else
            t = white->make_turn(g.current_position());
        g.make_turn(t.first, t.second);
    }
    print_field(g.current_position());

    return 0;
}
