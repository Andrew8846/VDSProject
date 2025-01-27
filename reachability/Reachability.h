#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

    class Reachability : public ReachabilityInterface {
        private:
//        unsigned int stateSize;
//        unsigned int inputSize;
        std::vector<BDD_ID> stateVars;
        std::vector<BDD_ID> nextStateVars;
        std::vector<BDD_ID> inputVars;
        BDD_ID transitionRelation;
        BDD_ID currentReachableSet;

        Manager manager;


      BDD_ID computeImage(BDD_ID set) {
        // Compute image ∃x ∃s cS(s) ⋅ τ(s, x, s')
        BDD_ID temp = manager.and2(set, transitionRelation);
        for (const auto &var : stateVars) {
          temp = manager.or2(manager.coFactorTrue(temp, var), manager.coFactorFalse(temp, var));
        }
        for (const auto &var : inputVars) {
          temp = manager.or2(manager.coFactorTrue(temp, var), manager.coFactorFalse(temp, var));
        }
        return temp;
     }

      BDD_ID computeImageNextStates(BDD_ID &currentStates) {
        // Compute image ∃x ∃s cS(s) ⋅ τ(s, x, s')
        BDD_ID temp = manager.and2(currentStates, transitionRelation);
        for (const auto &var : stateVars) {
          temp = manager.or2(manager.coFactorTrue(temp, var), manager.coFactorFalse(temp, var));
        }
        for (const auto &var : inputVars) {
          temp = manager.or2(manager.coFactorTrue(temp, var), manager.coFactorFalse(temp, var));
        }
        return temp;
     }

     BDD_ID computeImageCurrentStates(BDD_ID &nextStatesImage) {
		BDD_ID currentImage = nextStatesImage;

        BDD_ID eqivalence = manager.True();
        for (int i = 0 ; i < stateSize ; i++) {
          eqivalence = manager.and2(eqivalence, manager.xnor2(stateVars[i], nextStateVars[i]));
        }

        currentImage = manager.and2(eqivalence, currentImage);

        for (const auto &var : nextStateVars) {
          currentImage = manager.or2(manager.coFactorTrue(currentImage, var), manager.coFactorFalse(currentImage, var));
        }

        return currentImage;
     }



        public:
        Reachability(unsigned int stateSize, unsigned int inputSize = 0)
              : ReachabilityInterface(stateSize, inputSize){

          if (stateSize == 0) {
              throw std::invalid_argument("stateSize must be greater than zero");
          }

          for (int i = 0; i < stateSize; i++) {
            stateVars.push_back(manager.createVar("s" + std::to_string(i)));
            nextStateVars.push_back(manager.createVar("s'" + std::to_string(i)));
          }

         for (int i = 0; i < inputSize; i++) {
           inputVars.push_back(manager.createVar("v" + std::to_string(i)));
         }

         // create initial reachable state all 0
         // ~s1 & ~s2 & ~s3 ...
         currentReachableSet = manager.True();
         for (const auto &var : stateVars) {
           currentReachableSet = manager.and2(currentReachableSet, manager.neg(var));
         }

         transitionRelation = 0; // todo needs to be done
        }

        const std::vector<BDD_ID> &getStates() const override {
            return stateVars;
        }

        const std::vector<BDD_ID> &getInputs() const override {
            return inputVars;
        }

        void setInitState(const std::vector<bool> &stateVector) override {
           if (stateVector.size() != stateSize) {
             throw std::invalid_argument("stateSize must be equal to stateVector size");
           }

           currentReachableSet = manager.True();
           for (int i = 0; i < stateSize; i++) {
             currentReachableSet = manager.and2(currentReachableSet, stateVector[i] ? stateVars[i] : manager.neg(stateVars[i]));
           }
           // as already mentioned 011 = ~s1&s2&s3
        }

        void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override {
          if (transitionFunctions.size() != stateSize) {
            throw std::invalid_argument("stateSize must be equal to transitionFunctions size");
          }

          transitionRelation = manager.True();
          for (int i = 0; i < stateSize; i++) {
            // xnor relation = manager.xnor(transitionFunctions[i], nextStateVars[i])
            BDD_ID relation = manager.ite(transitionFunctions[i], nextStateVars[i], manager.neg(nextStateVars[i]));
            transitionRelation = manager.and2(relation, transitionRelation); // multiplication
          }

        }

        bool isReachable(const std::vector<bool> &stateVector) override {
            if (stateVector.size() != stateSize) {
              throw std::invalid_argument("Reachability::isReachable()");
            }

            // create bdd for input stateVector
            BDD_ID stateBDD = manager.True();
            for (int i = 0; i < stateSize; i++) {
              stateBDD = manager.and2(stateBDD, stateVector[i] ? stateVars[i] : manager.neg(stateVars[i])); // this creates bdd for stateVector
            }

            BDD_ID intersection = manager.and2(currentReachableSet, stateBDD);

            return intersection == stateBDD;
        }

        int stateDistance(const std::vector<bool> &stateVector) override {
          if (!isReachable(stateVector)) {
            return -1;
          }

          BDD_ID targetState = manager.True();
          for (int i = 0; i < stateSize; i++) {
            targetState = manager.and2(targetState, stateVector[i] ? stateVars[i] : manager.neg(stateVars[i]));
          }

          int distance = 0;
          BDD_ID reachableSet = currentReachableSet;

          while (true) {
            if (manager.and2(reachableSet, targetState) == targetState) {
              break;
            }

            BDD_ID nextStateImage = computeImageNextStates(reachableSet);
            BDD_ID currentStateImage = computeImageCurrentStates(nextStateImage);

            reachableSet = manager.and2(reachableSet, currentStateImage);
            distance++;
          }

          return distance;
        }


    };

}
#endif
