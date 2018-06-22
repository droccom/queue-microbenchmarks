#ifndef INCLUDE_QM_QUEUE_WRAPPERS_FF_HPP_
#define INCLUDE_QM_QUEUE_WRAPPERS_FF_HPP_

#include "qm/queue_wrapper.h"

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

#endif /* INCLUDE_QM_QUEUE_WRAPPERS_FF_HPP_ */
