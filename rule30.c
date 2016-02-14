/**
 * File name: rule30.c
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Date: February 1, 2016
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rule30.h"

struct bitfield *rule30_string(const struct bitfield *input)
{
	int input_size = bfsize(input);
	struct bitfield *left = bfsub(input, 0, input_size - 2);
	struct bitfield *center = bfsub(input, 1, input_size - 1);
	struct bitfield *right = bfsub(input, 2, input_size);
	struct bitfield *output = bfnew_quick(input_size - 2);
	/* compute the child generation by Rule 30:
	 * Child(i) = Parent(i-1) XOR ( Parent(i) OR Parent(i+1) )
	 */
	bfcpy(bfxor(left, bfor(center, right)), output);
	bfdel(left);
	bfdel(center);
	bfdel(right);
	return output;
}

void rule30_string_ip(struct bitfield *instance)
{
	int size = bfsize(instance);
	struct bitfield *left = bfsub(instance, 0, size - 2);
	struct bitfield *center = bfsub(instance, 1, size - 1);
	struct bitfield *right = bfsub(instance, 2, size);
	bfresize(instance, size - 2);
	/* compute the child generation by Rule 30:
	 * Child(i) = Parent(i-1) XOR ( Parent(i) OR Parent(i+1) )
	 */
	bfcpy(bfxor(left, bfor(center, right)), instance);
	bfdel(left);
	bfdel(center);
	bfdel(right);
}

struct bitfield *rule30_ring(const struct bitfield *input)
{
	int input_size = bfsize(input);
	struct bitfield *left = bfshift(input, 1);
	struct bitfield *right = bfshift(input, -1);
	struct bitfield *output = bfnew_quick(input_size);
	bfcpy(bfxor(left, bfor(input, right)), output);
	bfdel(left);
	bfdel(right);
	return output;
}

void rule30_ring_ip(struct bitfield *instance)
{
	struct bitfield *left = bfshift(instance, 1);
	struct bitfield *right = bfshift(instance, -1);
	bfcpy(bfxor(left, bfor(instance, right)), instance);
	bfdel(left);
	bfdel(right);
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
	char *errmsg;
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

				if (bfcmp
				    (bfsub(temp_l->parent[j], i, i + 2),
				     bfsub(temp_1->parent[k], 0, 2),
				     &errmsg) == 0) {
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
		result = rule30_ringify(potential_parents->parent[i], potential_parent, NULL);
		if (result == 0) {
			bfcpy(bfshift(potential_parent, -1), parents->parent[(int) *count]);
			(*count)++;
		}
	}
	return parents;
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

	char *errmsg2;
	/* check if the overlapping parts are identical */
	if (bfcmp(front, end, &errmsg2) != 0) {
		msg = "Can't ringify";
		goto error;
	}
	bfcpy(bfsub(input, 0, output_size), output);
	return 0;
error:
	if (errmsg) {
		*errmsg = malloc(strlen(msg) + 1);
		if (*errmsg) memcpy(*errmsg, msg, strlen(msg) + 1);
	}
	return 1;
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

void rule30_parents_del(struct parents *instance)
{
	int i;
	for (i = 0; i < 4; i++)
		bfdel(instance->parent[i]);
	free(instance);
	return;
}
