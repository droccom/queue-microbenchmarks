#ifndef INCLUDE_QM_QUEUE_WRAPPERS_GMT_HPP_
#define INCLUDE_QM_QUEUE_WRAPPERS_GMT_HPP_

#include "qm/queue_wrapper.h"

/*
 * GMT - MPMC bounded
 */
#include "gmt/queue.h"

typedef qmpmc_t gmt_bounded_mpmc;

template<>
inline void queue_init(gmt_bounded_mpmc *&q, unsigned len) {
	q = (qmpmc_t *)malloc(sizeof(qmpmc_t));
	qmpmc_init(q, (uint32_t)len);
}

template<>
inline void queue_destroy(gmt_bounded_mpmc *q) {
	qmpmc_destroy(q);
	free(q);
}

template<>
inline void queue_push(gmt_bounded_mpmc *q, void *x) {
	qmpmc_push(q, x);
}

template<>
inline void *queue_pop(gmt_bounded_mpmc *q) {
	void *res;
	if(!qmpmc_pop(q, &res))
		return nullptr;
	return res;
}

#endif /* INCLUDE_QM_QUEUE_WRAPPERS_GMT_HPP_ */
