#ifndef INCLUDE_QM_CONFIG_H_
#define INCLUDE_QM_CONFIG_H_

#include <cstdint>

constexpr unsigned qm_in_len = (1 << 20);
constexpr unsigned qm_np = 8;

constexpr unsigned qm_max_burst = 128;
constexpr uint64_t qm_max_val = 16;

#endif /* INCLUDE_QM_CONFIG_H_ */
