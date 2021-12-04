#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

struct cache_line {
	int valid_bit;
	int tag;
	int lru_count;
};

int main(int argc, char** argv) {
	// initialize arguments with 0
	// read command line arguments
	int opt, vflag = 0, svalue = 0, evalue = 0, bvalue = 0;
	char* tracefile = NULL;
	while( (opt = getopt(argc, argv, "hvs:E:b:t:")) != -1 ) {
		switch(opt) {
			case 'h':
				fprintf(stdout, "Usage: %s [-hv] -s <S> -E <E> -b <b> -t <tracefile>\n", argv[0]);
				exit(EXIT_SUCCESS);
			case 'v':
				vflag = 1;
				break;
			case 's':
				svalue = atoi(optarg);
				break;
			case 'E':
				evalue = atoi(optarg);
				break;
			case 'b':
				bvalue = atoi(optarg);
				break;
			case 't':
				tracefile = optarg;
				break;
			default:
				fprintf(stderr, "Wrong argument!\n");
				exit(EXIT_FAILURE);
		}
	}

	// print an error if any of the arguments are wrong or missing
	if ( (svalue <= 0) || (evalue <= 0) || (bvalue <= 0) || (tracefile == NULL)) {
		fprintf(stderr, "One or more arguments wrong or missing!\n");
		exit(EXIT_FAILURE);
	}

	// initialize cache (= 2D array of cache_line structs)
	int S = 2^svalue;
	int E = 2^evalue;
	struct cache_line *p_cache = (struct cache_line *) malloc(sizeof(struct cache_line) * S * E);
	if (p_cache == NULL) {
		fprintf(stderr, "malloc of size %ld failed!\n", sizeof(struct cache_line) * S * E);
		exit(EXIT_FAILURE);
	} else {
		fprintf(stdout, "malloc of size %ld successful\n", sizeof(struct cache_line) * S * E);
	}

	FILE* pFile = fopen(tracefile, "r");
	char identifier;
	unsigned int address;
	int size;
	while(fscanf(pFile, " %c %x,%d", &identifier, &address, &size) > 0) {
		if(identifier == 'I') {
			continue;
		}
		if(vflag) {
			fprintf(stdout, "%c %x,%d\n", identifier, address, size);
		}
	}
	fclose(pFile);

  	printSummary(0, 0, 0);
	free(p_cache);
  	return 0;
}
