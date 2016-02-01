/**
 * File name: tests/test5.c
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

/* Testing rule30_ringify() */

int main()
{
	srand((unsigned)time(NULL));
	int i, j, k;		//counters
	int len = 80;
	char *errmsg;
	int result_char;
	int result;
	char *msg = "Testing rule30_ringify()";
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
//              printf("%s\n", input_char);
		str2bf_ip(input_char, input);
//              bfprint(input);
		for (j = 1; j <= 40; j++) {	// no less than 1 and no more than half of the original string
			strncpy(start_char, input_char, j);
			strncpy(end_char, input_char + (len - j), j);
			for (k = 0; k < j; k++) {
				if (start_char[k] != end_char[k]) {
					result_char = 1;	// i.e. not equal
					k = j;	// stop comparing
				} else
					result_char = 0;	// assume equal and keep comparing
			}
//                      printf("Char comparison result: %i\n", result_char);
			struct bitfield *output = bfnew_quick(len - j);
			result = rule30_ringify(input, output, &errmsg);
			bfdel(output);
//                      printf("rule30_ringify result: %i\n", result);
			if (result_char != result) {
				printf("%s\n", failed);
				free(input_char);
				free(start_char);
				free(end_char);
				bfdel(input);
				return 1;
			}
		}

	}
	free(input_char);
	free(start_char);
	free(end_char);
	bfdel(input);
	printf("%s\n", passed);
	return 0;
}
