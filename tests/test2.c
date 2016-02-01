/**
 * File name: tests/test2.c
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Date: February 1, 2016
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rule30.h"

/* Testing rule30_rev_bit() */

int main()
{
	srand((unsigned)time(NULL));
	int i;			//counter
    unsigned int input;
	int len = 80;
	char *msg = "Testing rule30_rev_bit()";
	char *failed = "[FAIL]";
	char *passed = "[PASS]";
	int dots = len - strlen(msg) - 6;	/* 6 is the length of pass/fail string */
	printf("%s", msg);
	for (i = 0; i < dots; i++)
		printf(".");

	for (i = 0; i < 50; i++) {
		input = rand() % 2;
		struct parents *triplet = rule30_rev_bit(input);
		int choice = rand() % 4;
		struct bitfield *output = rule30_string(triplet->parent[choice]);
		rule30_parents_del(triplet);
		if (bfgetbit(output, 0) != input) {
			printf("%s\n", failed);
			bfdel(output);
			return 1;
		}
		bfdel(output);
	}
	printf("%s\n", passed);
	return 0;
}
