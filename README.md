(C) 2016 University of Bristol. See LICENSE.txt

# APRICOT OT Extension

Implementation of the [actively secure OT extension protocol](http://eprint.iacr.org/2015/546) by Keller, Orsini and Scholl.

### Requirements:

 * GCC and CPU supporting PCLMUL, AES-NI, and SSE4.1.

### Instructions:
* If the SimpleOT directory is empty, run:
```
git submodule init
git submodule update
```
 - Run `make ot.x`
 - Run `ot.x -p 0` and then `ot.x -p 1` to run the OT extension locally between two parties
 - For a full list of possible options, run `ot.x`
 - We have measured 69 seconds on a local link with `-n 1000000000 -l 100 -x 50`

### Further comments:

 * `OTExtensionWithMatrix` (used by default) is the optimized extension
   for 128-bit OT. `OTExtension` is slower but more flexible.
 * Base OTs use the SimpleOT library by Orlandi and Tung.
 * `ot-check.x` allows to check the output of ot.x.
 * `ot-bitmatrix.x` is a unit test for the BitMatrix class.
 * This has only been tested on Linux.
 * Parts of the code use a C implementation of AES, which has been
   removed.
