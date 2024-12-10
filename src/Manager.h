// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H


#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

#include "ManagerInterface.h"

namespace ClassProject {
    using std::vector;
    using std::unordered_map;
    using std::string;
    using std::set;

    struct BDDNode {
        size_t id;
        size_t high;
        size_t low;
        size_t topVar;
    };

    class Manager : public ManagerInterface {
    private:
        vector<BDDNode> uniqueTable;
        unordered_map<string, size_t> variableMap;
        const BDD_ID falseID = 0;
        const BDD_ID trueID = 1;
    public:
        Manager() { // constructor
            // initialize uniqueTable as it is on first task
            uniqueTable.push_back({falseID, 0, 0, 0});
            uniqueTable.push_back({trueID, 1, 1, 1});
            uniqueTable.push_back({2, 1, 0, 2}); // a
            uniqueTable.push_back({3, 1, 0, 3}); // b
            uniqueTable.push_back({4, 1, 0, 4}); // c
            uniqueTable.push_back({5, 1, 0, 5}); // d
            uniqueTable.push_back({6, 1, 3, 2}); // a + b
            uniqueTable.push_back({7, 5, 0, 4}); // c * d
            uniqueTable.push_back({8, 7, 0, 3}); // b * c * d
            uniqueTable.push_back({9, 7, 8, 2}); // f

            variableMap["a"] = 2;
            variableMap["b"] = 3;
            variableMap["c"] = 4;
            variableMap["d"] = 5;

            std::cout << "Manager initialized.\n";
        }

        BDD_ID createVar(const std::string &label) override {
            if(variableMap.find(label)!=variableMap.end()) {
                return variableMap[label];
            }

            BDD_ID id = uniqueTable.size();
            variableMap[label] = id;
            uniqueTable.push_back({id, trueID, falseID, id});
            std::cout << "Created variable: " << label << ", ID: " << id << "\n";
            return id;
        }

        const BDD_ID &True() override {
            return trueID;
        }

        const BDD_ID &False() override {
            return falseID;
        }

        bool isConstant(BDD_ID f) override {
            return f == falseID || f == trueID;
        }

        bool isVariable(BDD_ID x) override {
            if (isConstant(x)) return false;
            bool isVar = x < uniqueTable.size() && uniqueTable[x].topVar == x; // if its in bounds and its top variable is itself.
            return isVar;
        }

        BDD_ID topVar(BDD_ID f) override {
            if (f >= uniqueTable.size()) {
                throw std::out_of_range("Invalid BDD_ID: " + std::to_string(f));
            }
            return uniqueTable[f].topVar;
        }

        BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override {
            if(i == trueID) {
                return  t;
            }
            if(i == falseID) {
                return  e;
            }
            if(t == e) {
                return t;
            }

            size_t top = std::numeric_limits<size_t>::max(); // Start with a high value
            if (!isConstant(i)) top = std::min(top, topVar(i));
            if (!isConstant(t)) top = std::min(top, topVar(t));
            if (!isConstant(e)) top = std::min(top, topVar(e));

            // size_t top1 = std::min(topVar(i), topVar(t));
            // size_t top = std::min(top1, topVar(e));
            BDD_ID rhigh = ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
            BDD_ID rlow = ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));

            if (rhigh == rlow) {return rhigh;}

            // searching in unique table if the node already exists
            for (const auto &node : uniqueTable) {
                if (node.high == rhigh && node.low == rlow && node.topVar == top) {
                    return node.id;
                }
            }

            BDD_ID id = uniqueTable.size();
            uniqueTable.push_back({id, rhigh, rlow, top});
            return id;
        }

        // todo question these two
        BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override {
            // constant handling
            if (isConstant(f)) {
                return f;
            }

            if (topVar(f) > x) {
                return f;
            } // x doesn't affect f. x has lower id than top variable of f function
            if (topVar(f) < x) {
                return ite(x, coFactorTrue(uniqueTable[f].high, x), coFactorTrue(uniqueTable[f].low, x));
            }

            return uniqueTable[f].high; // topvariable is x and directly return high
        }

        BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override {
            // constant handling
            if (isConstant(f)) {
                return f;
            }

            if (topVar(f) > x) {
                return f;
            }
            if (topVar(f) < x) {
                return ite(x, coFactorFalse(uniqueTable[f].high, x), coFactorFalse(uniqueTable[f].low, x));
            }

            return uniqueTable[f].low;
        }

        BDD_ID coFactorTrue(BDD_ID f) override {
            return uniqueTable[f].high;
        }

        BDD_ID coFactorFalse(BDD_ID f) override {
            return uniqueTable[f].low;
        }

        BDD_ID neg(BDD_ID a) override {
            return ite (a, falseID, trueID);
        }

        BDD_ID and2(BDD_ID a, BDD_ID b) override {
            return ite(a, b, falseID);
        }

        BDD_ID or2(BDD_ID a, BDD_ID b) override {
            std::cout << "or2 called with a: " << a << ", b: " << b << "\n";
            BDD_ID result = ite(a, trueID, b);
            std::cout << "or2 result: " << result << "\n";
            return result;
        }

        BDD_ID xor2(BDD_ID a, BDD_ID b) override {
            return ite(a, neg(b), b);
        }

        BDD_ID nand2(BDD_ID a, BDD_ID b) override {
            return neg(and2(a, b));
        }

        BDD_ID nor2(BDD_ID a, BDD_ID b) override {
            return neg(or2(a, b));
        }

        BDD_ID xnor2(BDD_ID a, BDD_ID b) override {
            return neg(xor2(a, b));
        }

        std::string getTopVarName(const BDD_ID &root) override {
            for (const auto &pair : variableMap) {
                if (pair.second == topVar(root)) {
                    return pair.first;
                }
            }

            return "";
        }

        void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override {
            if (nodes_of_root.find(root) != nodes_of_root.end()) { // element already exsists in a set
                return;
            }

            nodes_of_root.insert(root);

            if (isConstant(root)) {
                return;
            }

            // recursively search for next level nodes
            findNodes(uniqueTable[root].high, nodes_of_root);
            findNodes(uniqueTable[root].low, nodes_of_root);
        }

        void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override {
            set<BDD_ID> nodes_of_root;
            findNodes(root, nodes_of_root);

            for (const auto &node : nodes_of_root) { // extract real variables
                if (isVariable(node)) {
                    vars_of_root.insert(uniqueTable[node].topVar);
                }
            }
        }

        size_t uniqueTableSize() override {
            return uniqueTable.size();
        }

        void visualizeBDD(std::string filepath, BDD_ID &root) override {
            printf("not implemented yet");
        }
    };
}

#endif
