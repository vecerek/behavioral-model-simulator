/*
  	This file stores implementation of utilities Facility and Store.
  	Author: Attila Večerek, xvecer17
  	Name: utility.cpp
  	Course: IMS 
*/

#include <string>
#include <stdbool>
#include <cfloat>
#include <cmath>
#include "utility.h"

using namespace std;

//UTILITY implementation
Utility::Utility(string name, bool priority_q = FALSE)
{
	this->name = name;
	this->capacity = this->remainingCap = 1;
	this->priority_q = priority_q;
	this->initStats();
}

Utility::Utility(string name, int cap, bool priority_q = FALSE)
{
	this->name = name;
	this->capacity = this->remainingCap = cap;
	this->priority_q = priority_q;
	this->initStats();
}

void Utility::InitStats()
{
	if(this->capacity > 1)
	{
		this->stats.capacityTotalUsage = 0;
		this->stats.maxUsedCapacity = 0;
		this->stats.lastUsed = 0.0;
		this->stats.capacityRequests = 0;
	}

	this->stats.startUsage = 0.0;
	this->stats.endUsage = 0.0;
	this->stats.totalUsedTime = 0.0;
	this->stats.totalRequests = 0;

	this->stats.transTotalWaitingInQueue = 0.0;
	this->stats.transTotalWaitingInQueue_2 = 0.0;
	this->stats.transMaxTimeWaitingInQueue = 0.0;
	this->stats.transMinTimeWaitingInQueue = DBL_MAX;
	this->stats.transInQueue = 0;

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

bool Utility::priorityQueue()
{
	return this->priority_q;
}

/*
 * Transaction enters the store and uses some of its capacity.
 */
void Utility::Enter(Event *e, int capacityToUse = 1)
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
		//Increment the number of total requests for this utility
		this->stats.totalRequests++;
	}
	else
		this->Enqueue(e);
}

void Utility::Leave(Event *e, int capacityToFree = 1)
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Is the utility a facility?
	bool Facility = this->isFacility();

	//Action taken only if the instance is a Store
	if(!Facility)
	{
		//The utility's actual capacity being used
		int usedCap = this->usedCapacity();
		//Summarizing the total capacity used of the store the same way
		//as in the method Enter()
		this->stats.capacityTotalUsage += usedCap * (actualTime - this->stats.lastUsed);
		//Set the time to the value, when this calculation was processed = now
		this->stats.lastUsed = actualTime;
	}

	//Action always taken, no matter about the type of utility
	//Does something wait in the Queue?
	if(!this->queue.empty())
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
	{	//the Queue is empty
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
		else if(this->remainingCap == this->capacity)
			this->stats.totalUsedTime += actualTime - this->stats.startUsage;
	}
}

void Utility::Enqueue(Event *e, int priority = 1)
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Get the size of the queue
	int queueSize = this->queue.size();
	//Summarizing the total length of queue, adding the actual queue
	//length for every timeunit since we did it last time
	this-stats.>queueLenSummed += queueSize * (actualTime - this->stats.queueLastAltered);
	//Adding the event into the Queue
	this->queue.insert(make_pair(priority, *e));
	//the event is in the queue, so we have to set its start time in 
	//the queue, which has also been altered, so we have to set its time
	e->startWaitingInQueue(actualTime);
	this->queueAltered(actualTime);
	//Incrementing the total amount of transactions inserted into this
	//queue for statistical purposes
	this->stats.transInQueue++;
	//Checking if the queue length record has been beaten
	if(++actSize > this->stats.queueMaxLen)
		this->stats.queueMaxLen = actSize;
}

Event * Utility::Dequeue()
{
	//Actual simulation time
	double actualTime = Calendar::instance->getTime();
	//Get the size of the queue
	int queueSize = this->queue.size();
	//Summarizing the total length of queue, adding the actual queue
	//length for every timeunit since we did it last time
	this-stats.>queueLenSummed += queueSize * (actualTime - this->stats.queueLastAltered);
	
	//Is the Queue a priority queue?
	if(this->priorityQueue()) 
		//Get the event with the heighest priority
		Event *e = this->queue.rbegin()->second;
	else
		//Get the first pushed event
		Event *e = this->queue.begin()->second;

	//Calculate the time spent in queue
	double spentInQueue = actualTime - e->queueStartTime;
	//Set minimal and maximal queue time, if new record's been reached
	if(spentInQueue > this->stats.transMaxTimeWaitingInQueue)
		this->stats.transMaxTimeWaitingInQueue = spentInQueue;
	if(spentInQueue < this->stats.transMinTimeWaitingInQueue)
		this->stats.transMinTimeWaitingInQueue = spentInQueue;

	//Sum up the total time spent in the queue of every transaction
	//and sum up the time's square to be able to calculate the standard
	//deviation
	this->transTotalWaitingInQueue += spentInQueue;
	this->transTotalWaitingInQueue_2 += pow(spentInQueue, 2);

	//Erasing the event from the multimap = dequeue it definitely
	if(this->priorityQueue())
		this->queue.erase(this->queue.rbegin());
	else
		this->queue.erase(this->queue.begin());

	//the queue has also been altered, so we have to set its time
	this->queueAltered(actualTime);

	return e;
}