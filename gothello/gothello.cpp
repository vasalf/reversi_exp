#include <sstream>
#include <fstream>

#include <gothello/gothello.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace gothello {

void genetics_engine::play_tournaments(std::string writedir) {
    std::vector<std::shared_ptr<player> > players;
    for (auto &p : populations_)
        for (auto &q : *p)
            players.push_back(std::dynamic_pointer_cast<player, genetics::basic_individual<phenotype_ptr> >(q));
    for (std::size_t nt = 0; nt < config.ntournaments; nt++) {
        std::sort(players.begin(), players.end(), [](const auto &a, const auto &b) -> bool {
                return a->get_phenotype()->get_ratings() > b->get_phenotype()->get_ratings();
            });
        std::size_t ctn = 1;
        for (std::size_t i = 0; i < players.size(); i += config.tournament_size) {
            std::size_t j = i + config.tournament_size;
            if (j > players.size())
                j = players.size();

            tournament t;
            std::copy(players.begin() + i, players.begin() + j,
                      std::back_inserter(t));

            std::ostringstream tname;
            tname << writedir << "/TN" << nt + 1 << "T" << ctn;
            
            t.prepare();
            t.play();
            t.write_games(tname.str() + ".rgf");
            t.write_json(tname.str() + ".json");
            t.write_elo_changes();
            ctn++;
        }
    }
}

void genetics_engine::new_generation() {
    ++generation_number_;
    for (auto &p : populations_) {
        if (p->config.evolving) {
            p->new_generation(*this);
        }
    }
}

void genetics_engine::write_json(std::string filename) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("populations");
    writer.StartArray();
    for (const auto &p : populations_)
        p->write_json(writer);
    writer.EndArray();
    writer.EndObject();

    std::ofstream ofs(filename);
    if (!ofs)
        throw json_file_exception("Could not open file " + filename + " for writing.");
    ofs << s.GetString();
    ofs.close();
}

}
