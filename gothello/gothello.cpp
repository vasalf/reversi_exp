#include <sstream>
#include <fstream>
#include <cstdlib>
#include <random>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <gothello/gothello.h>
#include <gothello/rsf.h>
#include <gothello/strategy.h>
#include <gothello/scoring.h>
#include <strategy/best_scoring.h>
#include <strategy/scorings.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>


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
    for (auto &storer : populations_) {
        storer->clear_ratings_history();
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

static std::string get_generation_desc(std::size_t gen_no) {
    char buf[20];
    sprintf(buf, "%04lu", gen_no);
    return buf;
}

static void check_directory(std::string path) {
    struct stat st;
    int status = stat(path.c_str(), &st);
    if (status == -1 && errno == ENOENT) {
        if (mkdir(path.c_str(), 0755) == -1) {
            char err_buf[500];
            std::sprintf(err_buf, "Could not mkdir %s: %m", path.c_str());
            throw gothello_exception(err_buf);
        }
    } else if (status == -1) {
        char err_buf[500];
        std::sprintf(err_buf, "Could not stat %s: %m", path.c_str());
        throw gothello_exception(err_buf);
    } else if (!S_ISDIR(st.st_mode))
        throw gothello_exception("Path " + path + " is not a directory.");
}

engine::launch_filesystem_manager::launch_filesystem_manager(std::string workdir)
    : workdir_(workdir) {
    while (workdir_.back() == '/')
        workdir_.pop_back();
    check_directory(workdir_);
}

static void check_population_directory(std::string workdir, std::size_t gen_no) {
    std::ostringstream path;
    path << workdir
         << "/generation" << get_generation_desc(gen_no);
    check_directory(path.str());
}

std::string engine::launch_filesystem_manager::tournaments_path(std::size_t gen_no) const {
    check_population_directory(workdir_, gen_no);
    
    std::ostringstream path;
    path << workdir_
         << "/generation" << get_generation_desc(gen_no)
         << "/tournaments";

    check_directory(path.str());
    return path.str();
}

std::string engine::launch_filesystem_manager::population_old_json_path(std::size_t gen_no) const {
    check_population_directory(workdir_, gen_no);

    std::ostringstream path;
    path << workdir_
         << "/generation" << get_generation_desc(gen_no)
         << "/populations_old.json";

    return path.str();
}

std::string engine::launch_filesystem_manager::population_new_json_path(std::size_t gen_no) const {
    check_population_directory(workdir_, gen_no);

    std::ostringstream path;
    path << workdir_
         << "/generation" << get_generation_desc(gen_no)
         << "/populations_new.json";

    return path.str();    
}

std::string engine::launch_filesystem_manager::rsf_path(std::size_t gen_no, std::shared_ptr<player> t_player) const {
    check_population_directory(workdir_, gen_no);

    std::ostringstream path;
    path << workdir_
         << "/generation" << get_generation_desc(gen_no)
         << "/strategies";
    check_directory(path.str());
    path << "/" << t_player->get_phenotype()->name() << ".rsf";

    return path.str();
}

std::string engine::launch_filesystem_manager::finalizing_file_path(std::size_t gen_no) const {
    check_population_directory(workdir_, gen_no);

    std::ostringstream path;
    path << workdir_
         << "/generation" << get_generation_desc(gen_no)
         << "/ready";

    return path.str();
}

static std::string read_whole_file(std::string path) {
    std::ifstream ifs(path, std::ios::ate);
    if (!ifs)
        throw json_file_exception("Could not open file " + path + " for reading.");

    std::string res;
    res.resize((std::size_t)ifs.tellg() + 1);
    ifs.seekg(std::ios::beg);
    ifs.read(const_cast<char*>(res.data()), res.size());

    ifs.close();
    return res;
}

template<class doc_t>
void verify_member(doc_t &conf, const char *member) {
    if (!conf.HasMember(member))
        throw json_file_exception(std::string("JSON: required property ") + member + " is missing.");
}

static void verify_member_type(bool ok, const char *member) {
    if (!ok)
        throw json_file_exception(std::string("JSON: property ") + member + " has a wrong type.");
}

template<class strategy_desc_t, class doc_t>
void add_population_of_strategy(const doc_t &population_conf, genetics_engine &eng) 
{
    typedef typename strategy_desc_t::generator_t player_gen_t;
    typedef typename strategy_desc_t::factory_t   factory_t;

    player_gen_t gen;
    auto p = std::make_shared<genetics::population<phenotype_ptr, factory_t> >();
    for (std::size_t i = 0; i != population_conf["size"].GetUint(); i++) {
        std::shared_ptr<player> individual = gen();
        p->push_back(individual);
    }

    eng.add_population(p, population_conf["name"].GetString());
    auto storer = (*std::prev(eng.end()));
    if (population_conf.HasMember("evolving")
        && !population_conf["evolving"].GetBool()) {
        storer->config.evolving = false;
    } else {
        storer->config.evolving = true;
        storer->config.alivers = population_conf["alivers"].GetUint();
        storer->config.mutation_frequency = population_conf["mutation_frequency"].GetDouble();
    }
}

template<class score_type>
struct weighted_quater_generator {
    typedef reversi::scoring::weighted_quater<score_type> scoring_t;
    typedef gothello::scoring::weighted_quater<score_type> scoring_w_t;
    
    scoring_t operator()() const {
        scoring_t ret;
        ret.template gen_random<std::mt19937>();
        return ret;
    }
};

template<class scoring_generator>
struct best_scoring_generator {
    typedef gothello::strategy<reversi::player::best_scoring,
                               typename scoring_generator::scoring_w_t> strategy_t;
    typedef gothello::player_phenotype<strategy_t> phenotype_t;

    scoring_generator gen;
    
    best_scoring_generator() {}
    
    std::shared_ptr<player> operator()() const {
        typename scoring_generator::scoring_t scoring = gen();
        strategy_t strategy(scoring);
        phenotype_ptr phenotype = std::make_shared<phenotype_t>(strategy);
        genetics::genotype genotype = strategy.get_genotype();
        return std::make_shared<player>(genotype, phenotype);
    }
};

template<class scoring_generator>
struct best_scoring_desc {
    typedef best_scoring_generator<scoring_generator> generator_t;
    typedef player_factory<typename generator_t::strategy_t> factory_t;
};

template<template<typename> class strategy_desc_t,
         class doc_t, class score_t>
void add_weighted_quater_population_type(const doc_t &population_conf, genetics_engine &eng) {
    typedef weighted_quater_generator<score_t> sc_generator_t;
    typedef strategy_desc_t<sc_generator_t> st_desc_t;

    add_population_of_strategy<st_desc_t, doc_t>(population_conf, eng);
}

template<template<typename> class strategy_desc_t,
         class doc_t>
void add_weighted_quater_population(const doc_t &population_conf, genetics_engine &eng) {
    verify_member(population_conf, "score_type");
    verify_member_type(population_conf["score_type"].IsString(), "score_type");

    std::string score_type = population_conf["score_type"].GetString();
    if (score_type == "double") {
        add_weighted_quater_population_type<strategy_desc_t, doc_t, double>(population_conf, eng);
    } else if (score_type == "int" || score_type == "int32") {
        add_weighted_quater_population_type<strategy_desc_t, doc_t, int>(population_conf, eng);
    } else if (score_type == "int64") {
        add_weighted_quater_population_type<strategy_desc_t, doc_t, long long>(population_conf, eng);
    } else {
        throw gothello_exception("Unknown weighted_quater score type: " + score_type);
    }
}

template<template<typename> class strategy_desc_t,
         class doc_t>
void add_scoring_population(const doc_t &population_conf, genetics_engine &eng) {
    verify_member(population_conf, "scoring");
    verify_member_type(population_conf["scoring"].IsString(), "scoring");

    std::string scoring = population_conf["scoring"].GetString();
    if (scoring == "weighted_quater") {
        add_weighted_quater_population<strategy_desc_t, doc_t>(population_conf, eng);
    } else {
        throw gothello_exception("Unknown scoring: " + scoring);
    }
}
    
template<class doc_t>
void add_population(const doc_t &population_conf, genetics_engine &eng) {
    verify_member_type(population_conf.IsObject(), "<one of the populations>");
    verify_member(population_conf, "strategy");
    verify_member_type(population_conf["strategy"].IsString(), "strategy");
    verify_member(population_conf, "name");
    verify_member_type(population_conf["name"].IsString(), "name");
    verify_member(population_conf, "size");
    verify_member_type(population_conf["size"].IsUint(), "size");

    std::string strategy = population_conf["strategy"].GetString();
    bool evolving = true;

    if (population_conf.HasMember("evolving")) {
        verify_member_type(population_conf["evolving"].IsBool(), "evolving");
        if (!population_conf["evolving"].GetBool())
            evolving = false;
    }
    
    if (evolving) {
        verify_member(population_conf, "alivers");
        verify_member_type(population_conf["alivers"].IsUint(), "alivers");
        verify_member(population_conf, "mutation_frequency");
        verify_member_type(population_conf["mutation_frequency"].IsDouble(), "mutation_frequency");
    }

    if (strategy == "best_scoring") {
        add_scoring_population<best_scoring_desc, doc_t>(population_conf, eng);
    } else
        throw gothello_exception("Unknown strategy: " + strategy);
}

engine::engine(std::string launch_params_file, bool continue_c) {

    if (continue_c)
        throw gothello_exception("Continuation is not yet implemented :(");
    
    std::string json = read_whole_file(launch_params_file);

    rapidjson::Document conf;
    conf.Parse(json.c_str());

    if (!conf.IsObject())
        throw json_file_exception("JSON: conf root is not an object");

    verify_member(conf, "workdir");
    verify_member_type(conf["workdir"].IsString(), "workdir");
    config.workdir = conf["workdir"].GetString();
    fs_ = engine::launch_filesystem_manager(config.workdir);

    verify_member(conf, "ntournaments");
    verify_member_type(conf["ntournaments"].IsUint(), "ntournaments");
    eng_.config.ntournaments = conf["ntournaments"].GetUint();

    verify_member(conf, "tournament_size");
    verify_member_type(conf["tournament_size"].IsUint(), "tournament_size");
    eng_.config.tournament_size = conf["tournament_size"].GetUint();

    verify_member(conf, "start_elo");
    verify_member_type(conf["start_elo"].IsInt(), "start_elo");
    eng_.config.start_elo = conf["start_elo"].GetInt();

    verify_member(conf, "populations");
    verify_member_type(conf["populations"].IsArray(), "populations");
    const rapidjson::Value &populations = conf["populations"];

    for (rapidjson::SizeType i = 0; i < populations.Size(); i++) {
        verify_member_type(populations[i].IsObject(), "(one of the populations)");
        const rapidjson::Value &p = populations[i];

        add_population(p, eng_);
    }
}

void engine::write_rsf() {
    for (auto &storer : eng_) {
        for (auto &p : *storer) {
            auto q = std::dynamic_pointer_cast<player, genetics::basic_individual<phenotype_ptr> >(p);
            std::string filename = fs_.rsf_path(eng_.current_generation_number(), q);
            gothello::write_rsf(filename, q);
        }
    }
}

void engine::finalize_generation() {
    std::string filename = fs_.finalizing_file_path(eng_.current_generation_number());
    std::ofstream ofs(fs_.finalizing_file_path(eng_.current_generation_number()));
    if (!ofs)
        throw gothello_exception("Could not open file " + filename + " for writing.");

    ofs << "1";
    ofs.close();
}

}
