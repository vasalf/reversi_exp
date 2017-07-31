#pragma once

#include <array>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <vector>

namespace reversi {

const std::size_t field_size = 8;

class field {
    const int dx[8];
    const int dy[8];
    
    std::array<std::array<char, field_size>, field_size> field_;
    int score_w_ = 2, score_b_ = 2;
    char turn_ = 'b';
    
    
    inline void set_field(std::size_t i, std::size_t j, char c) {
        if (field_[i][j] == 'w')
            --score_w_;
        if (field_[i][j] == 'b')
            --score_b_;
        if (c == 'w')
            ++score_w_;
        if (c == 'b')
            ++score_b_;
        
        field_[i][j] = c;
    }

 public:
    field()
        : dx{-1, -1, -1,  0, 0,  1, 1, 1},
          dy{-1,  0,  1, -1, 1, -1, 0, 1} {
        for (std::size_t i = 0; i != field_size; i++)
            std::fill(field_[i].begin(), field_[i].end(), '.');
        int mx = field_size / 2 - 1, my = field_size / 2 - 1;
        field_[mx][my] = 'w';
        field_[mx + 1][my + 1] = 'w';
        field_[mx][my + 1] = 'b';
        field_[mx + 1][my] = 'b';
    }

    std::pair<int, int> score() const {
        return std::make_pair(score_b_, score_w_);
    }

    const std::array<char, field_size>& operator[](std::size_t i) const {
        return field_[i];
    }

    void make_turn(int i, int j, char c);
    bool can_turn(int i, int j, char c);
    std::vector<std::pair<int, int> > possible_turns(char c);
};
    
}
