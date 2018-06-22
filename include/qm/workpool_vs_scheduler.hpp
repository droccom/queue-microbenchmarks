#ifndef INCLUDE_QM_WORKPOOL_VS_SCHEDULER_HPP_
#define INCLUDE_QM_WORKPOOL_VS_SCHEDULER_HPP_

#include <atomic>
#include <cstdint>
#include <iostream>

#include "qm/config.h"
#include "qm/qm_thread.hpp"
#include "qm/queue_wrappers.hpp"

template<typename queue_t>
struct pc_thread: public qm_thread<queue_t> {
	pc_thread(unsigned int seed) :
			qm_thread<queue_t>(seed) {
	}

	void routine() {
		do {
			this->push_burst();
			*termination_cnt -= this->pop_burst();
		} while (termination_cnt->load());
	}

	void set_termination_cnt(std::atomic<uint64_t> *cnt) {
		termination_cnt = cnt;
	}

	std::atomic<uint64_t> *termination_cnt = nullptr;
};

template<typename queue_t>
struct sched_thread: public qm_thread<queue_t> {
	sched_thread() :
			qm_thread<queue_t>(0) {
	}

	void routine() {
		do {
			this->fw_burst();
		} while (termination_cnt->load());
	}

	void set_termination_cnt(std::atomic<uint64_t> *cnt) {
		termination_cnt = cnt;
	}

	std::atomic<uint64_t> *termination_cnt = nullptr;
};

template<typename queue_t>
double workpool(unsigned seed) {
	auto m = std::max((unsigned) 1, qm_np / 2);
	std::atomic<uint64_t> cnt { qm_np * qm_in_len };

	std::vector<queue_t *> queues { m, nullptr };
	for (auto &q : queues)
		queue_init(q, qm_np * qm_in_len);

	std::vector<pc_thread<queue_t>> procs;
	for (unsigned i = 0; i < qm_np; ++i) {
		procs.emplace_back(i + seed);
		auto &p = procs.back();
		p.set_termination_cnt(&cnt);
		for (auto q : queues) {
			p.add_src(q);
			p.add_dst(q);
		}
	}

	auto t0 = std::chrono::high_resolution_clock::now();

	for (auto &p : procs)
		p.run();

	for (auto &p : procs)
		p.wait();

	auto t1 = std::chrono::high_resolution_clock::now();

	uint64_t exp = 0, obs = 0;
	for (auto &p : procs) {
		exp += p.in_checksum();
		obs += p.acc();
	}
	assert(exp == obs);

	std::chrono::duration<double> t = t1 - t0;
	return t.count();
}

template<typename queue_t>
double scheduler_(unsigned seed) {
	std::atomic<uint64_t> cnt { qm_np * qm_in_len };
	sched_thread<queue_t> scheduler;
	queue_t *fw_queues[qm_np], *bw_queues[qm_np];

	for (auto &q : fw_queues)
		queue_init(q, qm_np * qm_in_len);
	for (auto &q : bw_queues)
		queue_init(q, qm_np * qm_in_len);

	std::vector<pc_thread<queue_t>> procs;
	for (unsigned i = 0; i < qm_np; ++i) {
		procs.emplace_back(i + seed);
		auto &p = procs.back();
		p.set_termination_cnt(&cnt);
		p.add_dst(fw_queues[i]);
		p.add_src(bw_queues[i]);
	}

	scheduler.set_termination_cnt(&cnt);
	for (auto q : fw_queues)
		scheduler.add_src(q);
	for (auto q : bw_queues)
		scheduler.add_dst(q);

	auto t0 = std::chrono::high_resolution_clock::now();

	for (auto &p : procs)
		p.run();
	scheduler.run();

	for (auto &p : procs)
		p.wait();
	scheduler.wait();

	auto t1 = std::chrono::high_resolution_clock::now();

	uint64_t exp = 0, obs = 0;
	for (auto &p : procs) {
		exp += p.in_checksum();
		obs += p.acc();
	}
	assert(exp == obs);

	std::chrono::duration<double> t = t1 - t0;
	return t.count();
}

void workpool_vs_scheduler() {
	printf("> scheduler - FF bounded SPSC\n");
	experiment<10>(scheduler_<ff_bounded_spsc>);

	printf("> scheduler - FF unbounded SPSC\n");
	experiment<10>(scheduler_<ff_unbounded_spsc>);

	printf("> workpool - GMT bounded MPMC\n");
	experiment<10>(workpool<gmt_bounded_mpmc>);
}

#endif /* INCLUDE_QM_WORKPOOL_VS_SCHEDULER_HPP_ */
