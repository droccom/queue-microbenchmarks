/*
 * queue_wrappers.hpp
 *
 *  Created on: Jun 21, 2018
 *      Author: droc565
 */

#ifndef INCLUDE_QM_QUEUE_WRAPPERS_HPP_
#define INCLUDE_QM_QUEUE_WRAPPERS_HPP_

template<typename T>
inline void queue_init(T *&, unsigned);

template<typename T>
inline void queue_destroy(T *);

template<typename T>
inline void queue_push(T *, void *);

template<typename T>
inline void* queue_pop(T *);

/*
 * FF - SPSC bounded
 */
#define FF_UNCHECKED_PUSH
#include "ff/buffer.hpp"

typedef ff::SWSR_Ptr_Buffer ff_bounded_spsc;

template<>
inline void queue_init(ff::SWSR_Ptr_Buffer *&q, unsigned len) {
	q = new ff::SWSR_Ptr_Buffer(len);
	q->init();
}

template<>
inline void queue_destroy(ff::SWSR_Ptr_Buffer *q) {
	delete q;
}

template<>
inline void queue_push(ff::SWSR_Ptr_Buffer *q, void *x) {
	q->push(x);
}

template<>
inline void *queue_pop(ff::SWSR_Ptr_Buffer *q) {
	void *res;
	if(!q->pop(&res))
		return nullptr;
	return res;
}

/*
 * FF - SPSC unbounded
 */
#include "ff/ubuffer.hpp"

typedef ff::uSWSR_Ptr_Buffer ff_unbounded_spsc;

template<>
inline void queue_init(ff::uSWSR_Ptr_Buffer *&q, unsigned len) {
	q = new ff::uSWSR_Ptr_Buffer(1024, true);
	q->init();
}

template<>
inline void queue_destroy(ff::uSWSR_Ptr_Buffer *q) {
	delete q;
}

template<>
inline void queue_push(ff::uSWSR_Ptr_Buffer *q, void *x) {
	q->push(x);
}

template<>
inline void *queue_pop(ff::uSWSR_Ptr_Buffer *q) {
	void *res;
	if(!q->pop(&res))
		return nullptr;
	return res;
}

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



#endif /* INCLUDE_QM_QUEUE_WRAPPERS_HPP_ */
