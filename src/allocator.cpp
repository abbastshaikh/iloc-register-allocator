#include <Allocator.hpp>
#include <Operation.hpp>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>

void Allocator::allocate(InternalRepresentation& rep, int k){

    int PR;
    
    // Calculate number of free registers based on MAXLIVE
    nPR = k;
    if (rep.maxLive > k) {
        nPR = k - 1;
    }
    spillLoc = 65536;

    // Initialize maps
    VRToPR.assign(rep.maxVR + 1, -1);
    VRToSpillLoc.assign(rep.maxVR + 1, -1);
    VRToImmediate.assign(rep.maxVR + 1, -1);
    PRToVR.assign(nPR, -1);
    PRNU.assign(nPR, -1);
    PRMark.assign(nPR, false);

    // Initialize stack
    PRStack = std::stack<int>();
    for (int i = nPR - 1; i >= 0; i --) PRStack.push(i);

    // For each operation
    auto end = rep.operations.end();
    for (auto op = rep.operations.begin(); op != end; ) {

        if (op->opcode == Opcode::LOADI) {
            VRToImmediate[op->op3.VR] = op->op1.SR;
            op = rep.operations.erase(op);
        } else {
            // Define defs and uses
            std::vector<Operand*> uses;
            std::vector<Operand*> defs;
            switch (op->opcode) {
                case Opcode::LOAD:
                    uses.push_back(&op->op1);
                    defs.push_back(&op->op3);
                    break;
                case Opcode::STORE:
                    uses.push_back(&op->op1);
                    uses.push_back(&op->op3);
                    break;
                case Opcode::LOADI:
                    defs.push_back(&op->op3);
                    break;
                case Opcode::ADD:
                case Opcode::SUB:
                case Opcode::MULT:
                case Opcode::LSHIFT:
                case Opcode::RSHIFT:
                    uses.push_back(&op->op1);
                    uses.push_back(&op->op2);
                    defs.push_back(&op->op3);
                    break;
                default:
                    break;
            }

            // Clear the mark in each PR
            for (int i = 0; i < nPR; i ++) {
                PRMark[i] = false;
            }

            // Allocate uses
            for (Operand* o: uses) {
                PR = VRToPR[o->VR];
                if (PR == -1){
                    o->PR = getAPR(rep, op, o->VR, o->NU);
                    restore(rep, op, o->VR, o->PR);
                } else {
                    o->PR = PR;
                }
                PRMark[o->PR] = true;
            }

            // Free PR on last use
            for (Operand* o: uses) {
                if (o->NU == -1  and PRToVR[o->PR] != -1) {
                    freeAPR(o->PR);
                }
            }

            // Clear the mark in each PR
            for (int i = 0; i < nPR; i ++) {
                PRMark[i] = false;
            }

            // Allocate defs
            for (Operand* o: defs) {
                o->PR = getAPR(rep, op, o->VR, o->NU);
                PRMark[o->PR] = true;
            }

            ++op;
        }
    }
}

int Allocator::getAPR (InternalRepresentation& rep, std::list<Operation>::iterator& op, int vr, int nu){
    
    int x;
    int VRToSpill;
    
    if (!PRStack.empty()) {
        x = PRStack.top();
        PRStack.pop();
    } else {
        x = findAPR();
        VRToSpill = PRToVR[x]; // VR to be spilled

        // Spill only if the VR is not rematerializable and not already spilled
        if (VRToImmediate[VRToSpill] == -1 && VRToSpillLoc[VRToSpill] == -1){
            spill(rep, op, x);
        } else { // Otherwise, just reset VRToPR
            VRToPR[VRToSpill] = -1;
        }
    }

    VRToPR[vr] = x;
    PRToVR[x] = vr;
    PRNU[x] = nu;

    return x;
}

int Allocator::findAPR () {
    int maxPR = -1;
    int maxNU = -1;
    for (int i = 0; i < nPR; i++){
        if (!PRMark[i]) {
            if (PRNU[i] == -1) {
                maxPR = i;
                break;
            }
            else if (PRNU[i] > maxNU) {
                maxPR = i;
                maxNU = PRNU[i];
            }
        }
    }
    return maxPR;
}

void Allocator::freeAPR (int pr){
    VRToPR[PRToVR[pr]] = -1;
    PRToVR[pr] = -1;
    PRNU[pr] = -1;
    PRStack.push(pr);
}

void Allocator::spill (InternalRepresentation& rep, std::list<Operation>::iterator& op, int pr){
    
    VRToSpillLoc[PRToVR[pr]] = spillLoc;
    VRToPR[PRToVR[pr]] = -1;

    rep.operations.insert(op, 
        Operation{
            Opcode::LOADI, 
            {spillLoc, -1, -1, -1},
            {},
            {-1, -1, nPR, -1}
        }
    );

    rep.operations.insert(op, 
        Operation{
            Opcode::STORE, 
            {-1, -1, pr, -1},
            {},
            {-1, -1, nPR, -1}
        }
    );
    
    spillLoc += 4;
}

void Allocator::restore (InternalRepresentation& rep, std::list<Operation>::iterator& op, int vr, int pr){

    if (VRToImmediate[vr] != -1) {
        rep.operations.insert(op, 
            Operation{
                Opcode::LOADI, 
                {VRToImmediate[vr], -1, -1, -1},
                {},
                {-1, -1, pr, -1}
            }
        );
    } else {
        rep.operations.insert(op, 
            Operation{
                Opcode::LOADI, 
                {VRToSpillLoc[vr], -1, -1, -1},
                {},
                {-1, -1, nPR, -1}
            }
        );
        rep.operations.insert(op, 
            Operation{
                Opcode::LOAD, 
                {-1, -1, nPR, -1},
                {},
                {-1, -1, pr, -1}
            }
        );
    }
}