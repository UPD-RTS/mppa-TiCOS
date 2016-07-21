mppa-TiCOS
====

Porting of TiCOS, an ARINC653 real-time operating system, to the MPPA2®-256
(Bostan), the second generation of Kalray's manycore processor family.

The Bostan runs 288 programmable
cores, each of them being a 64-bit Very Long Instruction Word (VLIW).
For more info about Kalray and its products please refer to http://www.kalrayinc.com/.

The goal of this README is to provide quick start guide to use TiCOS.
The complete TiCOS documentation can be found in the `doc` directory.

Examples
--------

The `examples` directory contains several sample applications able to run on
the mppa (simulation and jtag):

* `arinc653-1event-O1`: consists of a single partition made of 2 periodic and 2 sporadic threads synchronizing on an ARINC event
* `arinc653-1event-O1-split`: consists of a single partition made of 2 periodic
and 1 sporadic thread synchronizing on an ARINC event using 01-split scheduler
* `arinc653-1part`: consists of a single partition made of 2 periodic threads
* `arinc653-2parts`: consists of a 2 partitions each made of 2 periodic threads
* `arinc653-3parts`: consists of a 3 partitions of 1 periodic thread
* `arinc653-4parts`: consists of a 4 partitions of 1 periodic thread
* `arinc653-4parts-2threads`: consists of a 4 partitions each made of 2 periodic threads
* `arinc653-sampling-queueing` : 2 partitions each made of 4 periodic threads,
1 sampling port and 3 queuing ports.


Building
--------

All the available examples can be built using the Makefile from the project's
root directory:

```shell
make
```

In order to clean everything from the compilation process, once again from the
project's root directory:

```shell
make distclean
```

To compile an individual example it is necessary to go to `examples/{EXAMPLE}/generated-code/cpu` and run `make` (where `{EXAMPLE}` refers
to one of the aforementioned examples).


 Running
-------

After building all the examples a folder named `build-all` will be created
containing the executables ready to run. In order to deploy one of them, inside
`build-all/{EXAMPLE}`:

```shell
make simu
```

or

```shell
make hard
```

to run in simulation or jtag, respectively.
