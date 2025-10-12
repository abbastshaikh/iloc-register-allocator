#include <Allocator.hpp>
#include <Operation.hpp>
#include <vector>
#include <stack>

void Allocator::allocate(InternalRepresentation& rep, int k){

    int PR;
    
    // Calculate number of free registers based on MAXLIVE
    nPR = k;
    if (rep.maxLive > k) {
        nPR = k - 1;
    }

    // Initialize maps
    VRToPR.assign(rep.maxVR + 1, -1);
    VRToSpillLoc.assign(rep.maxVR + 1, -1);
    PRToVR.assign(nPR, -1);
    PRNU.assign(nPR, -1);
    PRMark.assign(nPR, false);

    // Initialize stacks
    PRStack = std::stack<int>();
    spillLocStack = std::stack<int>();
    for (int i = 0; i <= rep.maxVR; i ++) spillLocStack.push(65536 - i);
    for (int i = 0; i < nPR; i ++) PRStack.push(i);

    // For each operation
    auto end = rep.operations.end();
    for (auto op = rep.operations.begin(); op != end; ++op) {

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
    }
}

int Allocator::getAPR (InternalRepresentation& rep, std::list<Operation>::iterator op, int vr, int nu){
    
    int x;
    
    if (!PRStack.empty()) {
        x = PRStack.top();
        PRStack.pop();
    } else {
        x = findAPR();
        spill(rep, op, vr, x);
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

void Allocator::spill (InternalRepresentation& rep, std::list<Operation>::iterator op, int vr, int pr){
    
    int loc;
    
    loc = spillLocStack.top();
    spillLocStack.pop();
    VRToSpillLoc[vr] = loc;
    rep.operations.insert(std::next(op), 
        Operation{
            Opcode::LOADI, 
            {loc, -1, -1, -1},
            {},
            {-1, -1, nPR, -1}
        }
    );
    rep.operations.insert(std::next(op), 
        Operation{
            Opcode::STORE, 
            {-1, -1, pr, -1},
            {},
            {-1, -1, nPR, -1}
        }
    );
}

void Allocator::restore (InternalRepresentation& rep, std::list<Operation>::iterator op, int vr, int pr){
    
    spillLocStack.push(VRToSpillLoc[vr]);
    VRToSpillLoc[vr] = -1;

    rep.operations.insert(std::next(op), 
        Operation{
            Opcode::LOADI, 
            {VRToSpillLoc[vr], -1, -1, -1},
            {},
            {-1, -1, nPR, -1}
        }
    );
    rep.operations.insert(std::next(op), 
        Operation{
            Opcode::LOAD, 
            {-1, -1, nPR, -1},
            {},
            {-1, -1, pr, -1}
        }
    );
}