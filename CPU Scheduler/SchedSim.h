#ifndef _SCHEDSIM_H_
#define _SCHEDSIM_H_

/*	Header file SchedSim.h
 *
 *	Defines functions to be used by CSCI 460 students
 *
 *	David Bover, WWU Computer Science, July, 2007
 *	updated 8 July 2009
 *	updated July 2010
 */

// Simulate() runs the CPU sceduler simulation
// rounds    : number of iterations that the simulation is to run
// timeslice : each round corresponds to one process in the running state, timeslice in milliseconds
void Simulate(int rounds, int timeslice );

#endif
