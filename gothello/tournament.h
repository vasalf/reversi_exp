#pragma once

#include <vector>
#include <string>
#include <exception>

#include <reversi/game.h>
#include <genetics/genetics.h>
#include <gothello/player.h>
#include <gothello/elo.h>

namespace gothello {

class rgf_exception : public std::exception {
public:
    rgf_exception(std::string reason)
        : reason_(reason) {}
    virtual ~rgf_exception() = default;

    const char *what() const noexcept override {
        return reason_.c_str();
    }
private:
    std::string reason_;
};

struct game_info {
    reversi::game_record record;
    std::shared_ptr<player> black;
    std::shared_ptr<player> white;
    reversi::field end_field;
    game_info(std::shared_ptr<player> b, std::shared_ptr<player> w, const reversi::game &game)
        : record(game.record()), black(b), white(w), end_field(game.current_position()) {}
    virtual ~game_info() = default;
    
    void write_rgf(std::string filename) const;
};
    
class tournament : public std::vector<std::shared_ptr<player> > {
    std::vector<game_info> games_;
    std::vector<elo_t> start_elo_;
    
public:
    tournament() : std::vector<std::shared_ptr<player> >() {}
    virtual ~tournament() = default;

    void prepare();
    void play();
    void write_games(std::string dir_name) const;
    void write_elo_changes() const;
};

}
