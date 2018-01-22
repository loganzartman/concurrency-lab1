#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static int maxcounter = 100;
static int workers = 1;

void parse_opts(int argc, char* const argv[]);

int main(int argc, char* const argv[]) {
	parse_opts(argc, argv);

	printf("Max counter: %d\nWorkers: %d\n", maxcounter, workers);
	return 0;
}

void parse_opts(int argc, char* const argv[]) {
	static struct option longopts[] = {
		{"maxcounter", required_argument, NULL, 'm'},
		{"workers", required_argument, NULL, 'w'}
	};

	int longindex = 0;
	char flag = 0;
	while ((flag = getopt_long_only(argc, argv, "", longopts, &longindex)) != -1) {
		switch (flag) {
			case 'm':
				maxcounter = atoi(optarg);
				break;
			case 'w':
				workers = atoi(optarg);
				break;
		}
	}
}
