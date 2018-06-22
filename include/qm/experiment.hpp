#ifndef INCLUDE_QM_EXPERIMENT_HPP_
#define INCLUDE_QM_EXPERIMENT_HPP_

#include <cmath>

template<unsigned repetitions, typename F>
void experiment(F &&f) {
	f(0); //warm up

	double obs[repetitions];
	for (unsigned i = 0; i < repetitions; ++i) {
		obs[i] = f(i);
		printf("\r%d\t/\t%d", i + 1, repetitions);
		fflush(stdout);
	}
	printf("\n");

	/* stats */
	double avg = 0, cv = 0;
	for (auto o : obs)
		avg += o;
	avg /= repetitions;
	for (auto o : obs) {
		auto d = o - avg;
		cv += d * d;
	}
	cv = std::sqrt(cv) / repetitions / avg;

	/* print */
	printf("AVG (s) %g\t\tCV (%%) %g\n", avg, cv * 100);
}

#endif /* INCLUDE_QM_EXPERIMENT_HPP_ */
