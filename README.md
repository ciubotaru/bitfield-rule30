rule30
======

Version 0.0.1 (February 1, 2015)

rule30 is a collection of functions that implement Wolfram's 
[Rule 30](http://en.wikipedia.org/wiki/Rule_30) cellular automaton and related 
transformations.

The Algorithm
-------------

Rule 30 is a one-dimensional two-state cellular automaton rule. The automaton 
consists of a sequence of symbols (sometimes thought of as a row of cells). Two 
states mean that the symbol can take one of two values (0 or 1), or, in other 
words, the corrssponding cell can be either "dead" or "alive". The state of the 
sequence is computed from the previous state. The state of a cell depends on 
the previous state of the cell itself, as well as the two neighbouring cells.

The rule is:

s'<sub>i</sub> = s<sub>i-1</sub> XOR (s<sub>i</sub> OR s<sub>i+1</sub>)

All possible inputs and outcomes are summarized below:

|current state|000|001|010|011|100|101|110|111|
|:-----------:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|new state    | 0 | 1 | 1 | 1 | 1 | 0 | 0 | 0 |

An important convention to be made here is that bits are numbered and showed
from left to right. In binary representation, triplets 000 through 111 
correspond to numbers 0 through 7.

Installation
------------

To compile bitfield from source code:

    $ autoconf
    $ ./configure --prefix=DIRECTORY
    $ make

The only compile-time and run-time dependency is Vitalie Ciubotaru's 'bitfield' 
library (version 0.3.*). If bitfield is not installed, or if the sources were 
configured with the '--with-local-bitfield' flag, make will assume that the 
source code of bitfield is available in the source tree (in 'bitfield' 
directory) and will attempt to build it first.

`make` builds a shared and a static library. For a shared or a static library only, do `make shared` or `make static`.
`make check` runs some tests agains the newly-built library.
`make examples` runs some examples with graphical output.

API of librule30
----------------
librule30 provides:

## rule30_string
`rule30_string` takes an array of bits, each bit representing the state of a cell, transforms it according to Wolfram's Rule 30 and returns the resulting array of bits, two bits shorter.

E.g. the following code

    /* create and fill the parent string, 5 bits long */
    struct bitfield *input = str2bf("00100");
    
    /* transform and get the child string, 3 bits long */
    struct bitfield *output = rule30_string(input);
    
    /* print the bits as zeroes and ones */
    bfprint(output);

should return "111".

## rule30_string_ip
`rule30_string_ip` takes an array of bits, each bit representing the state of a cell and transforms it according to Wolfram's Rule 30 "in-place".

## rule30_ring
`rule30_ring` takes an array of bits, each bit representing the state of a cell, treats it as a ring or loop, transforms it according to Wolfram's Rule 30 and returns the resulting array of bits of the same length.

E.g. the following code

    /* create and fill the parent string, 5 bits long */
    struct bitfield *input = str2bf("00100");
    
    /* transform  and get the child string, same length */
    struct bitfield *output = rule30_ring(input);
    
    /* print the bits as zeroes and ones */
    bfprint(output);

should return "01110".

## rule30_ring_ip
`rule30_ring_ip` takes an array of bits, each bit representing the state of a cell, treats it as a ring or loop, and transforms it "in-place" according to Wolfram's Rule 30.

## rule30_rev_bit
`rule30_rev_bit` takes one integer number and returns an array of 4 triplets corresponding to 4 possible "parents".

E.g. the following code

    /* set one bit in an integer */
    int input = 1;
    
    /* transform */
    struct parents *output = rule30_rev_bit(input);
    
    /* print one of the four possible parents */
    bfprint((output->parent[0]));

should return "100".

## rule30_rev_string
`rule30_rev_string` takes an array of bits, reverse-transforms it according to Wolfram's Rule 30, and returns 4 arrays of bits corresponding to 4 possible "parents".

E.g. the following code

    /* create and fill the child string, 5 bits long */
    struct bitfield *input = str2bf("00100");
    
    /* transform */
    struct parents *output = rule30_rev_string(input);
    
    /* print one of the four possible parents */
    bfprint((output->parent[0]));

should return "1110000".

## rule30_ringify
`rule30_ringify` takes an array of bits, checks if it can be treated as a ring of given length. On success it creates the resulting ring and returns code 0. On failure it creates an error message and returns code 1.

E.g. the following code

    /* create and fill a bit array, 5 bits long */
    struct bitfield *input = str2bf("00100");
    /* create an empty array, same length */
    struct bitfield *output = bfnew_quick(4);
    /* create a string for error message */
    char *errmsg;
    /* try to ringtify */
    int result = rule30_ringify(input, output, &errmsg);
    /* print the ring or the error message*/
    if (result == 0) bfprint(output);
    else printf("%s\n", errmsg);

should return "0010".

Licensing
---------

rule30 is free software, and is released under the terms of the GNU General 
Public License version 3 or any later version. Please see the file called 
LICENSE.
