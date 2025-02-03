#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"

namespace ClassProject {

    class Reachability : public ReachabilityInterface {

        std::vector<BDD_ID> stateVars;
        std::vector<BDD_ID> nextStateVars;
        std::vector<BDD_ID> inputVars;
        std::vector<bool> initState;
        BDD_ID transitionRelation = False();
        BDD_ID currentReachableSet = False();

        // Manager manager;


      BDD_ID computeImage(BDD_ID set) {
        // Compute image ∃x ∃s cS(s) ⋅ τ(s, x, s')
        BDD_ID temp = and2(set, transitionRelation);
        for (const auto &var : stateVars) {
          temp = or2(coFactorTrue(temp, var), coFactorFalse(temp, var));
        }
        for (const auto &var : inputVars) {
          temp = or2(coFactorTrue(temp, var), coFactorFalse(temp, var));
        }
        return temp;
     }

      BDD_ID computeImageNextStates(BDD_ID &currentStates) {
        // Compute image ∃x ∃s cS(s) ⋅ τ(s, x, s')
        BDD_ID temp = and2(currentStates, transitionRelation);
        for (const auto &var : stateVars) {
          temp = or2(coFactorTrue(temp, var), coFactorFalse(temp, var));
        }
        for (const auto &var : inputVars) {
          temp = or2(coFactorTrue(temp, var), coFactorFalse(temp, var));
        }
        return temp;
     }

     BDD_ID computeImageCurrentStates(BDD_ID &nextStatesImage) {
		  BDD_ID currentImage = nextStatesImage;

        BDD_ID eqivalence = True();
        for (int i = 0 ; i < stateVars.size() ; i++) {
          eqivalence = and2(eqivalence, xnor2(stateVars[i], nextStateVars[i]));
        }

        currentImage = and2(eqivalence, currentImage);

        for (const auto &var : nextStateVars) {
          currentImage = or2(coFactorTrue(currentImage, var), coFactorFalse(currentImage, var));
        }

        return currentImage;
     }



        public:
        Reachability(unsigned int stateSize, unsigned int inputSize = 0);
        const std::vector<BDD_ID> &getStates() const override;
        const std::vector<BDD_ID> &getInputs() const override;
        bool isReachable(const std::vector<bool> &stateVector) override;
        int stateDistance(const std::vector<bool> &stateVector) override;
        void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;
        void setInitState(const std::vector<bool> &stateVector) override;
    };

}
#endif
