#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#define HIT " hit"
#define MISS " miss"
#define EVICT " eviction"

int num_of_hits = 0, num_of_misses = 0, num_of_evicts = 0;
int hitflag = 0, missflag = 0, evictflag = 0;
int timecounter = 0;

struct cache_line {
	int valid_bit;
	int tag;
	int lru_count;
};

// computes binary number of given unsigned int and returns it
// in a int array of 64 bits.
// @param num unsigned int to convert to binary
// @return binaryArray int-pointer to the "64bit" array holding the binary
// 					   number
int * binaryNumber(unsigned int num) {
    int *binaryArray = (int*) calloc(64, sizeof(int));
    for(int i = 63; i >= 0; i--) {
        binaryArray[i] = num%2;
        num = num/2;
    }
    return binaryArray;
}

int main(int argc, char** argv) {
	// initialize arguments with 0
	// read command line arguments
	int opt, vflag = 0, s = 0, e = 0, b = 0;
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
				s = atoi(optarg);
				break;
			case 'E':
				e = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
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
	if ( (s <= 0) || (e <= 0) || (b <= 0) || (tracefile == NULL)) {
		fprintf(stderr, "One or more arguments wrong or missing!\n");
		exit(EXIT_FAILURE);
	}

	// initialize cache (= 2D array of cache_line structs)
	int nr_of_sets = 1 << s;

	struct cache_line *cache = (struct cache_line *) calloc(3 * nr_of_sets * e, sizeof(int));
	if (cache == NULL) {
		fprintf(stderr, "calloc of size %ld failed!\n", sizeof(struct cache_line) * nr_of_sets * e);
		exit(EXIT_FAILURE);
	}

	FILE* pFile = fopen(tracefile, "r");
	char identifier;
	unsigned int address;
	int size;
	while(fscanf(pFile, " %c %x,%d", &identifier, &address, &size) > 0) {
		if(identifier == 'I') {
			continue;
		}
		int times = 1;
		if(identifier == 'M') {
			times = 2;
		}
		unsigned int rshift_by_Bbits = address >> b;
		unsigned int lshift_by_Bbits = rshift_by_Bbits << b;
		unsigned int rshift_by_Sbits = lshift_by_Bbits >> (s+b);
		unsigned int lshift_by_Sbits = rshift_by_Sbits << (s+b);
		unsigned int set = (lshift_by_Bbits - lshift_by_Sbits) >> b ;
		unsigned int tag = rshift_by_Sbits;

		while(times > 0) {
			for(int i = 0 ; i < e; i++) {								// retrieve the set with the corresponding cache_lines
				if(cache[(set*e)+i].valid_bit) {						// check for each cache_line if valid
					if(cache[(set*e)+i].tag == tag) {					// if cache_line valid, check if tag fits
						hitflag = 1;									// if tag fits, set hitflag to 1
						num_of_hits++;
						timecounter++;
						cache[(set*e)+i].lru_count = timecounter;		// and increase lru_count
					}
				}
			}
			if(!hitflag) {												// if not found in memory
				for(int i = 0 ; i < e; i++) {
					if(missflag) break;
					if(!cache[(set*e)+i].valid_bit) {					// check for first invalid cache_line
						cache[(set*e)+i].valid_bit = 1;					// and set it to valid
						cache[(set*e)+i].tag = tag;
						timecounter++;
						cache[(set*e)+i].lru_count = timecounter;
						missflag = 1;									// it was a miss
						num_of_misses++;
					}
				}
				if(!missflag) {											// if no unused cache_lines
					int minLRUcount = cache[set*e].lru_count;			// set the minimum lru count to first cache_line
					int index = set*e;									// save its index
					for(int i = 0 ; i < e; i++) {
						if(cache[(set*e)+i].lru_count < minLRUcount) {	// check if another line in same set has a lower count
							minLRUcount = cache[(set*e)+i].lru_count;
							index = (set*e)+i;
						}
					}
					cache[index].tag = tag;								// replace cache_line with lowest lru_count
					timecounter++;
					cache[index].lru_count = timecounter;
					missflag = 1;
					evictflag = 1;										// it was a miss and eviction
					num_of_misses++;
					num_of_evicts++;
				}
			}
			times--;
		}

		if(vflag) {
			fprintf(stdout, "%c %x,%d", identifier, address, size);
			if(missflag) printf(MISS);
			if(evictflag) printf(EVICT);
			if(hitflag) printf(HIT);
			printf("\n");
		}
		hitflag = 0;
		missflag = 0;
		evictflag = 0;
	}
	fclose(pFile);

  	printSummary(num_of_hits, num_of_misses, num_of_evicts);

	free(cache);
  	return 0;
}
