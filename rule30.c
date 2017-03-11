/**
 * File name: rule30.c
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Copyright 2015, 2016
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rule30.h"

void rule30_parents_del(struct parents *instance)
{
	int i;
	for (i = 0; i < 4; i++)
		bfdel(instance->parent[i]);
	free(instance);
	return;
}

struct parents *rule30_parents_new(const int size)
{
	/* returns a pointer to struct parent */
	int i;
	struct parents *instance = malloc(4 * sizeof(struct parents));
	for (i = 0; i < 4; i++)
		(instance->parent)[i] = bfnew(size);
	return instance;
}

struct parents *rule30_rev_bit(const int input)
{
	struct parents *output = rule30_parents_new(3);
	if (input & 1) {
		str2bf_ip("100", output->parent[0]);
		str2bf_ip("010", output->parent[1]);
		str2bf_ip("011", output->parent[2]);
		str2bf_ip("001", output->parent[3]);
	} else {
		str2bf_ip("000", output->parent[0]);
		str2bf_ip("101", output->parent[1]);
		str2bf_ip("110", output->parent[2]);
		str2bf_ip("111", output->parent[3]);
	}
	return output;
}

struct parents *rule30_rev_ring(const struct bitfield *input, int *count)
{
	int i;
	int result;
	int size = bfsize(input);
	struct parents *potential_parents = rule30_rev_string(input);
	struct parents *parents = rule30_parents_new(size);
	struct bitfield *potential_parent = bfnew(size);
	(*count) = 0;
	for (i = 0; i < 4; i++) {
		result =
		    rule30_ringify(potential_parents->parent[i],
				   potential_parent, NULL);
		if (result == 0) {
			struct bitfield *tmp = bfshift(potential_parent, -1);
			bfcpy(tmp, parents->parent[(int)*count]);
			bfdel(tmp);
			(*count)++;
		}
	}
	rule30_parents_del(potential_parents);
	bfdel(potential_parent);
	return parents;
}

struct parents *rule30_rev_string(const struct bitfield *input)
{
	/**
	 * A string is always Rule30-wise reversible.
	 * There are always 4 strings of length n+2
	 * that generate a particular string length n
	 */

	int size = bfsize(input);
	int i, j, k, l;		//counters
	int t_l;		// counter for matches
	int bit;
	struct parents *output = rule30_parents_new(size + 2);
	struct parents *temp_0;
	struct parents *temp_1;

	//get all proto (3 bit) for the first bit of input string
	temp_0 = rule30_rev_bit(bfgetbit(input, 0));

	if (bfsize(input) == 1) {
		for (i = 0; i < 4; i++)
			bfcpy(temp_0->parent[i], output->parent[i]);
		rule30_parents_del(temp_0);
		return output;
	}
	//initialize temp storage arrays
	struct parents *temp_l = rule30_parents_new(size + 2);
	struct parents *temp_r = rule30_parents_new(size + 2);

	for (i = 0; i < 4; i++) {
		bfcpy(temp_0->parent[i], temp_l->parent[i]);
	}
	rule30_parents_del(temp_0);

	for (i = 1; i < bfsize(input); i++) {
		bit = bfgetbit(input, i);
		temp_1 = rule30_rev_bit(bit);
		t_l = 0;
		for (k = t_l; k < 4; k++) {	//4 substrings in temp_l
			for (j = 0; j < 4; j++) {	//4 substrings in temp_r
				struct bitfield *sub1 = bfsub(temp_l->parent[j], i, i + 2);
				struct bitfield *sub2 = bfsub(temp_1->parent[k], 0, 2);
				int result = bfcmp(sub1, sub2, NULL);
				bfdel(sub1);
				bfdel(sub2);
				if (result == 0) {
					// add to potential ancestors, move values of next_bit to prev_bit and move to next bit
					bfcpy(temp_l->parent[j],
					      temp_r->parent[t_l]);

					if (bfgetbit(temp_1->parent[k], 2))
						bfsetbit(temp_r->parent[t_l],
							 i + 2);
					else
						bfclearbit(temp_r->parent[t_l],
							   i + 2);
					t_l++;	// there can be only 4 matches, so stop comparing after the fourth match
				}
			}
		}
		rule30_parents_del(temp_1);
		for (l = 0; l < 4; l++) {
			bfcpy(temp_r->parent[l], temp_l->parent[l]);
		}
	}
	for (i = 0; i < 4; i++) {
		bfcpy(temp_r->parent[i], output->parent[i]);
	}

	rule30_parents_del(temp_l);
	rule30_parents_del(temp_r);
	return output;
}

void rule30_ring_ip(struct bitfield *instance)
{
	struct bitfield *left = bfshift(instance, 1);
	struct bitfield *right = bfshift(instance, -1);
	struct bitfield *or = bfor(instance, right);
	struct bitfield *xor = bfxor(left, or);
	bfcpy(xor, instance);
	bfdel(left);
	bfdel(right);
	bfdel(or);
	bfdel(xor);
}

struct bitfield *rule30_ring(const struct bitfield *input)
{
	int input_size = bfsize(input);
	struct bitfield *left = bfshift(input, 1);
	struct bitfield *right = bfshift(input, -1);
	struct bitfield *output = bfnew_quick(input_size);
	struct bitfield *or = bfor(input, right);
	struct bitfield *xor = bfxor(left, or);
	bfcpy(xor, output);
	bfdel(left);
	bfdel(right);
	bfdel(or);
	bfdel(xor);
	return output;
}

int rule30_ringify(const struct bitfield *input, struct bitfield *output,
		   char **errmsg)
{
	/* This function extracts 2 sub bitfields that overlap and compares them */

	char *msg;
	int input_size = bfsize(input);
	int output_size = bfsize(output);
	int diff_bits = input_size - output_size;	// expected to be non-negative, but...

	/* if input and output are of the same length, just copy and return */
	if (diff_bits == 0) {
		bfcpy(input, output);
		return 0;
	}

	/* the resulting ring can not be longer than the input string */
	else if (diff_bits < 0) {
		msg = "Input too short";
		goto error;
	}

	/* multiple overlaps are not implemented */
	else if (output_size < diff_bits) {
		msg = "Input too long";
		goto error;
	}

	int end_front = input_size - output_size;	// the last bit of the first subfield, plus one
	int start_back = output_size;	// the first bit of the second subfield
	struct bitfield *front = bfsub(input, 0, end_front);
	struct bitfield *end = bfsub(input, start_back, input_size);

	/* check if the overlapping parts are identical */
	int result = bfcmp(front, end, NULL);
	bfdel(front);
	bfdel(end);
	if (result != 0) {
		msg = "Can't ringify";
		goto error;
	}
	struct bitfield *sub = bfsub(input, 0, output_size);
	bfcpy(sub, output);
	bfdel(sub);
	return 0;
 error:
	if (errmsg) {
		*errmsg = malloc(strlen(msg) + 1);
		if (*errmsg)
			memcpy(*errmsg, msg, strlen(msg) + 1);
	}
	return 1;
}

inline static void eca_0(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	bfclearall(output);
}

inline static void eca_1(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_2(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, center);
	struct bitfield *tmp2 = bfnot(tmp1);
	struct bitfield *tmp3 = bfand(right, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_3(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, center);
	struct bitfield *tmp2 = bfnot(tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_4(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, right);
	struct bitfield *tmp2 = bfnot(tmp1);
	struct bitfield *tmp3 = bfand(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_5(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, right);
	struct bitfield *tmp2 = bfnot(tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_6(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	struct bitfield *tmp2 = bfxor(center, right);
	struct bitfield *tmp3 = bfand(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_7(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(center, right);
	struct bitfield *tmp2 = bfor(left, tmp1);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_8(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	struct bitfield *tmp2 = bfand(tmp1, center);
	struct bitfield *tmp3 = bfand(tmp2, right);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_9(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(center, right);
	struct bitfield *tmp2 = bfor(left, tmp1);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_10(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	struct bitfield *tmp2 = bfand(tmp1, right);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_11(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfor(left, tmp1);
	struct bitfield *tmp3 = bfor(tmp2, right);
	struct bitfield *tmp4 = bfxor(left, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_12(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfxor(tmp1, center);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_13(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfor(center, tmp1);
	struct bitfield *tmp3 = bfor(left, tmp2);
	struct bitfield *tmp4 = bfxor(left, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_14(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_15(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	bfcpy(tmp1, output);
	bfdel(tmp1);
}

inline static void eca_16(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfand(left, tmp1);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_17(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfnot(tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_18(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, right);
	struct bitfield *tmp2 = bfnot(center);
	struct bitfield *tmp3 = bfand(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_19(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, right);
	struct bitfield *tmp2 = bfor(tmp1, center);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_20(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, center);
	struct bitfield *tmp2 = bfnot(right);
	struct bitfield *tmp3 = bfand(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_21(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_22(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfxor(center, right);
	struct bitfield *tmp3 = bfor(tmp1, tmp2);
	struct bitfield *tmp4 = bfxor(left, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_23(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	struct bitfield *tmp3 = bfxor(center, right);
	struct bitfield *tmp4 = bfor(tmp2, tmp3);
	struct bitfield *tmp5 = bfxor(left, tmp4);
	bfcpy(tmp5, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
	bfdel(tmp5);
}

inline static void eca_24(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, center);
	struct bitfield *tmp2 = bfxor(left, right);
	struct bitfield *tmp3 = bfand(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_25(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfxor(center, right);
	struct bitfield *tmp3 = bfor(tmp1, tmp2);
	struct bitfield *tmp4 = bfnot(tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_26(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_27(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	struct bitfield *tmp3 = bfor(tmp2, right);
	struct bitfield *tmp4 = bfxor(left, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_28(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, right);
	struct bitfield *tmp2 = bfor(tmp1, center);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_29(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	struct bitfield *tmp3 = bfor(tmp2, center);
	struct bitfield *tmp4 = bfxor(left, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_30(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_31(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfand(left, tmp1);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_32(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfand(left, tmp1);
	struct bitfield *tmp3 = bfand(tmp2, right);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_33(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, right);
	struct bitfield *tmp2 = bfor(center, tmp1);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_34(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfand(tmp1, right);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_35(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	struct bitfield *tmp2 = bfor(tmp1, center);
	struct bitfield *tmp3 = bfor(tmp2, right);
	struct bitfield *tmp4 = bfxor(tmp3, center);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_36(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, center);
	struct bitfield *tmp2 = bfxor(center, right);
	struct bitfield *tmp3 = bfand(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_37(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(center, right);
	struct bitfield *tmp2 = bfxor(left, right);
	struct bitfield *tmp3 = bfor(tmp1, tmp2);
	struct bitfield *tmp4 = bfnot(tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_38(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfxor(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_39(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	struct bitfield *tmp3 = bfor(tmp2, right);
	struct bitfield *tmp4 = bfxor(tmp3, center);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_40(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, center);
	struct bitfield *tmp2 = bfand(tmp1, right);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_41(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfxor(left, center);
	struct bitfield *tmp3 = bfxor(tmp2, right);
	struct bitfield *tmp4 = bfor(tmp1, tmp3);
	struct bitfield *tmp5 = bfnot(tmp4);
	bfcpy(tmp5, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
	bfdel(tmp5);
}

inline static void eca_42(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfand(tmp1, right);
	struct bitfield *tmp3 = bfxor(tmp2, right);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_43(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfxor(left, right);
	struct bitfield *tmp2 = bfnot(center);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	struct bitfield *tmp4 = bfor(tmp1, tmp3);
	struct bitfield *tmp5 = bfxor(left, tmp4);
	bfcpy(tmp5, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
	bfdel(tmp5);
}

inline static void eca_44(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfand(left, tmp1);
	struct bitfield *tmp3 = bfxor(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_45(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfor(center, tmp1);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_46(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(left, center);
	struct bitfield *tmp2 = bfor(center, right);
	struct bitfield *tmp3 = bfxor(tmp1, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_47(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(left);
	struct bitfield *tmp2 = bfnot(center);
	struct bitfield *tmp3 = bfand(tmp2, right);
	struct bitfield *tmp4 = bfor(tmp1, tmp3);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_48(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	struct bitfield *tmp2 = bfand(left, tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_49(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfor(left, center);
	struct bitfield *tmp3 = bfor(tmp2, tmp1);
	struct bitfield *tmp4 = bfxor(tmp3, center);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_50(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, center);
	struct bitfield *tmp2 = bfor(tmp1, right);
	struct bitfield *tmp3 = bfxor(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_51(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(center);
	bfcpy(tmp1, output);
	bfdel(tmp1);
}

inline static void eca_52(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfand(center, right);
	struct bitfield *tmp2 = bfor(left, tmp1);
	struct bitfield *tmp3 = bfxor(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_53(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfxor(center, tmp1);
	struct bitfield *tmp3 = bfor(left, tmp2);
	struct bitfield *tmp4 = bfxor(tmp3, center);
	bfcpy(tmp4, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
	bfdel(tmp4);
}

inline static void eca_54(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, right);
	struct bitfield *tmp2 = bfxor(tmp1, center);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

inline static void eca_55(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, right);
	struct bitfield *tmp2 = bfand(tmp1, center);
	struct bitfield *tmp3 = bfnot(tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_56(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(left, right);
	struct bitfield *tmp2 = bfand(tmp1, center);
	struct bitfield *tmp3 = bfxor(left, tmp2);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_57(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfnot(right);
	struct bitfield *tmp2 = bfor(left, tmp1);
	struct bitfield *tmp3 = bfxor(tmp2, center);
	bfcpy(tmp3, output);
	bfdel(tmp1);
	bfdel(tmp2);
	bfdel(tmp3);
}

inline static void eca_58(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_59(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_60(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_61(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_62(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_63(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_64(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_65(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_66(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_67(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_68(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_69(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_70(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_71(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_72(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_73(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_74(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_75(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_76(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_77(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_78(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_79(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_80(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_81(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_82(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_83(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_84(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_85(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_86(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_87(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_88(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_89(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_90(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_91(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_92(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_93(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_94(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_95(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_96(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_97(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_98(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_99(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_100(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_101(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_102(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_103(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_104(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_105(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_106(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_107(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_108(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_109(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_110(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_111(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_112(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_113(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_114(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_115(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_116(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_117(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_118(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_119(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_120(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_121(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_122(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_123(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_124(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_125(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_126(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_127(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_128(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_129(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_130(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_131(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_132(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_133(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_134(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_135(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_136(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_137(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_138(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_139(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_140(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_141(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_142(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_143(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_144(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_145(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_146(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_147(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_148(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_149(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_150(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_151(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_152(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_153(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_154(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_155(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_156(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_157(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_158(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_159(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_160(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_161(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_162(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_163(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_164(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_165(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_166(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_167(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_168(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_169(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_170(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_171(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_172(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_173(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_174(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_175(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_176(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_177(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_178(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_179(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_180(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_181(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_182(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_183(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_184(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_185(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_186(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_187(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_188(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_189(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_190(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_191(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_192(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_193(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_194(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_195(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_196(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_197(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_198(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_199(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_200(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_201(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_202(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_203(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_204(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_205(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_206(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_207(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_208(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_209(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_210(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_211(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_212(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_213(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_214(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_215(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_216(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_217(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_218(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_219(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_220(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_221(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_222(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_223(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_224(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_225(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_226(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_227(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_228(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_229(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_230(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_231(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_232(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_233(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_234(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_235(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_236(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_237(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_238(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_239(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_240(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_241(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_242(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_243(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_244(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_245(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_246(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_247(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_248(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_249(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_250(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_251(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_252(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_253(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_254(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

inline static void eca_255(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{

}

static void (*eca[256])() = {
	&eca_0,
	&eca_1,
	&eca_2,
	&eca_3,
	&eca_4,
	&eca_5,
	&eca_6,
	&eca_7,
	&eca_8,
	&eca_9,
	&eca_10,
	&eca_11,
	&eca_12,
	&eca_13,
	&eca_14,
	&eca_15,
	&eca_16,
	&eca_17,
	&eca_18,
	&eca_19,
	&eca_20,
	&eca_21,
	&eca_22,
	&eca_23,
	&eca_24,
	&eca_25,
	&eca_26,
	&eca_27,
	&eca_28,
	&eca_29,
	&eca_30,
	&eca_31,
	&eca_32,
	&eca_33,
	&eca_34,
	&eca_35,
	&eca_36,
	&eca_37,
	&eca_38,
	&eca_39,
	&eca_40,
	&eca_41,
	&eca_42,
	&eca_43,
	&eca_44,
	&eca_45,
	&eca_46,
	&eca_47,
	&eca_48,
	&eca_49,
	&eca_50,
	&eca_51,
	&eca_52,
	&eca_53,
	&eca_54,
	&eca_55,
	&eca_56,
	&eca_57,
	&eca_58,
	&eca_59,
	&eca_60,
	&eca_61,
	&eca_62,
	&eca_63,
	&eca_64,
	&eca_65,
	&eca_66,
	&eca_67,
	&eca_68,
	&eca_69,
	&eca_70,
	&eca_71,
	&eca_72,
	&eca_73,
	&eca_74,
	&eca_75,
	&eca_76,
	&eca_77,
	&eca_78,
	&eca_79,
	&eca_80,
	&eca_81,
	&eca_82,
	&eca_83,
	&eca_84,
	&eca_85,
	&eca_86,
	&eca_87,
	&eca_88,
	&eca_89,
	&eca_90,
	&eca_91,
	&eca_92,
	&eca_93,
	&eca_94,
	&eca_95,
	&eca_96,
	&eca_97,
	&eca_98,
	&eca_99,
	&eca_100,
	&eca_101,
	&eca_102,
	&eca_103,
	&eca_104,
	&eca_105,
	&eca_106,
	&eca_107,
	&eca_108,
	&eca_109,
	&eca_110,
	&eca_111,
	&eca_112,
	&eca_113,
	&eca_114,
	&eca_115,
	&eca_116,
	&eca_117,
	&eca_118,
	&eca_119,
	&eca_120,
	&eca_121,
	&eca_122,
	&eca_123,
	&eca_124,
	&eca_125,
	&eca_126,
	&eca_127,
	&eca_128,
	&eca_129,
	&eca_130,
	&eca_131,
	&eca_132,
	&eca_133,
	&eca_134,
	&eca_135,
	&eca_136,
	&eca_137,
	&eca_138,
	&eca_139,
	&eca_140,
	&eca_141,
	&eca_142,
	&eca_143,
	&eca_144,
	&eca_145,
	&eca_146,
	&eca_147,
	&eca_148,
	&eca_149,
	&eca_150,
	&eca_151,
	&eca_152,
	&eca_153,
	&eca_154,
	&eca_155,
	&eca_156,
	&eca_157,
	&eca_158,
	&eca_159,
	&eca_160,
	&eca_161,
	&eca_162,
	&eca_163,
	&eca_164,
	&eca_165,
	&eca_166,
	&eca_167,
	&eca_168,
	&eca_169,
	&eca_170,
	&eca_171,
	&eca_172,
	&eca_173,
	&eca_174,
	&eca_175,
	&eca_176,
	&eca_177,
	&eca_178,
	&eca_179,
	&eca_180,
	&eca_181,
	&eca_182,
	&eca_183,
	&eca_184,
	&eca_185,
	&eca_186,
	&eca_187,
	&eca_188,
	&eca_189,
	&eca_190,
	&eca_191,
	&eca_192,
	&eca_193,
	&eca_194,
	&eca_195,
	&eca_196,
	&eca_197,
	&eca_198,
	&eca_199,
	&eca_200,
	&eca_201,
	&eca_202,
	&eca_203,
	&eca_204,
	&eca_205,
	&eca_206,
	&eca_207,
	&eca_208,
	&eca_209,
	&eca_210,
	&eca_211,
	&eca_212,
	&eca_213,
	&eca_214,
	&eca_215,
	&eca_216,
	&eca_217,
	&eca_218,
	&eca_219,
	&eca_220,
	&eca_221,
	&eca_222,
	&eca_223,
	&eca_224,
	&eca_225,
	&eca_226,
	&eca_227,
	&eca_228,
	&eca_229,
	&eca_230,
	&eca_231,
	&eca_232,
	&eca_233,
	&eca_234,
	&eca_235,
	&eca_236,
	&eca_237,
	&eca_238,
	&eca_239,
	&eca_240,
	&eca_241,
	&eca_242,
	&eca_243,
	&eca_244,
	&eca_245,
	&eca_246,
	&eca_247,
	&eca_248,
	&eca_249,
	&eca_250,
	&eca_251,
	&eca_252,
	&eca_253,
	&eca_254,
	&eca_255
};

void eca_string_ip(struct bitfield *instance, const unsigned int wolfram_code)
{
	int size = bfsize(instance);
	struct bitfield *left = bfsub(instance, 0, size - 2);
	struct bitfield *center = bfsub(instance, 1, size - 1);
	struct bitfield *right = bfsub(instance, 2, size);
	bfresize(instance, size - 2);
	eca[wolfram_code](left, center, right, instance);
	bfdel(left);
	bfdel(center);
	bfdel(right);
}



struct bitfield *eca_string(const struct bitfield *input, const unsigned int wolfram_code)
{
	int input_size = bfsize(input);
	struct bitfield *left = bfsub(input, 0, input_size - 2);
	struct bitfield *center = bfsub(input, 1, input_size - 1);
	struct bitfield *right = bfsub(input, 2, input_size);
	struct bitfield *output = bfnew_quick(input_size - 2);
	eca[wolfram_code](left, center, right, output);
	return output;
}
