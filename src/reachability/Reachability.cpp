#include "Reachability.h"
#include <iostream>
#include <fstream>

using namespace std;

using namespace ClassProject;

 Reachability::Reachability(unsigned int stateSize, unsigned int inputSize) {


          if (stateSize == 0) {
              throw std::invalid_argument("stateSize must be greater than zero");
          }

          for (int i = 0; i < stateSize; i++) {
            stateVars.push_back(createVar("s" + std::to_string(i)));
            nextStateVars.push_back(createVar("s'" + std::to_string(i)));
          }

         for (int i = 0; i < inputSize; i++) {
           inputVars.push_back(createVar("v" + std::to_string(i)));
         }

         // create initial reachable state all 0
         // ~s1 & ~s2 & ~s3 ...
         currentReachableSet = True();
         for (const auto &var : stateVars) {
           currentReachableSet = and2(currentReachableSet, neg(var));
         }

         transitionRelation = 0; // todo needs to be done
 }


        const std::vector<BDD_ID> &Reachability::getStates() const  {
            return stateVars;
        }

        const std::vector<BDD_ID> &Reachability::getInputs() const  {
            return inputVars;
        }

        void Reachability::setInitState(const std::vector<bool> &stateVector)  {
           if (stateVector.size() != initState.size()) {
             throw std::invalid_argument("stateSize must be equal to stateVector size");
           }

           currentReachableSet = True();
           for (int i = 0; i < stateVars.size(); i++) {
             currentReachableSet = and2(currentReachableSet, stateVector[i] ? stateVars[i] : neg(stateVars[i]));
           }
           // as already mentioned 011 = ~s1&s2&s3
        }

        void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)  {
          if (transitionFunctions.size() != stateVars.size()) {
            throw std::invalid_argument("stateSize must be equal to transitionFunctions size");
          }

          transitionRelation = True();
          for (int i = 0; i < stateVars.size(); i++) {
            // xnor relation = xnor(transitionFunctions[i], nextStateVars[i])
            BDD_ID relation = ite(transitionFunctions[i], nextStateVars[i], neg(nextStateVars[i]));
            transitionRelation = and2(relation, transitionRelation); // multiplication
          }

        }

        bool Reachability::isReachable(const std::vector<bool> &stateVector)  {
            if (stateVector.size() != stateVars.size()) {
              throw std::invalid_argument("Reachability::isReachable()");
            }

            // create bdd for input stateVector
            BDD_ID stateBDD = True();
            for (int i = 0; i < stateVars.size(); i++) {
              stateBDD = and2(stateBDD, stateVector[i] ? stateVars[i] : neg(stateVars[i])); // this creates bdd for stateVector
            }

            BDD_ID intersection = and2(currentReachableSet, stateBDD);

            return intersection == stateBDD;
        }

        int Reachability::stateDistance(const std::vector<bool> &stateVector) {
          if (!isReachable(stateVector)) {
            return -1;
          }

          BDD_ID targetState = True();
          for (int i = 0; i < stateVars.size(); i++) {
            targetState = and2(targetState, stateVector[i] ? stateVars[i] : neg(stateVars[i]));
          }

          int distance = 0;
          BDD_ID reachableSet = currentReachableSet;

          while (true) {
            if (and2(reachableSet, targetState) == targetState) {
              break;
            }

            BDD_ID nextStateImage = computeImageNextStates(reachableSet);
            BDD_ID currentStateImage = computeImageCurrentStates(nextStateImage);

            reachableSet = and2(reachableSet, currentStateImage);
            distance++;
          }

          return distance;
        }