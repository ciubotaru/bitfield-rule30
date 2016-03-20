rule30
======

Version 0.4.0 (March 20, 2016)

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

Functions
---------
For function syntax, see "rule30.h". For details on every function, see
its manual page.

rule30_parents_del() deletes a parents structure.

rule30_parents_new() creates an empty parents structure.

rule30_rev_bit() returns the four parent triplets of a bit (0 or 1)

rule30_rev_ring() treats the input array of bits as a ring and attempts
to reverse-transform it by Wolfram's Rule 30.

rule30_rev_string() reverse-transforms an array of bits by Wolfram's
Rule 30, and returns all possible parent combinations.

rule30_ring_ip() treats the input array of bits as a ring and transforms
it by Wolfram's Rule 30 "in-place".

rule30_ring() treats the input array of bits as a ring and transforms it
by Wolfram's Rule 30.

rule30_ringify() tests if an array of bits can be treated as a ring of
given length and returns the resulting ring.

rule30_string_ip() transforms an array of bits by Wolfram's Rule 30
"in-place".

rule30_string() transforms an array of bits by Wolfram's Rule 30.

Please, see "examples" directory for working examples.

Licensing
---------

rule30 is free software, and is released under the terms of the GNU General 
Public License version 3 or any later version. Please see the file called 
LICENSE.
