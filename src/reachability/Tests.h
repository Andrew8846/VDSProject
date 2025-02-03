#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2, 2);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> inputVars2 = fsm2->getInputs();
    std::vector<BDD_ID> transitionFunctions;

};

TEST_F(ReachabilityTest, HowTo_Example) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, HowTo_Example2)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
    BDD_ID i0 = inputVars2.at(0);
    BDD_ID i1 = inputVars2.at(1);

    transitionFunctions.push_back(fsm2->or2(fsm2->neg(s0), i0)); // s0' = not(s0)+i0
    transitionFunctions.push_back(fsm2->neg(s1));                // s1' = not(s1) 1 0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, HowTo_Example3)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->xor2(s0, s1)); // s0' = s0 ^ s1
    transitionFunctions.push_back(fsm2->neg(s1));      // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_TRUE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}


TEST_F(ReachabilityTest, HowTo_Example4)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0));
    transitionFunctions.push_back(fsm2->neg(s1));
    // fsm2->setTransitionFunctions(transitionFunctions);

    // fsm2->setInitState({true, false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_FALSE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, HowTo_Example5)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0));
    transitionFunctions.push_back(fsm2->neg(s1));
    // fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({true, false});

    ASSERT_FALSE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_TRUE(fsm2->isReachable({true, false}));
    ASSERT_FALSE(fsm2->isReachable({true, true}));
}


struct distanceReachabilityTest : testing::Test
{

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2, 1);
    std::unique_ptr<ClassProject::ReachabilityInterface> fsm3 = std::make_unique<ClassProject::Reachability>(3);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> inputVars = fsm2->getInputs();
    std::vector<BDD_ID> stateVars3 = fsm3->getStates();
    std::vector<BDD_ID> transitionFunctions;
};

TEST_F(distanceReachabilityTest, distance1)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0));
    transitionFunctions.push_back(fsm2->neg(s1));
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 1);
    EXPECT_EQ(fsm2->stateDistance({true, false}), -1);
    EXPECT_EQ(fsm2->stateDistance({false, true}), -1);
}

TEST_F(distanceReachabilityTest, distance2)
{
    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s1));
    transitionFunctions.push_back(s0);
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true, false}), 1);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 2);
    EXPECT_EQ(fsm2->stateDistance({false, true}), 3);
}

TEST_F(distanceReachabilityTest, distance3)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
    BDD_ID i0 = inputVars.at(0);

    transitionFunctions.push_back(fsm2->or2(fsm2->neg(s0), i0)); // s0' = not(s0)+i0
    transitionFunctions.push_back(fsm2->neg(s1));                // s1' = not(s1) 1 0
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false, false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({true, true}), 1);
    EXPECT_EQ(fsm2->stateDistance({true, false}), 2);
    EXPECT_EQ(fsm2->stateDistance({false, true}), 3);
}


TEST_F(distanceReachabilityTest, distance5)
{

    BDD_ID s0 = stateVars3.at(0);
    BDD_ID s1 = stateVars3.at(1);
    BDD_ID s2 = stateVars3.at(2);

    transitionFunctions.push_back(fsm3->or2(fsm3->and2(s0, fsm3->nand2(s1, s2)), fsm3->and2(s1, s2)));
    transitionFunctions.push_back(fsm3->xor2(s1, s2));
    transitionFunctions.push_back(fsm3->neg(s2));
    fsm3->setTransitionFunctions(transitionFunctions);

    fsm3->setInitState({false, false, false});

    EXPECT_EQ(fsm3->stateDistance({false, false, false}), 0);
    EXPECT_EQ(fsm3->stateDistance({false, false, true}), 1);
    EXPECT_EQ(fsm3->stateDistance({false, true, false}), 2);
    EXPECT_EQ(fsm3->stateDistance({false, true, true}), 3);
    EXPECT_EQ(fsm3->stateDistance({true, false, false}), 4);
    EXPECT_EQ(fsm3->stateDistance({true, false, true}), 5);
    EXPECT_EQ(fsm3->stateDistance({true, true, false}), 6);
    EXPECT_EQ(fsm3->stateDistance({true, true, true}), 7);
}

TEST_F(distanceReachabilityTest, distance6)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->xor2(s0, s1));
    transitionFunctions.push_back(fsm2->neg(s1));
    //fsm2->setTransitionFunctions(transitionFunctions);

    //fsm2->setInitState({false, false});

    EXPECT_EQ(fsm2->stateDistance({false, false}), 0);
    EXPECT_EQ(fsm2->stateDistance({false, true}), -1);
    EXPECT_EQ(fsm2->stateDistance({true, false}), -1);
    EXPECT_EQ(fsm2->stateDistance({true, true}), -1);
}


TEST_F(ReachabilityTest, isReachableRuntimeErrorTest)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(s0);
    transitionFunctions.push_back(s1);
    fsm2->setTransitionFunctions(transitionFunctions);
    fsm2->setInitState({false, false});

    // test for one missing state
    EXPECT_THROW(fsm2->isReachable({false}), std::runtime_error);
    // test for one additional state
    EXPECT_THROW(fsm2->isReachable({false, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, stateDistanceRuntimeErrorTest)
{

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);
    // throws std::runtime_error if size does not match with number of state bits
    transitionFunctions.push_back(s0);
    transitionFunctions.push_back(s1);
    fsm2->setTransitionFunctions(transitionFunctions);
    fsm2->setInitState({false, false});
    // test for one missing state
    EXPECT_THROW(fsm2->stateDistance({false}), std::runtime_error);
    // test for one additional state
    EXPECT_THROW(fsm2->stateDistance({false, false, false}), std::runtime_error);
}


#endif