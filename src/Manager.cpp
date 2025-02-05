#include "Manager.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace ClassProject {

    Manager::Manager() {
        uniqueTable[{0, 0, 0}] = falseID;
        uniqueTable[{1, 1, 1}] = trueID;

        uniqueTableReversed.push_back({falseID, 0, 0, 0});
        uniqueTableReversed.push_back({trueID, 1, 1, 1});

        variableMap["False"] = 0;
        variableMap["True"] = 1;

        std::cout << "Manager initialized.\n";
    }

    BDD_ID Manager::createVar(const std::string &label) {
        if (variableMap.find(label) != variableMap.end()) {
            return variableMap[label];
        }

        BDD_ID id = uniqueTable.size();
        BDDNode node{trueID, falseID, id};
        variableMap[label] = id;
        uniqueTable[node] = id;
        uniqueTableReversed.push_back({id, trueID, falseID, id});
        std::cout << "Created variable: " << label << ", ID: " << id << "\n";
        return id;
    }

    const BDD_ID &Manager::True() {
        return trueID;
    }

    const BDD_ID &Manager::False() {
        return falseID;
    }

    bool Manager::isConstant(BDD_ID f) {
        return f == falseID || f == trueID;
    }

    bool Manager::isVariable(BDD_ID x) {
        return !isConstant(x) && x < uniqueTableReversed.size() && uniqueTableReversed[x].topVar == x;
    }

    BDD_ID Manager::topVar(BDD_ID f) {
        if (f >= uniqueTableReversed.size()) {
            throw std::out_of_range("Invalid BDD_ID: " + std::to_string(f));
        }

        return uniqueTableReversed[f].topVar;
    }

    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
        if (i == trueID) {
            return t;
        }
        if (i == falseID) {
            return e;
        }
        if (t == e) {
            return t;
        }
        // computed table added
        auto itcomp = computedTable.find(ITETriple{i, t, e});
        if (itcomp!=computedTable.end()) {
            return itcomp->second;
        }

        size_t top = std::numeric_limits<size_t>::max();
        if (!isConstant(i)) top = std::min(top, topVar(i));
        if (!isConstant(t)) top = std::min(top, topVar(t));
        if (!isConstant(e)) top = std::min(top, topVar(e));

        BDD_ID rhigh = ite(coFactorTrue(i, top), coFactorTrue(t, top), coFactorTrue(e, top));
        BDD_ID rlow = ite(coFactorFalse(i, top), coFactorFalse(t, top), coFactorFalse(e, top));

        if (rhigh == rlow) {
            return rhigh;
        }

        BDDNode node{rhigh, rlow, top};
        ITETriple triple{i, t, e};

        auto it = uniqueTable.find(node);
        if (it != uniqueTable.end()) { // if result already exists in unique table
            computedTable[triple] = it->second;
            return it->second; // second is id
        }

        BDD_ID id = uniqueTable.size();
        uniqueTable[node] = id;
        uniqueTableReversed.push_back({id, rhigh, rlow, top});
        computedTable[triple] = id;

        return id;
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x) {
        if (isConstant(f)) {
            return f;
        }
        if (topVar(f) > x) {
            return f;
        }

        if (topVar(f) < x) {
            return ite(topVar(f), coFactorTrue(uniqueTableReversed[f].high, x), coFactorTrue(uniqueTableReversed[f].low, x));
        }
        return uniqueTableReversed[f].high;
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x) {
        if (isConstant(f)) {
            return f;
        }
        if (topVar(f) > x) {
            return f;
        }

        if (topVar(f) < x) {
            return ite(topVar(f), coFactorFalse(uniqueTableReversed[f].high, x), coFactorFalse(uniqueTableReversed[f].low, x));
        }
        return uniqueTableReversed[f].low;
    }

    BDD_ID Manager::coFactorTrue(BDD_ID f) {
        return uniqueTableReversed[f].high;
    }

    BDD_ID Manager::coFactorFalse(BDD_ID f) {
        return uniqueTableReversed[f].low;
    }

    BDD_ID Manager::neg(BDD_ID a) {
        return ite(a, falseID, trueID);
    }

    BDD_ID Manager::and2(BDD_ID a, BDD_ID b) {
        return ite(a, b, falseID);
    }

    BDD_ID Manager::or2(BDD_ID a, BDD_ID b) {
        return ite(a, trueID, b);
    }

    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b) {
        return ite(a, neg(b), b);
    }

    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b) {
        return neg(and2(a, b));
    }

    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b) {
        return neg(or2(a, b));
    }

    BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b) {
        return neg(xor2(a, b));
    }

    std::string Manager::getTopVarName(const BDD_ID &root) {
        for (const auto &pair : variableMap) {
            if (pair.second == topVar(root)) {
                return pair.first;
            }
        }
        return "";
    }

    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) {
        if (nodes_of_root.find(root) != nodes_of_root.end()) {
            return;
        }

        nodes_of_root.insert(uniqueTableReversed[root].topVar);
        if (isConstant(root)) {
            return;
        }
        findNodes(uniqueTableReversed[root].high, nodes_of_root);
        findNodes(uniqueTableReversed[root].low, nodes_of_root);
    }

    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) {
        std::set<BDD_ID> nodes_of_root;
        findNodes(root, nodes_of_root);

        for (const auto &node : nodes_of_root) {
            if (isVariable(node)) {
                vars_of_root.insert(uniqueTableReversed[node].topVar);
            }
        }
    }

    size_t Manager::uniqueTableSize() {
        return uniqueTable.size();
    }

    void Manager::visualizeBDD(std::string filepath, BDD_ID &root) {
        printf("not implemented yet");
    }
}
