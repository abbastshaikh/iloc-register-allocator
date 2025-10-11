#pragma once

#include <InternalRepresentation.hpp>

class Allocator {
public:
    void allocate(InternalRepresentation& rep, int k, int maxLive);
};