#pragma once

#include <InternalRepresentation.hpp>
#include <Renamer.hpp>
#include <stack>
#include <vector>

class Allocator {
private:
    int nPR;
    int spillLoc;
    std::vector<int> VRToPR;
    std::vector<int> VRToSpillLoc;
    std::vector<int> VRToImmediate;
    std::vector<int> PRToVR;
    std::vector<int> PRNU;
    std::vector<bool> PRMark;
    std::stack<int> PRStack;
    
    int getAPR (InternalRepresentation& rep, std::list<Operation>::iterator& op, int vr, int nu);
    int findAPR ();
    void freeAPR (int pr);
    void spill (InternalRepresentation& rep, std::list<Operation>::iterator& op, int pr);
    void restore (InternalRepresentation& rep, std::list<Operation>::iterator& op, int vr, int pr);

public:
    void allocate(InternalRepresentation& rep, int k);
};