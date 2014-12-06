/*
  	This file stores implementation of histogram.
  	Author: Matus Turic, xturic01.fit.vutbr.cz
  	Name: histogram.h
  	Subject: IMS 
*/

#ifndef HISTOGRAM
#define HISTOGRAM


#include <string>
#include <stdbool.h>
#include <map>

using namespace std;

class Histogram
{

	protected:
		string file_name;
		string out_name;
		bool is_set;
		bool write_it;
		int from;
		int step;
		int num_of_classes;
		map <double, int> histogram_map;
	public:
		Histogram(string out_name, int from, int step, int num_of_classes);
		Histogram(string out_name);

		void create_file(string file_name);
		void data_out_write( string data );

		double min_value();
		double max_value();
		void add_value(double value);
		
		int records_num(double begin, double end, bool int_rec_value);
		double sum_num();
		double deviation_num();

		void data_out();
	
};


#endif
