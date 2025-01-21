/*------------------------------------------------------------------------*\
**  
**  @file:      test_topology.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Unit tests for topology
**
\*------------------------------------------------------------------------*/


#include <gtest/gtest.h>

#include "topology.hh"
#include "Vector.hh"

#include <vector>

class topologyTest 
: 
public 
    ::testing::Test 
{
public:
    // Constructor
    topologyTest() {};
protected:
    // Define some points
    MATH::Vector p0 = MATH::Vector(std::vector<double>{0.0, 0.0});
    MATH::Vector p1 = MATH::Vector(std::vector<double>{0.0, 4.0});
    MATH::Vector p2 = MATH::Vector(std::vector<double>{4.0, 0.0});
    MATH::Vector p3 = MATH::Vector(std::vector<double>{4.0, 4.0});
    MATH::Vector p4 = MATH::Vector(std::vector<double>{1.0, 2.0});
    MATH::Vector p5 = MATH::Vector(std::vector<double>{3.0, 2.0});
    MATH::Vector p6 = MATH::Vector(std::vector<double>{2.0, 1.0});
    MATH::Vector p7 = MATH::Vector(std::vector<double>{2.0, 3.0});

    // First Test: only hull points in order
    std::vector<MATH::Vector> test1 = {p0, p1, p2, p3};
    std::vector<int> result1 = {0, 2, 3, 1};

    // Second Test: only hull points out of order
    std::vector<MATH::Vector> test2 = {p0, p3, p1, p2};
    std::vector<int> result2 = {0, 3, 1, 2};

    // Third Test: All points in order
    std::vector<MATH::Vector> test3 = {p0, p1, p2, p3, p4, p5, p6, p7};
    std::vector<int> result3 = {0, 2, 3, 1};

    // Fourth Test: All points out of order
    std::vector<MATH::Vector> test4 = {p3, p7, p5, p0, p6, p4, p2, p1};
    std::vector<int> result4 = {3, 6, 0, 7};

    // Fifth Test: different square
    MATH::Vector n1 = MATH::Vector(std::vector<double>{0.0, 0.0});
    MATH::Vector n2 = MATH::Vector(std::vector<double>{0.5, 0.0});
    MATH::Vector n3 = MATH::Vector(std::vector<double>{0.0, 0.5});
    MATH::Vector n4 = MATH::Vector(std::vector<double>{0.5, 0.5});

    std::vector<MATH::Vector> test5 = {n1, n2, n3, n4};
    std::vector<int> result5 = {0, 1, 3, 2};

    // Sixth test: scattered points
    MATH::Vector m1 = MATH::Vector(std::vector<double>{-0.5, -4.0 });
    MATH::Vector m2 = MATH::Vector(std::vector<double>{-0.735145, -3.96353});
    MATH::Vector m3 = MATH::Vector(std::vector<double>{-0.673918, -3.65004});
    MATH::Vector m4 = MATH::Vector(std::vector<double>{-0.456313, -3.67706 });
    MATH::Vector m5 = MATH::Vector(std::vector<double>{-0.51, -3.9});

    std::vector<MATH::Vector> test6 = {m1, m2, m3, m4, m5};
    std::vector<int> result6 = {1, 0, 3, 2};

};

// * * * * * * * * * * * * * * * * * * Test On Simple Square * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testSquare) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test1);

    // Assert
    ASSERT_EQ(result.size(),result1.size());
    for (int i = 0; i < result.size(); i++) {
        // std::cout << result[i] << std::endl;
        ASSERT_EQ(result[i], result1[i]);
    }
}

// * * * * * * * * * * * * * * * * * * Test With Points mixed * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testSquareMixedPoints) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test2);

    // Assert
    ASSERT_EQ(result.size(),result2.size());
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], result2[i]);
    }
}

// * * * * * * * * * * * * * * * * * * Test With All Points * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testSquareContainingPoints) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test3);

    // Assert
    ASSERT_EQ(result.size(),result3.size());
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], result3[i]);
    }
}

// * * * * * * * * * * * * * * * * * * Test With All Points out of order * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testAllPointsUnordered) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test4);

    // Assert
    ASSERT_EQ(result.size(),result4.size());
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], result4[i]);
    }
}

// * * * * * * * * * * * * * * * * * * Test With All Points out of order * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testNodeSquare) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test5);

    // Assert
    ASSERT_EQ(result.size(),result5.size());
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], result5[i]);
    }
}

// * * * * * * * * * * * * * * * * * * Test With scattered points * * * * * * * * * * * * * * * * * * //
TEST_F(topologyTest, testScatteredPoints) {
    // Arrange

    // Act
    std::vector<int> result = MATH::jarvis_march(test6);

    // Assert
    ASSERT_EQ(result.size(),result6.size());
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], result6[i]);
    }
}

