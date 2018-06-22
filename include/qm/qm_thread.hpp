#ifndef INCLUDE_QM_QM_THREAD_HPP_
#define INCLUDE_QM_QM_THREAD_HPP_

#include <vector>
#include <random>
#include <thread>

#include "config.h"
#include "qm/queue_wrappers.hpp"

template<typename queue_t>
void proxy_routine(void *th);

template<typename queue_t>
class qm_thread {
public:
	virtual ~qm_thread() {
	}

	qm_thread(unsigned int seed) :
			rng { seed } {
		std::uniform_int_distribution<unsigned> burst_len_d(0, qm_max_burst);
		std::uniform_int_distribution<uint64_t> val_d(1, qm_max_val);

		/* populate the pools */
		for (auto &x : burst_len_pool)
			x = burst_len_d(rng);
		for (auto &x : val_pool)
			x = val_d(rng);

		/* set start randomly */
		rr_burst_len = rng() % 1024;
		rr_val = rng() % 1024;

		/* compute input checksum */
		auto rr_val_backup = rr_val;
		for(unsigned i = 0; i < qm_in_len; ++i)
			in_checksum_ += get_val();
		rr_val = rr_val_backup;
	}

	void push_burst() {
		auto n_ = get_burst_len();
		auto n = std::min(n_, qm_in_len - n_pushed);
		for (unsigned i = 0; i < n; ++i)
			queue_push(get_dst(), (void *) get_val());
		n_pushed += n;
	}

	unsigned pop_burst() {
		unsigned n_popped = 0;
		void *popped;
		for (auto n = get_burst_len(); n > 0; --n) {
			if ((popped = queue_pop(get_src()))) {
				++n_popped;
				acc_ += (uint64_t) popped;
			}
		}
		return n_popped;
	}

	unsigned fw_burst() {
		unsigned n_popped = 0;
		void *popped;
		while ((popped = queue_pop(get_src()))) {
			queue_push(get_dst(), popped);
			++n_popped;
		}
		return n_popped;
	}

	virtual void routine() = 0;

	void run() {
		th = new std::thread(proxy_routine<queue_t>, this);
	}

	void wait() {
		th->join();
		delete th;
	}

	uint64_t in_checksum() const {
		return in_checksum_;
	}

	uint64_t acc() const {
		return acc_;
	}

	void add_src(queue_t *q) {
		src.push_back(q);
		++n_src;
	}

	void add_dst(queue_t *q) {
		dst.push_back(q);
		++n_dst;
	}

private:
	queue_t *get_dst() {
		queue_t *res = dst[rr_dst];
		if (++rr_dst == n_dst)
			rr_dst = 0;
		return res;
	}

	queue_t *get_src() {
		queue_t *res = src[rr_src];
		if (++rr_src == n_src)
			rr_src = 0;
		return res;
	}

	unsigned get_burst_len() {
		unsigned res = burst_len_pool[rr_burst_len];
		if (++rr_burst_len == 1024)
			rr_burst_len = 0;
		return res;
	}

	uint64_t get_val() {
		uint64_t res = val_pool[rr_val];
		if (++rr_val == 1024)
			rr_val = 0;
		return res;
	}

	std::thread *th = nullptr;

	std::vector<queue_t *> src, dst;
	std::mt19937 rng;
	uint64_t in_checksum_ = 0, acc_ = 0;
	unsigned n_pushed = 0;

	unsigned rr_src = 0, rr_dst = 0;
	unsigned n_src = 0, n_dst = 0;

	unsigned rr_burst_len = 0, rr_val = 0;
	unsigned burst_len_pool[1024];
	uint64_t val_pool[1024];
};

template<typename queue_t>
void proxy_routine(void *th_) {
	auto th = reinterpret_cast<qm_thread<queue_t> *>(th_);
	th->routine();
}

#endif /* INCLUDE_QM_QM_THREAD_HPP_ */
