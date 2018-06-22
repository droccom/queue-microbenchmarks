#ifndef INCLUDE_QM_TWO_STAGE_TORUS_HPP_
#define INCLUDE_QM_TWO_STAGE_TORUS_HPP_

#include <cstdint>
#include <iostream>
#include <cassert>

#include "qm/config.h"
#include "qm/qm_thread.hpp"
#include "qm/experiment.hpp"
#include "qm/queues.hpp"

/*
 * This benchmark is a simple 2-stage torus:
 * - the fw thread streams data to the bw thread, through a SPSC queue;
 * - the bw thread forwards the incoming data back to fw, through another SPSC.
 *
 * The data movement is performed by push/pop bursts of random length.
 */

template<typename queue_t>
struct fw_thread: public qm_thread<queue_t> {
	fw_thread(unsigned int seed) :
			qm_thread<queue_t>(seed) {
	}

	void routine() {
		do {
			this->push_burst();
			termination_cnt -= this->pop_burst();
		} while (termination_cnt);
	}

	void set_termination_cnt(uint64_t cnt) {
		termination_cnt = cnt;
	}

	uint64_t termination_cnt = 0;
};

template<typename queue_t>
struct bw_thread: public qm_thread<queue_t> {
	bw_thread(unsigned int seed) :
			qm_thread<queue_t>(seed) {
	}

	void routine() {
		while (termination_cnt)
			termination_cnt -= this->fw_burst();
	}

	void set_termination_cnt(uint64_t cnt) {
		termination_cnt = cnt;
	}

	uint64_t termination_cnt = 0;
};

template<typename queue_t>
double kernel_(unsigned seed) {
	queue_t *fw, *bw;
	queue_init(fw, qm_in_len);
	queue_init(bw, qm_in_len);

	fw_thread<queue_t> p { seed };
	p.add_src(bw);
	p.add_dst(fw);
	p.set_termination_cnt(qm_in_len);

	bw_thread<queue_t> c { seed + 1 };
	c.add_src(fw);
	c.add_dst(bw);
	c.set_termination_cnt(qm_in_len);

	auto t0 = std::chrono::high_resolution_clock::now();

	p.run();
	c.run();

	p.wait();
	c.wait();

	auto t1 = std::chrono::high_resolution_clock::now();

	assert(p.acc() == p.in_checksum());

	std::chrono::duration<double> t = t1 - t0;
	return t.count();
}

void two_stage_torus() {
	printf("> FF bounded SPSC\n");
	experiment<100>(kernel_<ff_bounded_spsc>);

	printf("> FF unbounded SPSC\n");
	experiment<100>(kernel_<ff_unbounded_spsc>);

	printf("> GMT bounded MPMC\n");
	experiment<100>(kernel_<gmt_bounded_mpmc>);
}

#endif /* INCLUDE_QM_TWO_STAGE_TORUS_HPP_ */
