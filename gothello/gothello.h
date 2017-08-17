#pragma once

#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <exception>

#include <gothello/elo.h>
#include <gothello/tournament.h>
#include <gothello/player.h>
#include <genetics/genetics.h>

namespace gothello {

class json_file_exception : public std::exception {
public:
    json_file_exception(std::string reason)
        : reason_(reason) {}
    virtual ~json_file_exception() {}

    virtual const char *what() const noexcept override {
        return reason_.c_str();
    }

private:
    std::string reason_;
};
    
class genetics_engine {
public:

    template<class writer_t>
    static void json_write_individual(writer_t &writer, genetics::basic_individual_ptr<phenotype_ptr> player) {
        writer.StartObject();
        writer.Key("name");
        writer.String(player->get_phenotype()->name().c_str());
        writer.Key("ratings_history");
        writer.StartArray();
        for (elo_t elo : player->get_phenotype()->ratings_history())
            writer.Int(elo);
        writer.EndArray();
        writer.EndObject();
    }
    
    struct {
        std::size_t ntournaments = 5;
        std::size_t tournament_size = 20;
        elo_t start_elo = 1500;
    } config;
    
    class basic_population_storer {
    public:
        struct {
            bool evolving;
            std::size_t population_size;
            std::size_t alivers;
            double mutation_frequency;
        } config;
        
        virtual ~basic_population_storer() = default;

        typedef typename std::vector<genetics::basic_individual_ptr<phenotype_ptr> >::iterator iterator;
        
        virtual iterator new_generation(const genetics_engine &eng) = 0;
        virtual std::size_t size() const = 0;
        virtual genetics::basic_individual_ptr<phenotype_ptr> operator[](std::size_t i) = 0;
        virtual iterator begin() = 0;
        virtual iterator end() = 0;
        virtual std::string description() const = 0;
        virtual const std::string &name() const = 0;
        virtual void prepare_phenotypes(const genetics_engine &eng) = 0;
        virtual void clear_ratings_history() = 0;

        template<class writer_t>
        void write_json(writer_t &writer) {
            writer.StartObject();
            writer.Key("name");
            writer.String(this->name().c_str());
            writer.Key("type");
            writer.String(this->description().c_str());
            writer.Key("individuals");
            writer.StartArray();
            for (genetics::basic_individual_ptr<phenotype_ptr> individual : *this) {
                genetics_engine::json_write_individual(writer, individual);
            }
            writer.EndArray();
            writer.EndObject();
        }
    };

    template<class strategy_t>
    class population_storer : public basic_population_storer {
        std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population_;
        std::string name_;

        void prepare_phenotypes(std::size_t gen_no, elo_t start_elo, iterator first, iterator last) {
            for (iterator jt = first; jt != last; jt++) {
                (*jt)->get_phenotype()->get_ratings() = start_elo;
                (*jt)->get_phenotype()->ratings_history() = { start_elo };

                std::ostringstream name_ss;
                name_ss << "G" << gen_no << "P" << name_ << "S" << jt - first + 1;
                (*jt)->get_phenotype()->name() = name_ss.str();
            }
        }
        
    public:
        population_storer(std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population,
                          std::string name)
            : population_(population), name_(name) {
            this->config.population_size = population_->size();
        }
        virtual ~population_storer() = default;

        typedef basic_population_storer::iterator iterator;
        
        virtual iterator new_generation(const genetics_engine &eng) override {
            player_fitness pf;
            auto it = population_->template new_generation<elo_t, player_fitness>(this->config.alivers,
                                                                                  population_->size(),
                                                                                  this->config.mutation_frequency,
                                                                                  pf);
            
            prepare_phenotypes(eng.current_generation_number(), eng.config.start_elo, it, end());

            return it;
        }
        
        virtual std::size_t size() const override {
            return population_->size();
        }

        virtual genetics::basic_individual_ptr<phenotype_ptr> operator[](std::size_t i) override {
            return population_->at(i);
        }

        virtual iterator begin() override {
            return population_->begin();
        }

        virtual iterator end() override {
            return population_->end();
        }

        virtual std::string description() const override {
            return strategy_t::description();
        }

        virtual const std::string &name() const override {
            return name_;
        }

        virtual void prepare_phenotypes(const genetics_engine &eng) override {
            prepare_phenotypes(eng.current_generation_number(), eng.config.start_elo, begin(), end());
        }

        virtual void clear_ratings_history() override {
            for (auto q : *population_) {
                q->get_phenotype()->ratings_history() = { q->get_phenotype()->get_ratings() };
            }
        }
    };
    
private:
    std::vector<std::shared_ptr<basic_population_storer> > populations_;
    std::size_t generation_number_ = 0;
    
public:
    genetics_engine() {}

    template<class strategy_t>
    void add_population(std::shared_ptr<genetics::population<phenotype_ptr, player_factory<strategy_t> > > population,
                        std::string name) {
        populations_.push_back(std::make_shared<population_storer<strategy_t> >(population, name));
        populations_.back()->prepare_phenotypes(*this);
    }
    
    void play_tournaments(std::string writedir);

    void /* Here comes the young, the */ new_generation();

    typedef std::vector<std::shared_ptr<basic_population_storer> >::const_iterator iterator;

    iterator begin() const { return populations_.begin(); }
    iterator end() const { return populations_.end(); }

    std::size_t size() const { return populations_.size(); }
    const std::shared_ptr<basic_population_storer>& operator[](std::size_t i) const { return populations_[i]; };

    std::size_t current_generation_number() const { return generation_number_; }

    void write_json(std::string filename);
};

class gothello_exception : public std::exception {
public:
    gothello_exception(std::string reason)
        : reason_(reason) {}
    virtual ~gothello_exception() {}

    virtual const char *what() const noexcept override {
        return reason_.c_str();
    }
    
private:
    std::string reason_;
};

class engine {
    class launch_filesystem_manager {
        std::string workdir_;
    public:
        launch_filesystem_manager(std::string workdir);
        launch_filesystem_manager() {}

        std::string tournaments_path(std::size_t gen_no) const;
        std::string population_old_json_path(std::size_t gen_no) const;
        std::string population_new_json_path(std::size_t gen_no) const;
        std::string rsf_path(std::size_t gen_no, std::shared_ptr<player> t_player) const;
        std::string finalizing_file_path(std::size_t gen_no) const;
    };

    genetics_engine eng_;
    launch_filesystem_manager fs_;
    
public:
    struct {
        std::string workdir;
    } config;

    engine(std::string launch_params_file, bool continue_c = false);

    genetics_engine &eng() { return eng_; }
    const genetics_engine &eng() const { return eng_; }

    void play_tournaments() {
        eng_.play_tournaments(fs_.tournaments_path(eng_.current_generation_number()));
    }
    
    void write_population_old_json() {
        eng_.write_json(fs_.population_old_json_path(eng_.current_generation_number()));
    }

    void new_generation() {
        eng_.new_generation();
    }

    void write_population_new_json() {
        eng_.write_json(fs_.population_new_json_path(eng_.current_generation_number()));
    }

    void write_rsf();
    void finalize_generation();
};
    
}
