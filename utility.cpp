/*
  	This file stores implementation of utilities Facility and Store.
  	Author: Attila Večerek, xvecer17
  	Name: utility.cpp
  	Course: IMS 
*/

#include <string>
#include <stdbool.h>
#include <cfloat>
#include <cmath>
#include "utility.h"
#include "histogram.h"

using namespace std;

//UTILITY implementation
Utility::Utility(string name)
{
	this->name = name;
	this->capacity = this->remainingCap = 1;
	this->utilityStats = "";
	this->initStats();
}

Utility::Utility(string name, int cap)
{
	this->name = name;
	this->capacity = this->remainingCap = cap;
	this->utilityStats = "";
	this->initStats();
}

void Utility::initStats()
{
	if(this->capacity > 1)
	{
		this->stats.capacityTotalUsage = 0.0;
		this->stats.minUsedCapacity = this->capacity + 1;
		this->stats.maxUsedCapacity = 0;
		this->stats.lastUsed = 0.0;
		this->stats.capacityRequests = 0;
	}

	this->stats.startUsage = 0.0;
	this->stats.endUsage = 0.0;
	this->stats.totalUsedTime = 0.0;
	this->stats.totalRequests = 0;

	this->stats.queueIn = this->stats.queueOut = 0;
	this->stats.transTotalWaitingInQueue = 0.0;
	this->stats.transTotalWaitingInQueue_2 = 0.0;
	this->stats.transMaxTimeWaitingInQueue = 0.0;
	this->stats.transMinTimeWaitingInQueue = DBL_MAX;

	this->stats.queueMaxLen = 0;
	this->stats.queueLenSummed = 0;
	this->stats.queueLastAltered = 0.0;
}

bool Utility::isFacility()
{
	return this->capacity == 1;
}

bool Utility::canBeSeized(int capacityToUse = 1)
{
	return this->remainingCap >= capacityToUse;
}

int Utility::usedCapacity()
{
	return this->capacity - this->remainingCap;
}

void Utility::queueAltered(double Time)
{
	this->stats.queueLastAltered = Time;
}

bool Utility::isQueueEmpty()
{
	if(this->queue.empty()) return true;
	else
	{
		map<int, samePriorityQueue>::iterator it;
		it = this->queue.end();
		it--;
		return it->second.empty();
	}
}

int Utility::getQueueLength()
{
	int size = 0;
	map <int, samePriorityQueue>::iterator it = this->queue.begin();
	while(it != this->queue.end())
	{
		size += it->second.size();
		it++;
	}
	return size;
}

/*
 * Transaction enters the store and uses some of its capacity.
 */
void Utility::Enter(Event *e, int capacityToUse)
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Has utility enough free capacity?
	if(this->canBeSeized(capacityToUse))
	{
		if(this->isFacility())
		{
			//Set starting time of usage for statistics
			this->stats.startUsage = actualTime;
			//Decrease the remaining capacity by the proper value
			this->remainingCap -= capacityToUse;
		}
		else
		{
			//The utility's actual capacity being used
			int usedCap = this->usedCapacity();
			//Set startUsage of Store if it's been not used
			if(usedCap == 0)
				this->stats.startUsage = actualTime;
			
			//Summarizing the total capacity used until now for every timeunit,
			//so we can later on calculate the avarage capacity used during the
			//simulation.
			this->stats.capacityTotalUsage += usedCap * (actualTime - this->stats.lastUsed);
			//Set the time to the value, when this calculation was processed = now
			this->stats.lastUsed = actualTime;
			//Decrease the remaining capacity by the proper value
			this->remainingCap -= capacityToUse;
			//get the recent capacity being used of the Store
			usedCap += capacityToUse;
			//Increment the total capacity requests by transaction for statistics
			this->stats.capacityRequests += capacityToUse;
			//Set maxUsedCapacity, if the actual value is greater then our max
			if(usedCap > this->stats.maxUsedCapacity)
				this->stats.maxUsedCapacity = usedCap;
		}
		Calendar::instance->Wait( e, 0 );
	}
	else
		this->Enqueue(e);
	
	//Increment the number of total requests for this utility
	this->stats.totalRequests++;
}

void Utility::Leave(Event *e, int capacityToFree)
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Is the utility a facility?
	bool Facility = this->isFacility();
	int usedCap;

	//Action taken only if the instance is a Store
	if(!Facility)
	{
		//The utility's actual capacity being used
		usedCap = this->usedCapacity();
		//Summarizing the total capacity used of the store the same way
		//as in the method Enter()
		this->stats.capacityTotalUsage += usedCap * (actualTime - this->stats.lastUsed);
		//Set the time to the value, when this calculation was processed = now
		this->stats.lastUsed = actualTime;
	}
	
	//Action always taken, no matter about the type of utility
	//Does something wait in the Queue?
	if(!this->isQueueEmpty())
	{
		//Get the next event from the Queue
		Event *dequeued = this->Dequeue();
		//Set the event's activation time to the value of the released
		//event's activation time, so the dequeued event can be pushed
		//to its proper place in the event calendar
		dequeued->setActivationTime(e->getActivationTime());
		//add the event into the event calendar
		Calendar::instance->addEvent(dequeued);
	}
	else
	{
		//the Queue is empty
		//Increase the remaining capacity
		this->remainingCap += capacityToFree;
		/*
		 * If the insatance of the utility is a Facility,
		 * the utility's total time of being used is incremented.
		 * The Store's total time of being used is incremented only
		 * if it gets all its capacity back meaning that all the
		 * transactions have left the Store.
		 */
		if(Facility)
			this->stats.totalUsedTime += actualTime - this->stats.startUsage;
		else
		{
			if(this->remainingCap == this->capacity)
				this->stats.totalUsedTime += actualTime - this->stats.startUsage;
			if((usedCap > 0) && (usedCap < this->stats.minUsedCapacity))
				this->stats.minUsedCapacity = usedCap;
		}
	}
}

void Utility::Enqueue(Event *e)
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Get the size of the queue
	int queueSize = this->getQueueLength();
	//Summarizing the total length of queue, adding the actual queue
	//length for every timeunit since we did it last time
	this->stats.queueLenSummed += queueSize * (actualTime - this->stats.queueLastAltered);
	
	/*
	 * The queue is implemented in a way that it acts like a priority queue,
	 * when there are added events with different priorities.
	 * Firstly, we get the event's priority, which will be the key in the map.
	 * The multimap's keys are the activation times of the events.
	 * Each record in this map models a subsequent part of the whole queue
	 * with the same priority.
	 * The greater the key, the higher is the priority of that specific part
	 * of the queue.
	 * Each subsequent part is a multimap, which models the arrival of events
	 * with the same priority.
	 */
	int event_prior = e->getPriority();
	double event_aTime = e->getActivationTime();
	map <int, samePriorityQueue>::iterator it = this->queue.find(event_prior);
	if(it == this->queue.end())
	{
		this->queue.insert(make_pair(event_prior, samePriorityQueue()));
		it = this->queue.find(event_prior);
		it->second.insert(make_pair(event_aTime, *e));
	}
	else
		it->second.insert(make_pair(event_aTime, *e));
	//the event is in the queue, so we have to set its start time in 
	//the queue, which has been altered, so we have to set its time
	e->startWaitingInQueue(actualTime);
	this->queueAltered(actualTime);
	//Incrementing the total amount of transactions inserted into this
	//queue for statistical purposes
	this->stats.queueIn++;
	//Checking if the queue length record has been beaten
	if(++queueSize > this->stats.queueMaxLen)
		this->stats.queueMaxLen = queueSize;
}

Event * Utility::Dequeue()
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Get the size of the queue
	int queueSize = this->getQueueLength();
	//Summarizing the total length of queue, adding the actual queue
	//length for every timeunit since we did it last time
	this->stats.queueLenSummed += queueSize * (actualTime - this->stats.queueLastAltered);
	
	/*
	 * The highest priority events are found in the last record of our map.
	 * So we access that record. All events in this record(which is a multimap)
	 * have the same priority but we have to find the first event of this subqueue.
	 * As thez all have the same priority, it will be on the beginning(first record of multimap).
	 */
	multimap<double, Event>::iterator it_in;
	map<int, samePriorityQueue>::iterator it_out;
	it_out = this->queue.end();
	it_out--;
	it_in = it_out->second.begin();
	Event *e = &it_in->second;
	it_out->second.erase(it_in);
	it_in = it_out->second.begin();
	if(it_out->second.empty())
		this->queue.erase(it_out);

	this->stats.queueOut++;

	//Calculate the time spent in queue
	double spentInQueue = actualTime - e->waitingSince();
	//Set minimal and maximal queue time, if new record's been reached
	if(spentInQueue > this->stats.transMaxTimeWaitingInQueue)
		this->stats.transMaxTimeWaitingInQueue = spentInQueue;
	if(spentInQueue < this->stats.transMinTimeWaitingInQueue)
		this->stats.transMinTimeWaitingInQueue = spentInQueue;

	//Sum up the total time spent in the queue of every transaction
	//and sum up the time's square to be able to calculate the standard
	//deviation
	this->stats.transTotalWaitingInQueue += spentInQueue;
	this->stats.transTotalWaitingInQueue_2 += pow(spentInQueue, 2);

	//the queue has also been altered, so we have to set its time
	this->queueAltered(actualTime);

	return e;
}

string Utility::Statistics()
{
	const int LINE_LENGTH = 60;
	double Time = Calendar::instance->getTime();
	bool Facility = this->isFacility();
	string line_divide = "+";

	line_divide.insert(1, 58, '-');
	line_divide += "+\n";

	this->utilityStats += line_divide;
	string utility = Facility ? "FACILITY" : "STORE";
	string line_utility = "| ";
	line_utility += utility + " " + this->name;
	line_utility.insert(line_utility.length(), LINE_LENGTH - line_utility.length() - 1, ' ');
	this->utilityStats += line_utility + "|\n";

	this->utilityStats += line_divide;
	string line_capacity;
	if(Facility)
	{
		line_capacity = "| Status: ";
		if(this->canBeSeized())
			line_capacity += "NOT BUSY";
		else
			line_capacity += "BUSY";
	}
	else
		line_capacity = "| Capacity: " + to_string(this->capacity) + " (in use: " + to_string(this->usedCapacity()) + ", free: " + to_string(this->remainingCap) + ")";

	line_capacity.insert(line_capacity.length(), LINE_LENGTH - line_capacity.length() - 1, ' ');
	this->utilityStats += line_capacity + "|\n";

	string line_t_interval = "| Time interval: " + to_string((int) Calendar::instance->getStart_t()) + " - " + to_string((int) Calendar::instance->getEnd_t());
	line_t_interval.insert(line_t_interval.length(), LINE_LENGTH - line_t_interval.length() - 1, ' ');
	this->utilityStats += line_t_interval + "|\n";

	string line_requests = "| ";
	string request = Facility ? "Number of requests: " : "Number of Enter Operations: ";
	line_requests += request + to_string(this->stats.totalRequests);
	line_requests.insert(line_requests.length(), LINE_LENGTH - line_requests.length() - 1, ' ');
	this->utilityStats += line_requests + "|\n";

	/*if(!Facility)
	{
		string line_avg_cap_on_enter = "| Average capacity requested on Enter operation: " + to_string(this->stats.capacityRequests / (double)this->stats.totalRequests);
		line_avg_cap_on_enter.insert(line_avg_cap_on_enter.length(), LINE_LENGTH - line_avg_cap_on_enter.length() - 1, ' ');
		this->utilityStats += line_avg_cap_on_enter + "|\n";
	}*/

	if(Facility)
	{
		string line_utilization = "| Average Utilization: " + to_string(this->stats.totalUsedTime / Calendar::instance->getTime());
		line_utilization.insert(line_utilization.length(), LINE_LENGTH - line_utilization.length() - 1, ' ');
		this->utilityStats += line_utilization + "|\n";
	}
	else
	{
		string line_capacities = "| Minimal used capacity: " + to_string(this->stats.minUsedCapacity);
		line_capacities.insert(line_capacities.length(), LINE_LENGTH - line_capacities.length() - 1, ' ');
		this->utilityStats += line_capacities + "|\n";

		line_capacities = "| Maximal used capacity: " + to_string(this->stats.maxUsedCapacity);
		line_capacities.insert(line_capacities.length(), LINE_LENGTH - line_capacities.length() - 1, ' ');
		this->utilityStats += line_capacities + "|\n";

		line_capacities = "| Average used capacity: " + to_string(this->stats.capacityTotalUsage / Time);
		line_capacities.insert(line_capacities.length(), LINE_LENGTH - line_capacities.length() - 1, ' ');
		this->utilityStats += line_capacities + "|\n";
	}

	//QUEUE
	this->utilityStats += line_divide + "Input queue '" + this->name + ".Q'\n" + line_divide;
	string line_queue = "| QUEUE Q";
	line_queue.insert(line_queue.length(), LINE_LENGTH - line_queue.length() - 1, ' ');
	this->utilityStats += line_queue + "|\n" + line_divide;

	string line_q_t_interval = "| Time interval: " + to_string((int) Calendar::instance->getStart_t()) + " - " + to_string((int) Calendar::instance->getEnd_t());
	line_q_t_interval.insert(line_q_t_interval.length(), LINE_LENGTH - line_q_t_interval.length() - 1, ' ');
	this->utilityStats += line_q_t_interval + "|\n";

	string line_incoming = "| Incoming: " + to_string(this->stats.queueIn);
	line_incoming.insert(line_incoming.length(), LINE_LENGTH - line_incoming.length() - 1, ' ');
	this->utilityStats += line_incoming + "|\n";

	string line_outgoing = "| Outgoing: " + to_string(this->stats.queueOut);
	line_outgoing.insert(line_outgoing.length(), LINE_LENGTH - line_outgoing.length() - 1, ' ');
	this->utilityStats += line_outgoing + "|\n";

	string line_curr_length = "| Current length: " + to_string(this->getQueueLength());
	line_curr_length.insert(line_curr_length.length(), LINE_LENGTH - line_curr_length.length() - 1, ' ');
	this->utilityStats += line_curr_length + "|\n";

	string line_max_len = "| Maximal length: " + to_string(this->stats.queueMaxLen);
	line_max_len.insert(line_max_len.length(), LINE_LENGTH - line_max_len.length() - 1, ' ');
	this->utilityStats += line_max_len + "|\n";

	this->stats.queueLenSummed += this->getQueueLength() * (Time - this->stats.queueLastAltered);
	double avg_q_len = this->stats.queueLenSummed / Time;
	string line_avg_len = "| Average length: " + to_string(avg_q_len);
	line_avg_len.insert(line_avg_len.length(), LINE_LENGTH - line_avg_len.length() - 1, ' ');
	this->utilityStats += line_avg_len + "|\n";

	bool noQueue = this->stats.transMinTimeWaitingInQueue == DBL_MAX;
	string line_min_time = "| Minimal time: ";
	if(noQueue)
		line_min_time += "undef";
	else
		line_min_time += to_string(this->stats.transMinTimeWaitingInQueue);
	line_min_time.insert(line_min_time.length(), LINE_LENGTH - line_min_time.length() - 1, ' ');
	this->utilityStats += line_min_time + "|\n";

	string line_max_time = "| Maximal time: " + to_string(this->stats.transMaxTimeWaitingInQueue);
	line_max_time.insert(line_max_time.length(), LINE_LENGTH - line_max_time.length() - 1, ' ');
	this->utilityStats += line_max_time + "|\n";

	double avg_time;
	string avg_time_s;
	if(this->stats.transTotalWaitingInQueue == 0)
		avg_time_s = "undef";
	else
	{
		avg_time = this->stats.transTotalWaitingInQueue / this->stats.queueOut;
		avg_time_s = to_string(avg_time);
	}
	string line_avg_time = "| Average time: " + avg_time_s;
	line_avg_time.insert(line_avg_time.length(), LINE_LENGTH - line_avg_time.length() - 1, ' ');
	this->utilityStats += line_avg_time + "|\n";

	string deviation;
	if(this->stats.queueOut < 2)
		deviation = "undef";
	else
		deviation = to_string(sqrt((this->stats.transTotalWaitingInQueue_2 - (this->stats.queueOut * pow(avg_time, 2))) / (this->stats.queueOut - 1)));
	string line_std_dev = "| Standard deviation: " + deviation;
	line_std_dev.insert(line_std_dev.length(), LINE_LENGTH - line_std_dev.length() - 1, ' ');
	this->utilityStats += line_std_dev + "|\n" + line_divide + "\n";

	return this->utilityStats;
}