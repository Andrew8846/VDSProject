// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <vector>
#include <string>
#include "ManagerInterface.h"

namespace ClassProject {
    using std::vector;
    using std::unordered_map;
    using std::string;
    using std::set;

    struct BDDNode {
        size_t high;
        size_t low;
        size_t topVar;

        bool operator ==(const BDDNode &other) const {
            return high == other.high && low == other.low && topVar == other.topVar;
        }
    };

    struct BDDNodeWithId {
        size_t id;
        size_t high;
        size_t low;
        size_t topVar;
    };

    struct ITETriple {
        size_t i;
        size_t t;
        size_t e;

        bool operator ==(const ITETriple &other) const {
            return i == other.i && t == other.t && e == other.e;
        }
    };

    struct BDDNodeHash {
        size_t operator()(const BDDNode& node) const {
            size_t hash = std::hash<size_t>()(node.high);
            hash ^= std::hash<size_t>()(node.low) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<size_t>()(node.topVar) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };

    struct ITEHash {
        size_t operator()(const ITETriple& node) const {
            size_t hash = std::hash<size_t>()(node.i);
            hash ^= std::hash<size_t>()(node.t) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<size_t>()(node.e) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };

    class Manager : public ManagerInterface {
    private:
        unordered_map<BDDNode, size_t, BDDNodeHash> uniqueTable; // double indexed list map
        vector<BDDNodeWithId> uniqueTableReversed;
        unordered_map<string, size_t> variableMap;
        unordered_map<ITETriple, size_t, ITEHash> computedTable;
        const BDD_ID falseID = 0;
        const BDD_ID trueID = 1;

    public:
        Manager();

        BDD_ID createVar(const std::string &label) override;

        const BDD_ID &True() override;

        const BDD_ID &False() override;

        bool isConstant(BDD_ID f) override;

        bool isVariable(BDD_ID x) override;

        BDD_ID topVar(BDD_ID f) override;

        BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;

        BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;

        BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;

        BDD_ID coFactorTrue(BDD_ID f) override;

        BDD_ID coFactorFalse(BDD_ID f) override;

        BDD_ID neg(BDD_ID a) override;

        BDD_ID and2(BDD_ID a, BDD_ID b) override;

        BDD_ID or2(BDD_ID a, BDD_ID b) override;

        BDD_ID xor2(BDD_ID a, BDD_ID b) override;

        BDD_ID nand2(BDD_ID a, BDD_ID b) override;

        BDD_ID nor2(BDD_ID a, BDD_ID b) override;

        BDD_ID xnor2(BDD_ID a, BDD_ID b) override;

        std::string getTopVarName(const BDD_ID &root) override;

        void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;

        void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;

        size_t uniqueTableSize() override;

        void visualizeBDD(std::string filepath, BDD_ID &root) override;
    };
}

#endif
