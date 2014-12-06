/*
  	This file stores implementation of random generator functions
  	Author: Matus Turic, xturic01.fit.vutbr.cz
  	Name: functions.h
  	Subject: IMS 
*/

#ifndef FUNCTIONS
#define FUNCTIONS

// Function for random generator initiaization
// We don't want same numbers when we run program again
void init_rand_gen();

// Function R(0,1) - generates number between 0 and 0.99999
// RAND_MAX is equal to INT_MAX
double random_gen();

// Function R(L,H) - generates number between L and H 
double uniform_gen(double L, double H);

// Exponential distribution function with center E
double exponential_gen(double E);

#endif
