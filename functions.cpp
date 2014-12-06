/*
  	This file stores implementation of random generator functions
  	Author: Matus Turic, xturic01.fit.vutbr.cz
  	Name: functions.cpp
  	Subject: IMS 
*/

#include "functions.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// Function for random generator initiaization
// We don't want same numbers when we run program again
void init_rand_gen()
{
	srand(time(NULL));
}

// Function R(0,1) - generates number between 0 and 0.99999
// RAND_MAX is equal to INT_MAX
double random_gen()
{
	double r = (rand() / (double)RAND_MAX);
	return r;
}

// Function R(L,H) - generates number between L and H
// 
double uniform_gen(double L, double H)
{
	if ( H < L )
	{
		double ur = L + ((L - H) * random_gen());
		return ur;
	} 
	else return -1;
}

// Exponential distribution function with center E
double exponential_gen(double E)
{
	double exponent = -E * log(random_gen());
	return exponent;
}
