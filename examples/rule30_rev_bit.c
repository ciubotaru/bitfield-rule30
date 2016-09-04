#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rule30.h"

int main()
{
	int i;
	srand((unsigned)time(NULL));
	for (i = 0; i < 50; i++) {
		unsigned int input = rand() % 2;
		struct parents *triplet = rule30_rev_bit(input);
		int choice = rand() % 4;
		struct bitfield *output =
		    rule30_string(triplet->parent[choice]);
		printf("%i -> ", input);
		bfprint_lsb(triplet->parent[choice]);
		printf(" -> ");
		bfprint_lsb(output);
		printf("\n");
		rule30_parents_del(triplet);
		bfdel(output);
	}
	return 0;
}
