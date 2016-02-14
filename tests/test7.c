/**
 * File name: tests/test7.c
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

/* Testing rule30_string_ip() */

int main()
{
	int i;			//counter
	int len = 80;
	char *errmsg;
	char *msg = "Testing rule30_string_ip()";
	char *failed = "[FAIL]";
	char *passed = "[PASS]";
	int dots = len - strlen(msg) - 6;	/* 6 is the length of pass/fail string */
	printf("%s", msg);
	for (i = 0; i < dots; i++)
		printf(".");
	char input_raw[] =
	    "00000000000000000000000000000000000000010000000000000000000000000000000000000000";
	char check_raw[] =
	    "10101010111010011001010000110000100011001000101011000000001100011011101110000101";
	struct bitfield *input = bfnew(strlen(input_raw));
	struct bitfield *check = bfnew(bfsize(input));
	str2bf_ip(check_raw, check);
	struct bitfield *empty = bfnew(1);
	struct bitfield *tmp;
	str2bf_ip("0", empty);
	str2bf_ip(input_raw, input);

//      bfprint(input);

	for (i = 0; i < 50; i++) {
		tmp = bfcat(empty, input);	// 1 + 80 = 81
		input = bfcat(tmp, empty);	// 81 + 1 = 82
		rule30_string_ip(input);
	}
	if (bfcmp(input, check, &errmsg) != 0) {
		printf("%s\n", failed);
		return 1;
	}
	printf("%s\n", passed);
	return 0;
}
