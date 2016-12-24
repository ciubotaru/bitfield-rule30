/**
 * File name: tests/test4.c
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

/* Testing rule30_rev_string() */

int main()
{
	srand((unsigned)time(NULL));
	int i, j;		//counters
	int len = 80;
	char *msg = "Testing rule30_rev_string()";
	char *failed = "[FAIL]";
	char *passed = "[PASS]";
	int dots = len - strlen(msg) - 6;	/* 6 is the length of pass/fail string */
	printf("%s", msg);
	for (i = 0; i < dots; i++)
		printf(".");

	struct bitfield *input = bfnew_quick(len);

	for (i = 0; i < 50; i++) {
		for (j = 0; j < len; j++) {
			if (rand() % 2)
				bfsetbit(input, j);
			else
				bfclearbit(input, j);
		}
		struct parents *output = rule30_rev_string(input);
		struct bitfield *check = rule30_string(output->parent[rand() % 4]);
		int result = bfcmp(input, check, NULL);
		bfdel(check);
		rule30_parents_del(output);
		if (result != 0) {
			printf("%s\n", failed);
			return 1;
		}
		
	}
	bfdel(input);
	printf("%s\n", passed);
	return 0;
}
