/*
  	This file stores implementation of histogram.
  	Author: Matus Turic, xturic01.fit.vutbr.cz
  	Name: histogram.cpp
  	Subject: IMS 
*/

#include "histogram.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;

// histogram constructor -> set variables
Histogram(string out_name, int from, int step, int num_of_classes);
{
	this->write_it = false;
	this->out_name = out_name;
	this->from = from;
	this->step = step;
	this->num_of_classes = num_of_classes;
}

// histogram constructor 2 -> set variables -> writes only name
Histogram(string out_name);
{
	this->write_it = true;
	this->out_name = out_name;
}

// create file for histogram output
void create_file(string file_name)
{

	if(!this->is_set)
	{
		this->file_name = file_name;
	}
	
	fstream fs;
	fs.open( file_name.c_str(), ios::out );
	fs.close();
}

// write to created file
void data_out_write(string data)
{
	fstream fs_out_file;
	fs_out_file.open ( this->file_name.c_str(), ios::out | ios::app );
	fs_out_file << data;
	fs_out_file.close();
}

// get min value 
int min_value()
{
	map<int, int>::iterator min;
	min = this->histogram_map.begin();
	return min->first;
}

// get max value
int max_value()
{
	map<int, int>::iterator max;
	max = this->histogram_map.rbegin();
	return max->first;
}

// add value to histogram map
void add_value(double value);
{
	map<double , int>::iterator add;
	add = this->histogram_map.find(value);
	if (add == this->histogram_map.end())
	{
		this->histogram_map.insert(make_pair(value, 1));
	}
	else
	{
		add->second++;
	}
}

// return number of records or number of records in interval
// int_rec_value = true -> value, else all values
int records_num(int begin, int end, bool int_rec_value)
{
	map<int, int>::iterator records;
	int value_all = 0;
	double value = 0;
	int = this->histogram_map.begin();

	while(records != this->histogram_map.end())
	{
		if ((records->first >= begin) && (records->first < end)) 
		{
			 value = value + records->second;
		}
		value_all = value_all + records->second;
		records++;
	}
	return int_rec_value ? value : value_all;
}

// get sum of each rows
double sum_num()
{
	map<int, int>::iterator sum;
	double value = 0;
	sum = this->histogram_map.begin();

	while(sum != this->histogram_map.end())
	{

		value = value + (sum->first * sum->second);
		sum++;
	}
	return value;
}

// get standard deviation
double deviation_num()
{
	map<int, int>::iterator dev;
	double value = 0.0;
	double average_value = this->sum_num() / (double)this->records_num(0, 0, false);
	dev = this->histogram_map.begin();
	
	while(dev != this->histogram_map.end())
	{
		value = value + pow((dev->first - average_value), 2) * dev->second;
		dev++;
	}
	
	return sqrt(value / (double)this->records_num(0, 0, false));
}

// data to file
void data_out()
{
	int record_value = 0;
	string out_data;
	string line = "+";
	string data_line = "";
	string min = "";
	string max = "";
	string record = "";
	string average = "";
	string deviation = "";
	string line_hist = "+-----------+-----------+------------+-----------+-----------+\n";

	line.insert(1, 58,' ');
	line += "+\n";

	out_data = out_data + line;

	if(this->write_it == false)
	{
		data_line = "| HISTOGRAM " + this->out_name;

		data_line.insert(12, 47,' ');
		data_line = data_line + "|\n" + line;
		out_data = out_data + data_line;
	}
		
	data_line = "| STATISTIC " + this->out_name;

	data_line.insert(12, 47,' ');
	data_line = data_line + "|\n" + line;
	out_data = out_data + data_line;

	///////////////////////////////////////////////////////////// MIN MAX
	min = to_string(this->min_value());
	max = to_string(this->max_value());

	data_line = "| Min = " + min + "           Max = " + max; 
	
	data_line.insert(25 + min.length() + max.length(), 34 - min.length() - max.length(),' ');
	data_line = data_line + "|\n";
	out_data = out_data + data_line;

	///////////////////////////////////////////////////////////// RECORD
	record = to_string(this->records_num(0, 0, false));
	record_value = records_num(0, 0, false); 
	data_line = "| Number of records = " + record;
	data_line.insert(22 + record.length(), 37 - record.length(),' ');

	data_line = data_line + "|\n";
	out_data = out_data + data_line;

	///////////////////////////////////////////////////////////// AVERAGE
	if(record_value != 0)
	{
		average = to_string(this->sum_num() / (double)record_value);
	}
	else
	{
		average = "undef";
	}

	data_line = "| Average value = " + average;

	data_line.insert(18 + average.length(), 41 - average.length(),' ');
	data_line = data_line + "|\n";
	out_data = out_data + data_line;

	///////////////////////////////////////////////////////////// DEVIATION
	if(record_value >= 2)
	{
		deviation = to_string(this->deviation_num());
	}
	else
	{
		deviation = "undef";
	}

	data_line = "| Standard deviation = " + deviation;
	data_line.insert(23 + deviation.length(), 36 - deviation.length(),' ');
	data_line = data_line + "|\n" + line;
	out_data = out_data + data_line;

	if (this->write_it)
	{
		out_data = out_data + "\n";		
		this->data_out_write(out_data);
		return;
	}
	
	///////////////////////////////////////////////////////////// FROM TO ....

					
	data_line = line_hist;
	data_line = data_line + "|   from    |     to    |      n     |    rel    |    sum    |\n";
	data_line = data_line + line_hist;
	out_data = out_data + data_line;

	int from_index = this->from;
	int step_index = this->step;
	double sum_value = 0;
	
	int index = 1;
	while(index <= this->num_of_classes)
	{
		int interval = this->records_num(from_index, from_index + step_index, true);
		double rel_value = ( double ) interval / ( double ) record_value;

		///////////////////////////////////////////////////////////// FROM
		string from_str = to_string(from_index);

		data_line = "|";
		int parity = from_str.length() % 2; 
		data_line.insert(1,5 - from_str.length() / 2, ' ');
		data_line = data_line + from_str;
		data_line.insert(data_line.length(),6 - parity - from_str.length() / 2, ' ');
		data_line = data_line + "|";

		from_index = from_index + step_index;

		///////////////////////////////////////////////////////////// TO
		string to_str = to_string(from_index);
		
		parity = to_str.length() % 2; 
		data_line.insert(data_line.length(),5 - to_str.length() / 2, ' ');
		data_line = data_line + to_str;
		data_line.insert(data_line.length(),6 - parity - to_str.length() / 2, ' ');
		data_line = data_line + "|";

		///////////////////////////////////////////////////////////// N
		string n_str = to_string(interval);

		parity = n_str.length() % 2; 
		data_line.insert(data_line.length(),6 - n_str.length() / 2, ' ');
		data_line = data_line + n_str;
		data_line.insert(data_line.length(),6 - parity - n_str.length() / 2, ' ');
		data_line = data_line + "|";

		///////////////////////////////////////////////////////////// REL

		
		if(record_value != 0) 
		{
			sum_value = sum_value + rel_value;
			string rel_str = to_string(rel_value);
		}
		else
		{
			string rel_str = "undef";
		}

		parity = rel_str.length() % 2; 
		data_line.insert(data_line.length(),5 - rel_str.length() / 2, ' ');
		data_line = data_line + rel_str;
		data_line.insert(data_line.length(),6 - parity - rel_str.length() / 2, ' ');
		data_line = data_line + "|";
	
		///////////////////////////////////////////////////////////// SUM

		string sum_str = "";

		if(record_value != 0 ) 
		{
			string sum_str = to_string(sum_value);
		}
		else 
		{
			string sum_str = "undef";

		}
		

		parity = sum_str.length() % 2; 
		data_line.insert(data_line.length(),5 - sum_str.length() / 2, ' ');
		data_line = data_line + rel_str;
		data_line.insert(data_line.length(),6 - parity - sum_str.length() / 2, ' ');
		data_line = data_line + "|";

		out_data = out_data + data_line + "\n";

		index++;
	}

	out_data = out_data + line_hist + "\n";

	this->data_out_write(out_data);


}

