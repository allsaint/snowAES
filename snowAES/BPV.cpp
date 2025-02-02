#include "BPV.hpp"

BitProbability::BitProbability()
    : bpv_in(1024, 0), bpv_out() {} // Initialize member vectors directly

BitProbability::BitProbability(int size, int init_val)
    : bpv_in(size, init_val), bpv_out() {} // Properly initialize vectors
