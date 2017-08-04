#pragma once

#include <cstddef>
#include <array>
#include <string>
#include <fstream>
#include <exception>
#include <random>

#include <reversi/field.h>

namespace reversi {
namespace scoring {

struct by_score {
    int operator()(const field& field);
};

class scoring_file_exception final : public std::exception {
    std::string reason_;
public:
    scoring_file_exception(std::string reason)
        : reason_(reason) {}
    const char *what() const noexcept override {
        return reason_.c_str();
    }
};

template<typename score_type>
class weighted_quater {
public:
    static const std::size_t half_field = field_size / 2;

private:
    std::array<std::array<score_type, half_field>, half_field> weights_;
    
public:
    weighted_quater() {}

    template<class random_engine>
    void gen_random(random_engine &rnd) {
        // the second parameter is not really significant
        // as multiplication on a positive constant doesn't
        // change the order.
        std::normal_distribution<double> dist(0.0, 10.0);

        for (std::size_t i = 0; i != half_field; i++) {
            for (std::size_t j = 0; j != half_field; j++) {
                weights_[i][j] = dist(rnd);
            }
        }
    }

    template<class random_engine>
    void gen_random() {
        std::random_device rd;
        random_engine eng(rd());
        gen_random<random_engine>(eng);
    }
    
    void read(std::string filename) {
        std::ifstream file(filename, std::ios::binary);

        if (!file)
            throw scoring_file_exception("Could not open file " + filename + " for reading.");
        
        for (std::size_t i = 0; i != half_field; i++) {
            for (std::size_t j = 0; j != half_field; j++) {
                file.read(reinterpret_cast<char*>(&weights_[i][j]), sizeof(score_type));
                if (file.gcount() != sizeof(score_type))
                    throw scoring_file_exception("File " + filename + " is possibly dammaged.");
            }
        }

        if (!file.eof())
            throw scoring_file_exception("File " + filename + " is possibly dammaged.");
    }

    void write(std::string filename) {
        std::ofstream file(filename, std::ios::binary);

        if (!file)
            throw scoring_file_exception("Could not open file " + filename + " for writing.");

        for (std::size_t i = 0; i != half_field; i++) {
            for (std::size_t j = 0; j != half_field; j++) {
                file.write(reinterpret_cast<char*>(&weights_[i][j]), sizeof(score_type));
                if (!file)
                    throw scoring_file_exception("Writing into file " + filename + " failed.");
            }
        }
    }

    std::array<score_type, half_field>& operator[](std::size_t i) {
        return weights_[i];
    }

    const std::array<score_type, half_field>& operator[](std::size_t i) const {
        return weights_[i];
    }

    score_type operator()(const field& f) const {
        score_type ans = 0;
        for (std::size_t i = 0; i != field_size; i++) {
            for (std::size_t j = 0; j != field_size; j++) {
                std::size_t qi = (i < half_field ? i : field_size - i);
                std::size_t qj = (j < half_field ? j : field_size - j);
                if (f[i][j] == 'b')
                    ans += weights_[qi][qj];
                else
                    ans -= weights_[qi][qj];
            }
        }
        return ans;
    }
};
    
}
}
