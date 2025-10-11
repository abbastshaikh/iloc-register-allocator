#pragma once

#include <InternalRepresentation.hpp>
#include <Renamer.hpp>
#include <stack>

class Allocator {
public:
    void allocate(InternalRepresentation& rep, int k);
};