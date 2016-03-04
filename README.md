# mppa-TiCOS
Porting of TiCOS to 




The goal of this README is to provide quick start guide to use TiCOS. 
The complete TiCOS documentation can be found in the doc directory.

Most macros in the code exhibit the POK suffix: it has been decided to
follow the original naming scheme from POK, even for those macros that
have been introduced for the first time in TiCOS.


To compile an example
=====================

1) Before compiling TiCOS you should:
- add to your path the PowerPC compiler bin directory;
- add to the LD_LIBRARY_PATH  the PowerPC compiler lib directory;
- set the POK_PATH variable to the TiCOS installation directory. 
- set the QEMU_HOME variable to the QEMU installation directory (required only if you want to run TiCOS on QEMU).  

2) cd $POK_PATH/examples/<example-name>/generated-code

3) If you want to run it on QEMU add to the cpu/kernel/deployment.h file the following line:

#define POK_NEEDS_QEMU_SETUP    1

4) make


To run an example on QEMU
=========================

make run


To run an example on PROARTIS Sim
=================================

1) modify the PROARTIS sim param_file setting the –executable option to the path of the .elf file you want to execute 
(i.e. $POK_PATH/examples/<example-name>/generated-code/cpu/pok.elf) 

2) cd <PROARTIS_sim PATH>

3) run the PROARTIS sim:

./PROARTIS_sim param_file


Examples
========

In this directory you can find the following examples:

arinc653-1part
--------------
This is the simplest example: it has just one partition with two threads that simply print a message.

arinc653-2parts
---------------
This example shows  an application having two partitions with two threads each that simply print a message. The scheduler is O(1).

arinc653-2parts-FPPS
--------------------
This example shows  an application having two partitions with two threads each that simply print a message. The scheduler is FPPS.

arinc653-3parts
---------------
This example shows  an application having three partitions with two threads each. The scheduler is O(1). The major frame is made up of four execution slots: the first and the third slots are assigned to the first partition, the second and the forth slot are assigned to the second and third partition, respectively. 

arinc653-1event-O1
------------------
This example shows how to apply the process split mechanism introduced in the document “TiCOS” to an application that makes use of the O(1) scheduler and invokes the ARINC653 EVENT primitives. The process calling the WAIT_EVENT service is split into two sub processes: one periodic (thr1_1_job) and one sporadic (thr1_1bis_job) so as to make the WAIT_EVENT call as the invocation event for a sporadic process. The process calling the SET_EVENT service is also specularly split into two sub processes: one periodic (thr1_3_job) and one sporadic (thr1_3bis_job) so as to enforce a rescheduling after the event is set up.

arinc653-1event-O1-split
------------------------
This example shows how to apply the process split mechanism introduced in the document “TiCOS” to an application that makes use of O(1) scheduler WITH TASK SPLIT and invokes the ARINC653 EVENT primitives. The process calling the WAIT_EVENT service is split into two sub processes: one periodic (thr1_1_pre_wait1_job) and one sporadic (thr1_2_succ_wait1_job) so as to make the WAIT_EVENT call as the invocation event for a sporadic process. The process calling the SET_EVENT is not splitted.

arinc653-blackboards
--------------------
This example shows how to apply the process split mechanism introduced in the document “TiCOS” to an application that makes use of the O(1) scheduler and invokes the ARINC653 BLACKBOARDS primitives. The process calling the READ_BLACKBOARD service is split into two sub processes: one periodic (thr1_1_job) and one sporadic (thr1_1bis_job) so as to make each call as the invocation event for as many sporadic processes. The process calling the DISPLAY_BLACKBOARD service is also specularly split into two sub processes: one periodic (thr1_3_job) and one sporadic (thr1_3bis_job) so as to enforce a rescheduling after a message is displayed in a blackboard.

arinc653-buffers
----------------
This example shows how to apply the process split mechanism introduced in the document “TiCOS” to an application that makes use of the O(1) scheduler and invokes the ARINC653 BUFFERS primitives. The process calling the RECEIVE_BUFFER service is split into two sub processes: one periodic (thr1_1_job) and one sporadic (thr1_1bis_job) so as to make each call as the invocation event for as many sporadic processes. The process calling the SEND_BUFFER service is also specularly split into two sub processes: one periodic (thr1_3_job) and one sporadic (thr1_3bis_job) so as to make each call as the invocation event for as many sporadic processes.

arinc653-sampling-queueing
--------------------------
This example shows an application that uses sampling and queueing ports. 
 

