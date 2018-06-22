#include "qm/two_stage_torus.hpp"
#include "qm/workpool_vs_scheduler.hpp"

int main(int argc, char *argv[]) {
	printf("*** 2-STAGE TORUS ***\n");
	two_stage_torus();

	printf("*** WORKPOOL vs SCHEDULER ***\n");
	workpool_vs_scheduler();

	return 0;
}
