#include <fstream>
#include <cstring>

#include <gothello/rsf.h>
#include <gothello/strategy.h>
#include <gothello/scoring.h>
#include <gothello/player.h>
#include <strategy/best_scoring.h>
#include <strategy/scorings.h>

namespace gothello {

static void write_weighted_quater(std::ofstream &ofs,
                                  [[gnu::unused]] std::string filename,
                                  std::shared_ptr<player> s,
                                  std::string desc) {
    ofs.write("WQ", 2);
    ofs.write(desc.c_str(), 2);

    const genetics::genotype &g = s->get_genotype();

    std::size_t buf_sz = (g.size() + 7) / 8;
    char buf[buf_sz];
    std::memset(buf, 0, sizeof(buf));
    for (std::size_t i = 0; i != buf_sz; i++)
        for (std::size_t j = 0; j != 8 && (8 * i + j) != g.size(); j++)
            buf[i] |= (g[8 * i + j] << j);

    std::size_t sz = g.size();
    ofs.write(reinterpret_cast<char*>(&sz), sizeof(std::size_t));
    ofs.write(buf, buf_sz);
}

static void write_with_scoring(std::ofstream &ofs,
                               [[gnu::unused]] std::string filename,
                               std::shared_ptr<player> s,
                               std::string sdesc,
                               std::string desc) {
    ofs.write(sdesc.c_str(), 2);

    if (desc.substr(0, 2) == "WQ")
        write_weighted_quater(ofs, filename, s, desc.substr(2));
    else
        throw rsf_exception(".rsf export is unimplemented for " + desc + "scorings.");
}

void write_rsf(std::string filename, std::shared_ptr<player> s) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        throw rsf_exception("Could not open file " + filename + " for reading");

    std::string desc = s->get_phenotype()->description();

    if (desc.substr(0, 2) == "RN") {
        ofs.write(desc.c_str(), 2);
    } else if (desc.substr(0, 2) == "BS") {
        write_with_scoring(ofs, filename, s, desc.substr(0, 2), desc.substr(2));
    } else
        throw rsf_exception(".rsf export is unimplemented for " + desc + " strategies.");

    ofs.close();
}

static inline void read_bytes(char *buf, std::size_t num, std::string filename, std::ifstream &ifs) {
    if ((std::size_t)ifs.readsome(buf, num) != num)
        throw rsf_exception("File " + filename + " is corrupted.");
}

static genetics::genotype read_genotype(std::string filename, std::ifstream &ifs) {
    std::size_t size;
    read_bytes(reinterpret_cast<char*>(&size), sizeof(std::size_t), filename, ifs);

    std::size_t buf_sz = (size + 7) / 8;
    char buf[buf_sz];
    read_bytes(buf, buf_sz, filename, ifs);

    genetics::genotype g(size);
    for (std::size_t i = 0; i != buf_sz; i++)
        for (std::size_t j = 0; j != 8 && (8 * i + j) != size; j++)
            g[8 * i + j] = (buf[i] >> j) & 1;

    return g;
}

template<template<typename> class strategy_t,
         class score_type>
std::shared_ptr<player> read_weighted_quater_genotype(std::string filename, std::ifstream &ifs) {
    typedef reversi::scoring::weighted_quater<score_type> scoring_t;
    typedef gothello::scoring::weighted_quater<score_type> scoring_w_t;
    typedef gothello::strategy<strategy_t, scoring_w_t> strategy_w_t;
    typedef gothello::player_phenotype<strategy_w_t> phenotype_t;

    genetics::genotype g = read_genotype(filename, ifs);
    scoring_w_t scoring_w(g);
    scoring_t scoring = scoring_w.wrapped();
    strategy_w_t strategy(scoring);
    phenotype_ptr phenotype = std::make_shared<phenotype_t>(strategy);
    return std::make_shared<player>(g, phenotype);
}

template<template<typename> class strategy_t>
std::shared_ptr<player> read_weighted_quater(std::string filename, std::ifstream &ifs) {
    char type_desc[3];
    read_bytes(type_desc, 2, filename, ifs);
    type_desc[2] = '\0';

    if (std::strcmp(type_desc, "DF") == 0)
        return read_weighted_quater_genotype<strategy_t, double>(filename, ifs);
    else if (std::strcmp(type_desc, "ID") == 0)
        return read_weighted_quater_genotype<strategy_t, int>(filename, ifs);
    else
        throw rsf_exception("File " + filename + " is corrupted.");
}

template<template<typename> class strategy_t>
std::shared_ptr<player> read_scoring(std::string filename, std::ifstream &ifs) {
    char scoring_desc[3];
    read_bytes(scoring_desc, 2, filename, ifs);
    scoring_desc[2] = '\0';

    if (std::strcmp(scoring_desc, "WQ") == 0)
        return read_weighted_quater<strategy_t>(filename, ifs);
    else
        throw rsf_exception("File " + filename + " is corrupted.");
}

std::shared_ptr<player> read_rsf([[gnu::unused]] std::string filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs)
        throw rsf_exception("Could not open file " + filename + " for reading.");

    char strategy_desc[3];
    read_bytes(strategy_desc, 2, filename, ifs);
    strategy_desc[2] = '\0';

    if (std::strcmp(strategy_desc, "RN") == 0)
        throw rsf_exception("Reading random strategies from .rsf file is unimplemented :(");
    else if (std::strcmp(strategy_desc, "BS") == 0)
        return read_scoring<reversi::player::best_scoring>(filename, ifs);
    else
        throw rsf_exception("File " + filename + " is corrupted.");
}

}
