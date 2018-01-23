#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>
#include <assert.h>

static int shared_counter = 0;
static int maxcounter = 100;
static int workers = 1;
static bool show_counts = false;
static bool show_lost = false;
static bool show_load = false;

void *worker_func(void *args);
void parse_opts(int argc, char* const argv[]);

int main(int argc, char* const argv[]) {
	parse_opts(argc, argv);

	shared_counter = 0;
	int thread_args[workers];
	pthread_t threads[workers];
	int thread_counts[workers];

	//spawn worker threads
	for (int i=0; i<workers; ++i) {
		thread_args[i] = i;
		int result = pthread_create(&threads[i], NULL, worker_func, &thread_args[i]);
		assert(!result);
	}

	if (show_counts)
		printf("Thread\tCount\n");

	//wait for all threads to complete
	long counts_total = 0;
	for (int i=0; i<workers; ++i) {
		int result = pthread_join(threads[i], (void*)&thread_counts[i]);
		assert(!result);
		
		counts_total += thread_counts[i];
		if (show_counts)
			printf("%d\t%d\n", i, thread_counts[i]);
	}

	if (show_lost) {
		printf("lost/maxcounter: %f\n", (double)counts_total / maxcounter);
	}
	if (show_load) {
		printf("thread\tload\n");
		for (int i=0; i<workers; i++)
			printf("%d\t%f\n", i, (double)thread_counts[i] / ((double)maxcounter / workers));
	}

	return 0;
}

void *worker_func(void *args) {
	int id = *((int*)args);

	int my_counter = 0;
	while (shared_counter < maxcounter) {
		++my_counter;
		++shared_counter;
	}

	pthread_exit((void*)my_counter);
}

void parse_opts(int argc, char* const argv[]) {
	static struct option longopts[] = {
		{"maxcounter", required_argument, NULL, 'm'},
		{"workers", required_argument, NULL, 'w'},
		{"show-counts", no_argument, NULL, 'c'},
		{"show-lost", no_argument, NULL, 'l'},
		{"show-load", no_argument, NULL, 'L'},
		{"help", no_argument, NULL, 'h'}
	};

	int longindex = 0;
	char flag = 0;
	while ((flag = getopt_long(argc, argv, "m:w:", longopts, &longindex)) != -1) {
		switch (flag) {
			case 'm':
				maxcounter = atoi(optarg);
				break;
			case 'w':
				workers = atoi(optarg);
				break;
			case 'c':
				show_counts = true;
				break;
			case 'l':
				show_lost = true;
				break;
			case 'L':
				show_load = true;
				break;
			case 'h':
			default:
				printf("Usage: lab1 [--workers=n] [--maxcounter=n] [--show-counts] [--show-load] [--show-lost]\n");
				exit(-1);
		}
	}
}
