/*
  	This file stores implementation of random generator functions
  	Author: Matus Turic, xturic01.fit.vutbr.cz
  	Name: functions.h
  	Subject: IMS 
*/


#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
using namespace std;

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

// create file for histogram output
string create_file(string file_name);

// write to created file
void data_out_write(string file_name, string data);

#endif
