#include <gothello/gothello.h>

namespace gothello {

void genetics_engine::play_tournaments() {
    std::vector<genetics::basic_individual_ptr<phenotype_ptr> > players;
    for (auto &p : populations_)
        for (auto &q : *p)
            players.push_back(q);
    for (std::size_t nt = 0; nt < config.ntournaments; nt++) {
        std::sort(players.begin(), players.end(), [](const auto &a, const auto &b) -> bool {
                return a->get_phenotype()->get_ratings() > b->get_phenotype()->get_ratings();
            });
        for (std::size_t i = 0; i < players.size(); i += config.tournament_size) {
            std::size_t j = i + config.tournament_size;
            if (j > players.size())
                j = players.size();

            tournament t;
            std::copy(players.begin() + i, players.begin() + j,
                      std::back_inserter(t));
            t.play();
        }
    }
}

void genetics_engine::new_generation() {
    for (auto &p : populations_) {
        auto it = p->new_generation(*this);
        while (it != p->end()) {
            (*it)->get_phenotype()->get_ratings() = 1500;
            it++;
        }
    }
}

}
