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

inline static void eca_30(const struct bitfield *left, const struct bitfield *center, const struct bitfield *right, struct bitfield *output)
{
	struct bitfield *tmp1 = bfor(center, right);
	struct bitfield *tmp2 = bfxor(left, tmp1);
	bfcpy(tmp2, output);
	bfdel(tmp1);
	bfdel(tmp2);
}

void eca_string_ip(struct bitfield *instance, const unsigned int wolfram_code)
{
	int size = bfsize(instance);
	struct bitfield *left = bfsub(instance, 0, size - 2);
	struct bitfield *center = bfsub(instance, 1, size - 1);
	struct bitfield *right = bfsub(instance, 2, size);
	bfresize(instance, size - 2);
	switch(wolfram_code) {
		case 1:
			eca_1(left, center, right, instance);
			break;
		case 2:
			eca_2(left, center, right, instance);
			break;
		case 3:
			eca_3(left, center, right, instance);
			break;
		case 4:
			eca_4(left, center, right, instance);
			break;
		case 5:
			eca_5(left, center, right, instance);
			break;
		case 6:
			eca_6(left, center, right, instance);
			break;
		case 7:
			eca_7(left, center, right, instance);
			break;
		case 8:
			eca_8(left, center, right, instance);
			break;
		case 9:
			eca_9(left, center, right, instance);
			break;
		case 10:
			eca_10(left, center, right, instance);
			break;
		case 11:
			eca_11(left, center, right, instance);
			break;
		case 12:
			eca_12(left, center, right, instance);
			break;
		case 13:
			eca_13(left, center, right, instance);
			break;
		case 14:
			eca_14(left, center, right, instance);
			break;
		case 30:
			eca_30(left, center, right, instance);
			break;
		default:
			break;
	}
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
	switch(wolfram_code) {
		case 1:
			eca_1(left, center, right, output);
			break;
		case 2:
			eca_2(left, center, right, output);
			break;
		case 3:
			eca_3(left, center, right, output);
			break;
		case 4:
			eca_4(left, center, right, output);
			break;
		case 5:
			eca_5(left, center, right, output);
			break;
		case 6:
			eca_6(left, center, right, output);
			break;
		case 7:
			eca_7(left, center, right, output);
			break;
		case 8:
			eca_8(left, center, right, output);
			break;
		case 9:
			eca_9(left, center, right, output);
			break;
		case 10:
			eca_10(left, center, right, output);
			break;
		case 11:
			eca_11(left, center, right, output);
			break;
		case 12:
			eca_12(left, center, right, output);
			break;
		case 13:
			eca_13(left, center, right, output);
			break;
		case 14:
			eca_14(left, center, right, output);
			break;
		case 30:
			eca_30(left, center, right, output);
			break;
		default:
			break;
	}
	return output;
}
