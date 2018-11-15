/* pic_SwUart_Baud.c
Only for the "p*_SwUart-4.mcl" and the "p*_Ps2sim.mcl" libraries !

This file does not generate real codes in ROM nor in RAM. */

  // SUart_BAUDRATE =  9600, Clock = 8 MHz
#define SUart_CONST  169 // from the CONST TABLE, see below

  // KbSim_BAUDRATE =  9600, Clock = 8 MHz
#define KbSim_CONST  169

/* Possible regular (clock-depending) min-max Baudrate values:

1 MHz clock            600 -   9600 Bd
2 MHz                 1200 -  19200 Bd
4-6 MHz               2400 -  57600 Bd   (max. limit for Ps2sim)
8-10-12 MHz           4800 - 115200 Bd   (max. usual Baud value)
16-20 MHz             9600 - 115200 Bd
32-40 MHz            19200 - 115200 Bd

Note there is an 57600 Bd max limit for Ps2sim (Proteus VT feature) and also a 
clock dependent min. limit due to the simple one_byte delay counter (max. 255)

***** CONSTANT TABLE:

Baud\Clk:  1m     2m     4m     6m     8m    10m    12m    16m    20m    32m    40m

   600    134      *      *      *      *      *      *      *      *      *      *
  1200     65    134      *      *      *      *      *      *      *      *      *
  2400     30     65    134    204      *      *      *      *      *      *      *
  4800     13     30     65     99    134    169    204      *      *      *      *
  9600      4     13     30     47     65     82     99    134    169      *      *
 19200      *      4     13     21     30     39     47     65     82    134    169
 38400      *      *      4      8     13     17     21     30     39     65     82
 57600      *      *      1      4      7     10     13     18     24     42     53
115200      *      *      *      *      1      3      4      7     10     18     24

------------

   Please DO NOT modify the following lines  !! */

//'export' the baudrate depending BIT_DLY constants, using in my libraries:
const char SUart_BIT_DLY = SUart_CONST;
const char KbSim_BIT_DLY = KbSim_CONST;
//NOT equal to the bit_time, assume also the loop time

// 'export' the calculated start_delay constants (equal to BIT_DLY/4 but min. 1):
#define SUart_Start SUart_CONST/4
#if SUart_Start < 1
 #undef  SUart_Start
 #define SUart_Start 1
#endif
const char SUart_STA_DLY = SUart_Start;

#define KbSim_Start KbSim_CONST/4
#if KbSim_Start < 1
 #undef  KbSim_Start
 #define KbSim_Start 1
#endif
const char KbSim_STA_DLY = KbSim_Start;

/*   Description:

Determines the BAUDRATE for the "p*_SwUart-4.mcl" and the "p*_Ps2sim.mcl" libraries.

  Usage:

-  choose the proper constant value(s) from the CONSTANT TABLE
   (and modify the #define ...._CONST lines above, on top)

-  add this file to your project by right-clicking the 'Sources' node (see help)
    and
-  add the proper *.mcl file to the project (eg p18_SwUart-4.mcl for a PIC18)
        by right-clicking the 'Binaries' node

--------- OR ----------

- use the "p*_SwUart-8m9600-4.mcl" or the "p*_Ps2sim-8m9600.mcl" library file
    to work with a predetermined baudrate:
    fix 9600 Bd at 8MHz (4800 Bd at 4M, 19200 at 16M, 38400 at 32MHz) clock
- and do not add this .c file to the project !!
 
------ Error calculations:

The percentage difference from the nominal BAUDRATE value everywhere less than 
the normally permitted 3%, except the "115200 Bd at 10MHz" pairing (-5.98%):

Baud\Clk:  1m     2m     4m     6m     8m    10m    12m    16m    20m    32m    40m

   600    0.16     *      *      *      *      *      *      *      *      *      *
  1200   -0.32   0.16     *      *      *      *      *      *      *      *      *
  2400    0.16  -0.32   0.16  -0.16     *      *      *      *      *      *      *
  4800   -1.76   0.16  -0.32   0.48   0.16  -0.03  -0.16     *      *      *      *
  9600    0.16  -1.76   0.16   0.81  -0.32   0.16   0.48   0.16  -0.03     *      *
 19200      *    0.16  -1.76   1.46   0.16  -0.61   0.81  -0.32   0.16   0.16  -0.03
 38400      *      *    0.16   2.80  -1.76   0.16   1.46   0.16  -0.61  -0.32   0.16
 57600      *      *    2.12   0.16  -0.80  -1.38  -1.76   2.12   0.94  -0.80   0.35
115200      *      *      *      *    2.12  -5.98   0.16  -0.80  -1.38   2.12   0.94

   The values of the constant table came from this formula:
(in my new library routines Loop_Cycles = 17 and Delay_Loop = 3)

   BIT_DLY = ((250 * __FOSC__/BAUDRATE) - Loop_Cycles)/Delay_Loop + 1

The mikroC preprocessor's unsigned 32-bit arithmetic does not gave me always correct
result, so I decided that the constant table is pre-generated by an external program,
though I know a bit cumbersome to use. */