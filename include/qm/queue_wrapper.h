#ifndef INCLUDE_QM_QUEUE_WRAPPER_H_
#define INCLUDE_QM_QUEUE_WRAPPER_H_

template<typename T>
inline void queue_init(T *&, unsigned);

template<typename T>
inline void queue_destroy(T *);

template<typename T>
inline void queue_push(T *, void *);

template<typename T>
inline void* queue_pop(T *);

#endif /* INCLUDE_QM_QUEUE_WRAPPER_H_ */
