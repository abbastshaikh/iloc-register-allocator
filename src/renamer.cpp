#include <Renamer.hpp>
#include <Operation.hpp>
#include <algorithm>

void Renamer::rename(InternalRepresentation& rep){

    int VRName = 0;
    int maxSR = -1;
    int index = rep.operations.size();

    for (Operation op: rep.operations) {
        maxSR = std::max({maxSR, op.op1.SR, op.op2.SR, op.op3.SR});
    }

    int SRToVR [maxSR + 1];
    int LU [maxSR + 1];
    for (int i = 0; i <= maxSR; i ++) {
        SRToVR[i] = -1;
        LU[i] = -1;
    }

    for (auto op = rep.operations.rbegin(); op != rep.operations.rend(); ++op) {

        Operand& o = op->op3;
        if (op->opcode != Opcode::STORE && o.SR != -1) {
            if (SRToVR[o.SR] == -1) {
                SRToVR[o.SR] = VRName++;
            }
            o.VR = SRToVR[o.SR];
            o.NU = LU[o.SR];
            SRToVR[o.SR] = -1;
            LU[o.SR]= -1;
        }

        std::list<Operand*> uses;
        switch (op->opcode) {
            case Opcode::LOAD:
                uses.push_back(&op->op1);
                break;
            case Opcode::STORE:
                uses.push_back(&op->op1);
                uses.push_back(&op->op3);
                break;
            case Opcode::LOADI:
                break;
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MULT:
            case Opcode::LSHIFT:
            case Opcode::RSHIFT:
                uses.push_back(&op->op1);
                uses.push_back(&op->op2);
                break;
            default:
                break;
        }

        for (Operand* o: uses) {
            if (SRToVR[o->SR] == -1) {
                SRToVR[o->SR] = VRName++;
            }
            o->VR = SRToVR[o->SR];
            o->NU = LU[o->SR];
        }

        for (Operand* o: uses) {
            LU[o->SR] = index;
        }
        index--;
    }
}