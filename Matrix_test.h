#ifndef MATRIXTEST_CPP
#define MATRIXTEST_CPP
#include <gtest/gtest.h>
#include "Matrix.h"

TEST(Matrix, Init) {
    Matrix<int, -1> m;
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.rows(), 0);
    EXPECT_EQ(m.cols(), 0);

    m.insert(40, 40, -1);
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.rows(), 0);

    m.insert(40, 40, 1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(m.cols(), 41);
    EXPECT_EQ(m.rows(), 41);

    m.insert(10, 45, 2);
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m.cols(), 46);
    EXPECT_EQ(m.rows(), 41);

    m.insert(45, 1, -1);
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m.cols(), 46);
    EXPECT_EQ(m.rows(), 41);

    m.insert(45, 1, -2);
    EXPECT_EQ(m.size(), 3);
    EXPECT_EQ(m.cols(), 46);
    EXPECT_EQ(m.rows(), 46);

    m.insert(45, 1, -1);
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m.cols(), 46);
    EXPECT_EQ(m.rows(), 41);

    m.insert(10, 45, -1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(m.cols(), 41);
    EXPECT_EQ(m.rows(), 41);
}

TEST(Matrix, IteratorsAndIndexing) {
    constexpr auto defaultValue = 0;
    Matrix<int, defaultValue> m;
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.rows(), 0);
    EXPECT_EQ(m.cols(), 0);

    int sz = 0;
    for (auto row = 0; row < 10; ++row) {
        for (auto col = 0; col < 10; ++col) {
            if (row + col != defaultValue)
                sz++;
            m.insert(row, col, row + col);

            EXPECT_EQ(m.size(), sz);
            EXPECT_EQ(m.rows(), !row && !col ? 0 : row + 1);
            EXPECT_EQ(m.cols(), !row && !col ? 0 : col * row + 1);
        }
    }

    for (auto it = m.begin(); it != m.end(); ++it) {
        EXPECT_EQ(it.value(), it.pos().col + it.pos().row);
    }

    for (auto it = m.cbegin(); it != m.cend(); ++it) {
        EXPECT_EQ(it.value(), it.pos().col + it.pos().row);
    }
}

#endif // MATRIXTEST_CPP
