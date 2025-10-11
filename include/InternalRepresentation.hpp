#pragma once

#include <Operation.hpp>
#include <vector>

struct InternalRepresentation {
    std::vector<Operation> operations;
    int maxSR;
};