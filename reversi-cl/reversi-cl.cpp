#include <iostream>
#include <string>
#include <sstream>
#include <cstddef>

#include <reversi/field.h>

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
    reversi::field f;
    char c = 'b';
    while (true) {
        print_field(f);
        for (auto p : f.possible_turns(c)) {
            std::cout << "(" << p.first + 1 << "," << p.second + 1 << ") ";
        }
        std::cout << std::endl;
        int i, j;
        std::cin >> i >> j;
        i--; j--;

        if (f.can_turn(i, j, c)) {
            f.make_turn(i, j, c);
            c = (c == 'w' ? 'b' : 'w');
        } else
            std::cout << "No." << std::endl;
    }
    
}
