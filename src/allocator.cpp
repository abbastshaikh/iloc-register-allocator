#include <Allocator.hpp>
#include <Operation.hpp>
#include <stack>

void Allocator::allocate(InternalRepresentation& rep, int k){

    int VRToPR [rep.maxVR + 1];
    int PRToVR [k - 1];
    int PRNU [k - 1];
    std::stack<int> PRStack;

    for (int i = 0; i <= rep.maxVR; i ++) {
        VRToPR[i] = -1;
    }
    for (int i = 0; i < k; i ++) {
        PRToVR[i] = -1;
        PRNU[i] = -1;
        PRStack.push(i);
    }

    return;
}