#pragma once

#include <cstddef>
#include <cstdint>

namespace genetics {

class genotype {
public:
    typedef std::uint64_t storage;

private:
    storage *s_;
    std::size_t s_sz_;
    std::size_t n_bits_;

public:

    class bitreference {
        std::size_t bit_;
        storage *s_;
    public:
        bitreference(storage *s, std::size_t bit)
            : s_(s), bit_(bit) {}
        virtual ~bitreference() = default;

        bitreference &operator=(bool b);
        bitreference &operator=(const bitreference &other);
        operator bool() const;
    };
    
    genotype(std::size_t bits);
    virtual ~genotype();

    genotype(const genotype &other);
    genotype &operator=(genotype other);

    genotype(genotype &&other);
    genotype &operator=(genotype &&other);

    bitreference operator[](std::size_t i);
    bool operator[](std::size_t i) const;

    std::size_t size() const { return n_bits_; }

    void cross_over(const genotype &a, std::size_t l, std::size_t r);
};

}
