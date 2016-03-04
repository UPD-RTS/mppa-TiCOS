# mppa-TiCOS
Porting of TiCOS to the MPPA2®-256 (Bostan), the second generation of
Kalray's manycore processor family. The Bostan runs 288 programmable
cores, each of them being a 64-bit Very Long Instruction Word (VLIW).
For more info about Kalray and its products please refer to http://www.kalrayinc.com/.

The goal of this README is to provide quick start guide to use TiCOS.
The complete TiCOS documentation can be found in the `doc` directory.

Most macros in the code exhibit the POK suffix: it has been decided to
follow the original naming scheme from POK, even for those macros that
have been introduced for the first time in TiCOS.
