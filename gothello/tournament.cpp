#include <fstream>
#include <sstream>

#include <gothello/tournament.h>
#include <gothello/elo.h>
#include <reversi/player/player.h>
#include <reversi/game_controller.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace gothello {

template<class integral>
static inline void write_integral(integral s, std::ofstream &ofs) {
    ofs.write(reinterpret_cast<char *>(&s), sizeof(integral));
}

void game_info::write_rgf(std::ofstream &ofs) const {
    write_integral<std::size_t>(black->get_phenotype()->name().size(), ofs);
    ofs.write(black->get_phenotype()->name().c_str(), black->get_phenotype()->name().size());
    write_integral<std::size_t>(white->get_phenotype()->name().size(), ofs);
    ofs.write(white->get_phenotype()->name().c_str(), white->get_phenotype()->name().size());

    write_integral<std::size_t>(record.size(), ofs);
    for (std::pair<int, int> p : record) {
        write_integral<char>(p.first, ofs);
        write_integral<char>(p.second, ofs);
    }
}

void tournament::prepare() {}

const score_t win_score = 2;
const score_t draw_score = 1;
const score_t lose_score = 0;

void tournament::play() {
    scores_.assign(this->size(), 0);
    start_elo_.resize(this->size());
    for (std::size_t i = 0; i != size(); i++)
        start_elo_[i] = (*(begin() + i))->get_phenotype()->get_ratings();
    
    for (std::size_t i = 0; i != size(); i++) {
        for (std::size_t j = 0; j != size(); j++) {
            if (i != j) {
                std::shared_ptr<reversi::player::player> black = (*(begin() + i))->get_phenotype()->get_strategy('b');
                std::shared_ptr<reversi::player::player> white = (*(begin() + j))->get_phenotype()->get_strategy('w');

                reversi::game_controller gc(*black, *white);
                while (!gc.get_game().end())
                    gc.next_turn();
                games_.push_back(game_info(*(begin() + i), *(begin() + j), gc.get_game()));

                score_t score_black;
                score_t score_white;
                if (gc.get_game().current_position().score().first > gc.get_game().current_position().score().second) {
                    score_black = win_score;
                    score_white = lose_score;
                } else if (gc.get_game().current_position().score().first == gc.get_game().current_position().score().second) {
                    score_black = draw_score;
                    score_white = draw_score;
                } else {
                    score_black = lose_score;
                    score_white = win_score;
                }

                elo_t elo_black = new_ratings((*(begin() + i))->get_phenotype()->get_ratings(),
                                              (*(begin() + j))->get_phenotype()->get_ratings(),
                                              win_score, draw_score, score_black);
                elo_t elo_white = new_ratings((*(begin() + j))->get_phenotype()->get_ratings(),
                                              (*(begin() + i))->get_phenotype()->get_ratings(),
                                              win_score, draw_score, score_white);
                (*(begin() + i))->get_phenotype()->get_ratings() = elo_black;
                (*(begin() + j))->get_phenotype()->get_ratings() = elo_white;

                scores_[i] += score_black;
                scores_[j] += score_white;
            }
        }
    }
}

void tournament::write_games(std::string filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        throw rgf_exception("Could not open file " + filename + " for writing.");
    
    for (const game_info &gi : games_) {
        gi.write_rgf(ofs);
    }

    ofs.close();
}

struct player_info {
    std::string name;
    score_t score;
    std::size_t place;
    elo_t start_elo;
    elo_t end_elo;

    player_info() {}

    template<class writer_t>
    void write_json(writer_t &writer) {
        writer.StartObject();
        writer.Key("player");
        writer.String(name.c_str());
        writer.Key("score");
        writer.Int(score);
        writer.Key("place");
        writer.Int(place);
        writer.Key("start_elo");
        writer.Int(start_elo);
        writer.Key("end_elo");
        writer.Int(end_elo);
        writer.EndObject();
    }
};

void tournament::write_json(std::string filename) const {
    std::vector<std::pair<score_t, std::size_t> > result;
    for (std::size_t i = 0; i != this->size(); i++)
        result.push_back(std::make_pair(scores_[i], i));
    std::sort(result.rbegin(), result.rend());

    std::vector<std::size_t> place(this->size());
    for (std::size_t i = 0; i != this->size(); i++)
        place[result[i].second] = i + 1;
    
    std::vector<player_info> data(this->size());
    for (std::size_t i = 0; i != this->size(); i++) {
        data[i].name = (*(begin() + i))->get_phenotype()->name();
        data[i].score = scores_[i];
        data[i].place = place[i];
        data[i].start_elo = start_elo_[i];
        data[i].end_elo = (*(begin() + i))->get_phenotype()->get_ratings();
    }
    
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("tournament");
    writer.StartArray();
    for (std::size_t i = 0; i != this->size(); i++)
        data[i].write_json(writer);
    writer.EndArray();
    writer.EndObject();

    std::ofstream ofs(filename);
    if (!ofs)
        throw rgf_exception("Could not open file " + filename + " for writing.");
    ofs << s.GetString();
    ofs.close();
}
    
void tournament::write_elo_changes() const {
    for (std::size_t i = 0; i != size(); i++) {
        elo_t new_elo = (*(begin() + i))->get_phenotype()->get_ratings();
        (*(begin() + i))->get_phenotype()->ratings_history().push_back(new_elo);
    }
}

}
