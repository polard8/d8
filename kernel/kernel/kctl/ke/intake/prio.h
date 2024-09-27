
// prio.h
// Priorities

#ifndef __PRIO_H
#define __PRIO_H    1

//
// == Priorities ==================================================
//

// 2 classes of priority.

//----------------------------
// class 1: Normal priorities
#define PRIORITY_P1  1
#define PRIORITY_P2  2
#define PRIORITY_P3  3
// class 2: System priorities
#define PRIORITY_P4  4
#define PRIORITY_P5  5
#define PRIORITY_P6  6
//----------------------------

#define PRIORITY_NORMAL_THRESHOLD  PRIORITY_P1
#define PRIORITY_SYSTEM_THRESHOLD  PRIORITY_P4

//----------------------------

#define PRIORITY_NORMAL_LOW      PRIORITY_P1
#define PRIORITY_NORMAL_BALANCE  PRIORITY_P2
#define PRIORITY_NORMAL_HIGH     PRIORITY_P3

#define PRIORITY_SYSTEM_LOW      PRIORITY_P4
#define PRIORITY_SYSTEM_BALANCE  PRIORITY_P5
#define PRIORITY_SYSTEM_HIGH     PRIORITY_P6

//----------------------------

#define PRIORITY_MIN  PRIORITY_P1
#define PRIORITY_MAX  PRIORITY_P6

//----------------------------

// ##
// A prioridade no escalonamento não afetara os créditos.

// ----------------------------------------------------

#endif    




