#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <getopt.h>
#include <assert.h>
#include <sys/sysinfo.h>
#define DATA_SEPARATOR ","
#define PIN_THREADS
#define PIN_EVENLY

static uint64_t shared_counter = 0;
static uint64_t maxcounter = 100;
static uint64_t workers = 1;
static bool show_info = false, show_headers = false;
static double writeProbability = 0.5;

static pthread_spinlock_t spinlock;

void *worker_func(void *args);
void parse_opts(int argc, char* const argv[]);
bool operation_is_a_write();

int main(int argc, char* const argv[]) {
	parse_opts(argc, argv);

	pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

	shared_counter = 0;
	int thread_args[workers];
	pthread_t threads[workers];
	uint64_t thread_counts[workers];

	//spawn worker threads
	const int cpus = get_nprocs();
	for (int i=0; i<workers; ++i) {
		thread_args[i] = i;
		int result = pthread_create(&threads[i], NULL, worker_func, &thread_args[i]);
		assert(!result);

		#ifdef PIN_THREADS

		//choose CPU core
		#ifdef PIN_EVENLY
		const int core = i % cpus;
		#else
		const int core = 0;
		#endif

		//set affinity
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(core, &cpuset);
		pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);

		#endif
	}

	//wait for all threads to complete
	long counts_total = 0;
	for (int i=0; i<workers; ++i) {
		int result = pthread_join(threads[i], (void*)&thread_counts[i]);
		assert(!result);
		counts_total += thread_counts[i]; //sum total number of updates
	}

	//display statistics
	if (show_headers)
		printf("update ratio" DATA_SEPARATOR "average imbalance\n");

	if (show_info) {
		//print information about lost updates
		printf("%f" DATA_SEPARATOR, (double)counts_total / maxcounter);
	
		//print information about load imbalance
		uint64_t imbalance_total = 0;

		for (int i=0; i<workers; i++) {
			const uint64_t expected_count = maxcounter / workers;
			imbalance_total += labs(thread_counts[i] - expected_count);
		}
		printf("%ld\n", imbalance_total / workers);
	}
	return 0;
}

bool operation_is_a_write() {
	return (double)rand() / RAND_MAX < writeProbability;
}

void *worker_func(void *args) {
	int id = *((int*)args);

	uint64_t my_op_max = maxcounter / workers;
	uint64_t my_counter = 0;
	uint64_t my_operations = 0;

	while (true) {
		pthread_spin_lock(&spinlock);
		if (my_operations >= my_op_max) {
			pthread_spin_unlock(&spinlock);
			break;
		}
		if (operation_is_a_write()) {
			++my_counter;
			++shared_counter;
		}
		++my_operations;
		pthread_spin_unlock(&spinlock);
	}

	pthread_exit((void*)my_counter);
}

void parse_opts(int argc, char* const argv[]) {
	static struct option longopts[] = {
		{"maxcounter", required_argument, NULL, 'm'},
		{"workers", required_argument, NULL, 'w'},
		{"write-prob", required_argument, NULL, 'p'},
		{"show-info", no_argument, NULL, 'i'},
		{"show-headers", no_argument, NULL, 'H'},
		{"help", no_argument, NULL, 'h'}
	};

	int longindex = 0;
	char flag = 0;
	while ((flag = getopt_long(argc, argv, "m:w:p:iH", longopts, &longindex)) != -1) {
		switch (flag) {
			case 'm':
				maxcounter = atoll(optarg);
				break;
			case 'w':
				workers = atoll(optarg);
				break;
			case 'p':
				writeProbability = atof(optarg);
				break;
			case 'i':
				show_info = true;
				break;
			case 'H':
				show_headers = true;
				break;
			case 'h':
			default:
				printf("Usage: lab1 [--workers=n] [--maxcounter=n] [--show-info] [--show-headers]\n");
				exit(-1);
		}
	}
}
