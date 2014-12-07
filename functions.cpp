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
#include <string>
#include <fstream>



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


// create file for histogram output
string create_file(string file_name)
{
	fstream fs;
	fs.open( file_name.c_str(), ios::out );
	fs.close();
	return file_name;
}

// write to created file
void data_out_write(string file_name, string data)
{
	fstream fs_out_file;
	fs_out_file.open ( file_name.c_str(), ios::out | ios::app );
	fs_out_file << data;
	fs_out_file.close();
}