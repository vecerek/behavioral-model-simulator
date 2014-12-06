/*
  	This file stores implementation of events' calendar.
  	Author: Attila Večerek, xvecer17
  	Name: calendar.cpp
  	Course: IMS 
*/
#include <stdbool.h>
#include <iostream>
#include "calendar.h"
#include "functions.h"
#include <map>
using namespace std;


//EVENT implementation
Event::Event(double aTime, int priority)
{
	this->aTime = aTime;
	this->priority = priority;
}

double Event::getActivationTime()
{
	return this->aTime;
}

void Event::setActivationTime(double aTime)
{
	this->aTime = aTime;
}

int Event::getPriority()
{
	return this->priority;
}

void Event::setPriority(int priority)
{
	this->priority = priority;
}

void Event::startWaitingInQueue(double startTime)
{
	this->queueStartTime = startTime;
}

double Event::waitingSince()
{
	return this->queueStartTime;
}

//CALENDAR implementation
//if NULL, no instance of Calender has been created, yet
Calendar *Calendar::instance = NULL;

//setting the start and end time of simulation
Calendar::Calendar(double start_t, double end_t)
{
	this->sim_start_t = start_t;
	this->sim_end_t = end_t;
}

//instantiation of Calendar, if no instance has been created, create one
Calendar *Calendar::Instance(double start_t, double end_t)
{
	if(instance == NULL)
		instance = new Calendar(start_t, end_t);
	return instance;
}

bool Calendar::isEmpty()
{
	return this->e_calendar.empty();
}

Event * Calendar::getEvent()
{
	//check whether event calendar is empty
	if(this->isEmpty()) return NULL;
	//get the event with the lowest activation time and highest priority
	multimap<int, Event>::iterator it;
	it = this->e_calendar.begin()->second.end();
	it--;
	//save the event temporarily
	Event *tmp = &it->second;
	//erase the event from the multimap, so it can't be pulled more times from the calendar
	this->e_calendar.begin()->second.erase(it);
	//check if the multimap of events(of the same aTime) is empty
	if(this->e_calendar.begin()->second.empty())
		//if empty, erase the record
		this->e_calendar.erase(this->e_calendar.begin());
	
	return tmp;
}

/*
 * Inserts event into the event calendar onto its proper place.
 */
void Calendar::addEvent(Event * e)
{
	double aTime = e->getActivationTime();
	//check if event with that activation time already exists(not likely to happen but still possible)
	map <double, eventPriority>::iterator it = this->e_calendar.find(aTime);
	if(it == this->e_calendar.end())
	{
		//not found
		//insert empty event-priority and activation time pair
		this->e_calendar.insert(make_pair(aTime, eventPriority()));
		//get a reference to the newly added event into the calendar
		it = this->e_calendar.find(aTime);
		//it must have been found -> no check for end()
		//insert event and its priority into eventPriority Map(the value of our e_calendar map)
		it->second.insert(make_pair(e->getPriority(), *e));
	}
	else
		//the same activation time has been found -> add new event into the multimap
		it->second.insert(make_pair(e->getPriority(), *e));
}

void  Calendar::Wait(Event *e, double aTime)
{
	//check if greater than zero to avoid pointless, resp. unnecessary method invoking 
	if(aTime > 0)
		e->setActivationTime(e->getActivationTime() + aTime);
	//finally add event to the calendar
	this->addEvent(e);
}

double Calendar::getStart_t()
{
	return this->sim_start_t;
}

double Calendar::getEnd_t()
{
	return this->sim_end_t;
}

//gets actual simulation time
double Calendar::getTime()
{
	return this->sim_actual_t;
}

void Calendar::Run()
{
	//run until calendar becomes empty
	while(!this->isEmpty())
	{
		Event *e = this->getEvent();
		double aTime = e->getActivationTime();
		//check if the simulation ran out of time by checking the activation time of the event
		if(aTime > this->sim_end_t)
			//end simulation
			break;
		this->sim_actual_t = aTime;
		e->Behaviour();
	}
}