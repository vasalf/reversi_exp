#include <genetics/genotype.h>

#include <cstring>
#include <algorithm>
#include <cassert>

namespace genetics {

genotype::bitreference &genotype::bitreference::operator=(bool b) {
    genotype::storage &ct = s_[bit_ / (8 * sizeof(genotype::storage))];
    std::size_t bn = bit_ % (8 * sizeof(genotype::storage));
    if (!b)
        ct &= ~((genotype::storage)1 << bn);
    else
        ct |= (genotype::storage)1 << bn;
    return *this;
}

genotype::bitreference &genotype::bitreference::operator=(const genotype::bitreference &other) {
    *this = (bool)(other);
    return *this;
}

genotype::bitreference::operator bool() const {
    genotype::storage &ct = s_[bit_ / (8 * sizeof(genotype::storage))];
    std::size_t bn = bit_ % (8 * sizeof(genotype::storage));
    return (ct >> bn) & 1;
}

void genotype::swap(genotype &g) {
    std::swap(n_bits_, g.n_bits_);
    std::swap(s_sz_, g.s_sz_);
    std::swap(s_, g.s_);
}
    
genotype::genotype(std::size_t bits) {
    n_bits_ = bits;
    s_sz_ = (bits + 8 * sizeof(storage) - 1) / (8 * sizeof(storage));
    s_ = new storage[s_sz_];
    std::memset(s_, 0, sizeof(storage) * s_sz_);
}

genotype::~genotype() {
    delete[] s_;
}

genotype::genotype(const genotype &other)
    : genotype(other.n_bits_) {
    std::memcpy(s_, other.s_, sizeof(storage) * s_sz_);
}

genotype &genotype::operator=(genotype other) {
    this->swap(other);
    return *this;
}

genotype::genotype(genotype &&other)
    : s_(nullptr), s_sz_(0), n_bits_(0) {
    this->swap(other);
}

genotype &genotype::operator=(genotype &&other) {
    if (this != &other) {
        this->~genotype();
        this->swap(other);
        other.s_sz_ = other.n_bits_ = 0;
        other.s_ = nullptr;
    }
    return *this;
}

genotype::bitreference genotype::operator[](std::size_t i) {
    assert(i < n_bits_);
    return genotype::bitreference(s_, i);
}

bool genotype::operator[](std::size_t i) const {
    assert(i < n_bits_);
    return genotype::bitreference(s_, i);
}

void genotype::cross_over(const genotype &a, std::size_t l, std::size_t r) {
    assert(a.size() == size());
    
    std::size_t li = l / (8 * sizeof(storage));
    std::size_t ri = r / (8 * sizeof(storage));

    for (std::size_t i = l; i != 8 * sizeof(storage) * (li + 1) && i != r; i++) {
        (*this)[i] = a[i];
    }

    std::memcpy(s_ + li, a.s_ + li, (ri - li) * sizeof(storage));

    if (li != ri) {
        for (std::size_t i = r; i != 8 * sizeof(storage) * ri - 1; i--) {
            bitreference b(s_, i);
            b = a[i];
        }
    }
}
    
}
