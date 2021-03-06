/**
 * File name: tests/test8.c
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Date: February 15, 2016
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rule30.h"

/* Testing rule30_ring_ip() */

int main()
{
	int i;			//counter
	int len = 80;
	char *errmsg;
	char *msg = "Testing rule30_ring_ip()";
	char *failed = "[FAIL]";
	char *passed = "[PASS]";
	int dots = len - strlen(msg) - 6;	/* 6 is the length of pass/fail string */
	printf("%s", msg);
	for (i = 0; i < dots; i++)
		printf(".");

	char *input_raw =
	    "00000000000000000000000000000000000000010000000000000000000000000000000000000000";
	char *check_raw =
	    "11011001011010011001010000110000100011001000101011000000001100011011101110010010";
	struct bitfield *input = bfnew_quick(len);
	struct bitfield *check = bfnew_quick(len);
	str2bf_ip(input_raw, input);
	str2bf_ip(check_raw, check);

	for (i = 0; i < 50; i++) {
		rule30_ring_ip(input);
	}
	if (bfcmp(input, check, &errmsg) != 0) {
		printf("%s\n", failed);
		return 1;
	}
	bfdel(input);
	bfdel(check);
	printf("%s\n", passed);
	return 0;
}
