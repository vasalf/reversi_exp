#include <random>
#include <array>
#include <iostream>

#include <gtest/gtest.h>

#include <genetics/genotype.h>

TEST(Genotype, dummy) {
    genetics::genotype g(64);
    for (std::size_t i = 0; i != 64; i++) {
        g[i] = false;
    }
    for (std::size_t i = 0; i != 64; i++) {
        EXPECT_EQ(g[i], false);
    }
}

TEST(Genotype, rnd_array) {
    const std::size_t size = 1e6 + 179;
    genetics::genotype g(size);
    std::array<bool, size> *arr = new std::array<bool, size>();
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i != size; i++) {
        (*arr)[i] = dist(rnd);
        g[i] = (*arr)[i];
    }
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ((*arr)[i], g[i]);
    }
    delete arr;
}

TEST(Genotype, rnd_eq_line) {
    const std::size_t size = 1e6 + 179;
    genetics::genotype g(size), h(size), k(size);
    std::array<bool, size> *arr = new std::array<bool, size>();
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i != size; i++) {
        (*arr)[i] = dist(rnd);
        g[i] = h[i] = k[i] = (*arr)[i];
    }
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ((*arr)[i], g[i]);
        EXPECT_EQ((*arr)[i], h[i]);
        EXPECT_EQ((*arr)[i], k[i]);
    }
    delete arr;
}

TEST(Genotype, copy_assignment) {
    const std::size_t size = 1e6 + 179;
    genetics::genotype g(size), h(size);
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i != size; i++) {
        bool b = dist(rnd);
        g[i] = b;
        h[i] = !b;
    }
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_NE(g[i], h[i]);
    }
    h = g;
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ(h[i], g[i]);
    }
    h[0] = !h[0];
    EXPECT_NE(h[0], g[0]);
    for (std::size_t i = 1; i != size; i++)
        EXPECT_EQ(h[i], g[i]);
    genetics::genotype w(0);
    w = h = g;
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ(w[i], h[i]);
        EXPECT_EQ(w[i], g[i]);
    }
    genetics::genotype::bitreference br = g[0];
    w = g;
    br = !br;
    EXPECT_NE(g[0], w[0]);
}

TEST(Genotype, copy_constructor) {
    const std::size_t size = 1e6 + 179;
    genetics::genotype g(size);
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i != size; i++) {
        bool b = dist(rnd);
        g[i] = b;
    }
    genetics::genotype h(g);
    EXPECT_EQ(g.size(), h.size());
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ(g[i], h[i]);
    }
}

bool get_bit_const(const genetics::genotype &g, std::size_t i) {
    return g[i];
}

TEST(Genotype, const_at) {
    const std::size_t size = 1e6 + 179;
    genetics::genotype g(size);
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    std::array<bool, size> *arr = new std::array<bool, size>();
    for (std::size_t i = 0; i != size; i++) {
        bool b = dist(rnd);
        g[i] = b;
        (*arr)[i] = b;
    }
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ(get_bit_const(g, i), (*arr)[i]);
    }
    delete arr;
}

TEST(Genotype, size) {
    genetics::genotype s0(0), s1(1), s179(179), s1791791(1791791);
    EXPECT_EQ(s0.size(), 0);
    EXPECT_EQ(s1.size(), 1);
    EXPECT_EQ(s179.size(), 179);
    EXPECT_EQ(s1791791.size(), 1791791);
}

TEST(Genotype, bitreference_assignment) {
    genetics::genotype g(1);
    g[0] = false;
    EXPECT_EQ(g[0], false);
    genetics::genotype::bitreference b = g[0];
    b = true;
    EXPECT_EQ(b, true);
    EXPECT_EQ(g[0], true);
}

genetics::genotype dummy_crossing_over(genetics::genotype g, genetics::genotype h, std::size_t l, std::size_t r) {
    for (std::size_t i = l; i != r + 1; i++) {
        g[i] = h[i];
    }
    return g;
}


TEST(Genotype, cross_over) {
    const std::size_t size = 20;//1e6 + 179;
    genetics::genotype g(size), h(size);
    std::mt19937 rnd(179);
    std::uniform_int_distribution<int> dist(0, 1);
    for (std::size_t i = 0; i != size; i++) {
        g[i] = dist(rnd);
        h[i] = g[i];
    }
    genetics::genotype exp = dummy_crossing_over(g, h, 1, size - 1);
    g.cross_over(h, 1, size - 1);
    for (std::size_t i = 0; i != size; i++) {
        EXPECT_EQ(g[i], exp[i]);
    }
}

TEST(Genotype, move_constructor) {
    genetics::genotype g(5);
    genetics::genotype h(std::move(g));
}

TEST(Genotype, move_assignment) {
    genetics::genotype g(5);
    genetics::genotype h = std::move(g);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
