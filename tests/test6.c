/**
 * File name: tests/test6.c
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Date: February 15, 2016
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rule30.h"

/* Testing rule30_rev_ring */

int main()
{
	srand((unsigned)time(NULL));
	int i, j;		//counters
	int len = 80;
	char *msg = "Testing rule30_rev_ring()";
	char *failed = "[FAIL]";
	char *passed = "[PASS]";
	int dots = len - strlen(msg) - 6;	/* 6 is the length of pass/fail string */
	printf("%s", msg);
	for (i = 0; i < dots; i++)
		printf(".");

	char *input_char = malloc((len + 1) * sizeof(char));
	input_char[len] = '\0';
	struct bitfield *input = bfnew_quick(len);
	char *start_char = malloc((len + 1) * sizeof(char));
	char *end_char = malloc((len + 1) * sizeof(char));

	for (i = 0; i < 50; i++) {
		for (j = 0; j < len; j++) {
			if (rand() % 2)
				input_char[j] = '1';
			else
				input_char[j] = '0';
		}
		str2bf_ip(input_char, input);
		int count;
		struct parents *output = rule30_rev_ring(input, &count);
		if (count != 0) {
			for (j = 0; j < count; j++) {
				struct bitfield *ring = rule30_ring(output->parent[j]);
				int result = bfcmp(ring, input, NULL);
				bfdel(ring);
				if (result != 0) {
					printf("%s\n", failed);
					free(input_char);
					free(start_char);
					free(end_char);
					bfdel(input);
					return 1;
				}
			}
			rule30_parents_del(output);
		}
	}
	free(input_char);
	free(start_char);
	free(end_char);
	bfdel(input);
	printf("%s\n", passed);
	return 0;
}
