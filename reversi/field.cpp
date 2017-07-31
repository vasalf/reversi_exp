#include <reversi/field.h>

namespace reversi {

inline bool ok(int i, int j) {
    return 0 <= i && i < field_size && 0 <= j && j < field_size;
}
    
void field::make_turn(int i, int j, char c) {
    char other = (c == 'w' ? 'b' : 'w');
    
    for (int d = 0; d < 8; d++) {
        int cx = i + dx[d], cy = j + dy[d];
        while (ok(cx, cy) && field_[cx][cy] == other) {
            cx += dx[d];
            cy += dy[d];
        }
        if (ok(cx, cy) && field_[cx][cy] == c) {
            cx = i + dx[d], cy = j + dy[d];
            while (ok(cx, cy) && field_[cx][cy] == other) {
                set_field(cx, cy, c);
                cx += dx[d];
                cy += dy[d];
            }            
        }
    }
    set_field(i, j, c);
}
    
bool field::can_turn(int i, int j, char c) {
    if (!ok(i, j))
        return false;
    if (field_[i][j] != '.')
        return false;
    
    char other = (c == 'w' ? 'b' : 'w');
    
    for (int d = 0; d < 8; d++) {
        int cx = i + dx[d], cy = j + dy[d];
        while (ok(cx, cy) && field_[cx][cy] == other) {
            cx += dx[d];
            cy += dy[d];
        }
        if (ok(cx, cy) && (cx != i + dx[d] || cy != j + dy[d]) && field_[cx][cy] == c)
            return true;
    }

    return false;
}

std::vector<std::pair<int, int> > field::possible_turns(char c) {
    std::vector<std::pair<int, int> > res;
    for (int i = 0; i < field_size; i++)
        for (int j = 0; j < field_size; j++)
            if (can_turn(i, j, c))
                res.push_back(std::make_pair(i, j));
    return res;
}

}

