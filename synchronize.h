#ifndef _SYNCHRONIZED_H__
#define _SYNCHRONIZED_H__

#include <vector>
#include <cinttypes>

void find_syncronize_sequence_trivial(
                    const std::vector<std::vector<int>> &edge,
                    const std::vector<int> &start_states,
                    std::vector<uint8_t> &states_masks);

#endif
