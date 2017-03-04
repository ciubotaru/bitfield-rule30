/**
 * File name: rule30.h
 * Project name: rule30, an implementation of  Wolfram's Rule 30 written in C
 * URL: https://github.com/ciubotaru/bitfield
 * Author: Vitalie Ciubotaru <vitalie at ciubotaru dot tk>
 * License: General Public License, version 3 or later
 * Copyright 2015, 2016
**/

#ifndef RULE30_H
#define RULE30_H
#include <bitfield.h>

struct parents {
	struct bitfield *parent[4];	// a pointer to an array of 4 struct bitfield
};

void rule30_parents_del(struct parents *instance);

struct parents *rule30_parents_new(const int size);

struct parents *rule30_rev_bit(const int input);	/* returns the four parent triplets of a bit (0 or 1) */

struct parents *rule30_rev_ring(const struct bitfield *input, int *count);	/* treat the input array of bits as a ring and attempt to reverse-transform it by Wolfram's Rule 30 */

struct parents *rule30_rev_string(const struct bitfield *input);	/* reverse-transforms an array of bits by Wolfram's Rule 30, and returns all possible parent combinations */

void rule30_ring_ip(struct bitfield *instance);	/* treats the input array of bits as a ring and transforms it by Wolfram's Rule 30 "in-place" */

struct bitfield *rule30_ring(const struct bitfield *input);	/* treats the input array of bits as a ring and transforms it by Wolfram's Rule 30 */

int rule30_ringify(const struct bitfield *input, struct bitfield *output, char **errmsg);	/* tests if an array of bits can be treated as a ring of given length and returns the resulting ring */

void eca_string_ip(struct bitfield *instance, const unsigned int wolfram_code);	/* transforms an array of bits as an elementary cellular automaton given by Wolfram's code "in-place" */

struct bitfield *eca_string(const struct bitfield *input, const unsigned int wolfram_code);	/* transforms an array of bits as an elementary cellular automaton given by Wolfram's code */

#endif
