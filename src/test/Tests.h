//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

class ManagerTests : public testing::Test {
protected:
    ClassProject::Manager manager;
    ClassProject::BDD_ID a_id, b_id, c_id;

    void SetUp() override {
        a_id = manager.createVar("a");
        b_id = manager.createVar("b");
        c_id = manager.createVar("c");
    }
};

TEST_F(ManagerTests, CreateVariable) {
    EXPECT_EQ(a_id, 2);
    EXPECT_EQ(b_id, 3);
    EXPECT_EQ(c_id, 4);

    EXPECT_EQ(manager.createVar("a"), a_id); // re-creating same variable
    EXPECT_EQ(manager.createVar("b"), b_id);

    EXPECT_EQ(manager.uniqueTableSize(), 5);
}

TEST_F(ManagerTests, Constants) {
    EXPECT_EQ(manager.True(), 1);
    EXPECT_EQ(manager.False(), 0);

    EXPECT_TRUE(manager.isConstant(0));
    EXPECT_TRUE(manager.isConstant(1));
    EXPECT_FALSE(manager.isConstant(a_id));
}

TEST_F(ManagerTests, TopVariable) {
    EXPECT_EQ(manager.topVar(a_id), a_id); // Top variable of 'a' is 'a' itself
    EXPECT_EQ(manager.topVar(b_id), b_id); // Top variable of 'b' is 'b' itself

    // For constants, the topVar is 0 (no variable)
    EXPECT_EQ(manager.topVar(manager.True()), 0);
    EXPECT_EQ(manager.topVar(manager.False()), 0);
}

// TEST_F(ManagerTests, LogicalOperations) {
//     // auto and_ab = manager.and2(a_id, b_id);
//     // EXPECT_EQ(and_ab, manager.ite(a_id, b_id, manager.False())); // AND operation should match ITE
//     // // EXPECT_EQ(!and_ab, manager.nand2(a_id, b_id));
//
//     // auto or_ab = manager.or2(a_id, b_id);
//     // EXPECT_EQ(or_ab, manager.ite(a_id, manager.True(), b_id));
//     // EXPECT_EQ(!or_ab, manager.nor2(a_id, b_id));
//
//     // manager.or2(a_id, b_id);
//     // auto xor_ab = manager.xor2(a_id, b_id);
//     // EXPECT_EQ(xor_ab, manager.ite(a_id, manager.neg(b_id), b_id ));
//     // // EXPECT_EQ(!xor_ab, manager.xnor2(a_id, b_id));
// }

TEST_F(ManagerTests, Negation) {
    // Negate constants
    EXPECT_EQ(manager.neg(manager.True()), manager.False()); // !True = False
    EXPECT_EQ(manager.neg(manager.False()), manager.True()); // !False = True

    // Negate variables
    auto not_a = manager.neg(a_id);
    EXPECT_NE(not_a, a_id); // Negated 'a' should be a new node
}

TEST_F(ManagerTests, FindNodes) {
    std::set<ClassProject::BDD_ID> nodes;
    manager.findNodes(a_id, nodes);

    // 'a' is the only node reachable from itself
    EXPECT_EQ(nodes.size(), 3); // 'a', 'True', and 'False'
    EXPECT_TRUE(nodes.find(a_id) != nodes.end()); // 'a' is reachable
    EXPECT_TRUE(nodes.find(manager.True()) != nodes.end()); // True is reachable
    EXPECT_TRUE(nodes.find(manager.False()) != nodes.end()); // False is reachable
}

TEST_F(ManagerTests, FindVars) {
    std::set<ClassProject::BDD_ID> vars;
    manager.findVars(a_id, vars);

    // 'a' is the only variable
    EXPECT_EQ(vars.size(), 1);
    EXPECT_TRUE(vars.find(a_id) != vars.end()); // 'a' should be in the set
}

#endif
