//
// Created by ludwig on 22.11.16.
//


#include "Tests.h"

using namespace ClassProject;

class ManagerTests : public testing::Test {
protected:
    ClassProject::Manager manager;
    ClassProject::BDD_ID a_id, b_id, c_id, d_id, a_or_b_id, c_and_d_id, f1_id, androtestID;

    void SetUp() override {
        a_id = manager.createVar("a");
        b_id = manager.createVar("b");
        c_id = manager.createVar("c");
        d_id = manager.createVar("d");

        a_or_b_id = manager.or2(a_id, b_id);
        c_and_d_id = manager.and2(c_id, d_id);
        // f1 = (a+b)*c*d
        f1_id = manager.and2(a_or_b_id, c_and_d_id);

    }
};


TEST_F(ManagerTests, ccdsdfa) {
    EXPECT_EQ(manager.coFactorTrue(a_or_b_id, a_id), manager.True());
    EXPECT_EQ(manager.coFactorTrue(f1_id, a_id), c_and_d_id);
    EXPECT_EQ(manager.coFactorTrue(f1_id, a_id), manager.coFactorTrue(f1_id));
    EXPECT_EQ(manager.coFactorTrue(f1_id, b_id), c_and_d_id);
    EXPECT_EQ(manager.coFactorTrue(f1_id, c_id), manager.and2(d_id, a_or_b_id));
    EXPECT_EQ(manager.coFactorTrue(f1_id, d_id), manager.and2(c_id, a_or_b_id));

    std::cout << "es testic damtavrdaaa" << std::endl;

}


TEST_F(ManagerTests, FindVariables1) {
    std::set<ClassProject::BDD_ID> vars;
    BDD_ID a_or_b_id = manager.or2(a_id, b_id);
    ClassProject::BDD_ID c_and_d_id = manager.and2(c_id, d_id);
    // f1 = a*b+c+d
    ClassProject::BDD_ID f1_id = manager.and2(a_or_b_id, c_and_d_id);
    manager.findVars(f1_id, vars);

    // 'a' is the only variable
    EXPECT_EQ(vars.size(), 4);
    EXPECT_TRUE(vars.find(a_id) != vars.end()); // 'a' should be in the set
}

TEST_F(ManagerTests, CreateVariable) {
    EXPECT_EQ(a_id, 2);
    EXPECT_EQ(b_id, 3);
    EXPECT_EQ(c_id, 4);

    EXPECT_EQ(manager.createVar("a"), a_id); // re-creating same variable
    EXPECT_EQ(manager.createVar("b"), b_id);

    EXPECT_EQ(manager.uniqueTableSize(), 10);
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
    EXPECT_EQ(manager.topVar(manager.True()), 1);
    EXPECT_EQ(manager.topVar(manager.False()), 0);
}

TEST_F(ManagerTests, LogicalOperations) {
    auto and_ab = manager.and2(a_id, b_id);
    EXPECT_EQ(and_ab, manager.ite(a_id, b_id, manager.False())); // AND operation should match ITE
    // // EXPECT_EQ(!and_ab, manager.nand2(a_id, b_id));

    auto or_ab = manager.or2(a_id, b_id);
    EXPECT_EQ(or_ab, manager.ite(a_id, manager.True(), b_id));
    // EXPECT_EQ(!or_ab, manager.nor2(a_id, b_id));

    auto xor_ab = manager.xor2(a_id, b_id);
    EXPECT_EQ(xor_ab, manager.ite(a_id, manager.neg(b_id), b_id ));
    // // EXPECT_EQ(!xor_ab, manager.xnor2(a_id, b_id));
}

TEST_F(ManagerTests, CofactorsForConstants) {
    EXPECT_EQ(manager.coFactorTrue(manager.True(), a_id), manager.True());  // True coFactored w.r.t 'a' is still True
    EXPECT_EQ(manager.coFactorFalse(manager.True(), a_id), manager.True()); // True coFactored w.r.t 'a' is still True

    EXPECT_EQ(manager.coFactorTrue(manager.False(), a_id), manager.False()); // False coFactored w.r.t 'a' is still False
    EXPECT_EQ(manager.coFactorFalse(manager.False(), a_id), manager.False()); // False coFactored w.r.t 'a' is still False
}

TEST_F(ManagerTests, CofactorsForLogicalExpressions) {
    auto and_ab = manager.and2(a_id, b_id);

    // High branch for 'a AND b' w.r.t 'a' is 'b' (when 'a' is True)
    EXPECT_EQ(manager.coFactorTrue(and_ab, a_id), b_id);

    // Low branch for 'a AND b' w.r.t 'a' is False (when 'a' is False)
    EXPECT_EQ(manager.coFactorFalse(and_ab, a_id), manager.False());
    // 'a AND b' does not depend on 'c', so cofactors w.r.t 'c' should return the expression itself
}

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

TEST_F(ManagerTests, ITEFunction) {
    // Case: i == True
    EXPECT_EQ(manager.ite(manager.True(), a_id, b_id), a_id);

    // Case: i == False
    EXPECT_EQ(manager.ite(manager.False(), a_id, b_id), b_id);

    // Case: t == e
    EXPECT_EQ(manager.ite(a_id, b_id, b_id), b_id);

    // General case
    auto result = manager.ite(a_id, b_id, c_id);
    EXPECT_NE(result, a_id);
    EXPECT_NE(result, b_id);
    EXPECT_NE(result, c_id);
}

TEST_F(ManagerTests, IsVariable) {
    EXPECT_TRUE(manager.isVariable(a_id)); // Variable

    EXPECT_FALSE(manager.isVariable(manager.True())); // True is not a variable
    EXPECT_FALSE(manager.isVariable(manager.False())); // False is not a variable

    auto and_ab = manager.and2(a_id, b_id);
    EXPECT_FALSE(manager.isVariable(and_ab)); // Logical operation result is not a variable
}

TEST_F(ManagerTests, LogicalNegation) {
    EXPECT_EQ(manager.nand2(a_id, b_id), manager.neg(manager.and2(a_id, b_id)));
    EXPECT_EQ(manager.nor2(a_id, b_id), manager.neg(manager.or2(a_id, b_id)));
    EXPECT_EQ(manager.xnor2(a_id, b_id), manager.neg(manager.xor2(a_id, b_id)));
}

TEST_F(ManagerTests, TopVarInvalidID) {
    EXPECT_THROW(manager.topVar(999), std::out_of_range); // Non-existent ID
}

TEST_F(ManagerTests, UniqueTableSize) {
    EXPECT_EQ(manager.uniqueTableSize(), 10); // Initial size
    manager.createVar("new_var");
    EXPECT_EQ(manager.uniqueTableSize(), 11); // After adding a new variable
}

TEST_F(ManagerTests, CofactorsOneInputTest) {
    EXPECT_EQ(manager.coFactorTrue(a_id), manager.True());
    EXPECT_EQ(manager.coFactorFalse(a_id), manager.False());
    auto or_ab = manager.or2(a_id, b_id);
    EXPECT_EQ(manager.coFactorTrue(or_ab), manager.True());
    EXPECT_EQ(manager.coFactorFalse(or_ab), b_id);
}


TEST_F(ManagerTests, random) {
    // auto result1 = manager.ite(a_id, b_id, c_id);
    // auto result2 = manager.ite(a_id, b_id, c_id);
    // EXPECT_EQ(result1, result2);

    // Case: i == False
    // EXPECT_EQ(manager.ite(manager.False(), a_id, b_id), b_id);
    //
    // // Case: t == e
    // EXPECT_EQ(manager.ite(a_id, b_id, b_id), b_id);
    //
    // // General case
    // auto result = manager.ite(a_id, b_id, c_id);
    // EXPECT_NE(result, a_id);
    // EXPECT_NE(result, b_id);
    // EXPECT_NE(result, c_id);
}
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
