#ifndef UTILITY
#define UTILITY

#include <string>
#include <map>
#include <stdbool.h>
#include "calendar.h"

using namespace std;

/*
 * Facility and Store have been merged into one class as the only difference
 * shows up to be the capacity.
 */
typedef struct
{
	//Only Store stats
	int capacityTotalUsage;
	int maxUsedCapacity;
	double lastUsed;
	int capacityRequests;
	
	//Both utilities stats
	double startUsage, endUsage, totalUsedTime;
	int totalRequests;

	double transTotalWaitingInQueue;
	double transTotalWaitingInQueue_2;
	double transMaxTimeWaitingInQueue;
	double transMinTimeWaitingInQueue;
	int transInQueue;
	
	int queueMaxLen;
	int queueLenSummed;
	double queueLastAltered;

} Stats;
			
//UTILITY
class Utility
{
	private:
		string name;
		int capacity;
		int remainingCap;
		bool priority_q;
		multimap<int, Event> queue;

		void initStats();
		bool isFacility();
		bool canBeSeized(int capacityToUse);
		int usedCapacity();
		void queueAltered(double Time);
		bool priorityQueue();

	public:
		Stats stats;

		Utility(string name, bool priority_q);
		Utility(string name, int capacity, bool priority_q);
		void Enter(Event *e, int capacityToUse);
		void Leave(Event *e, int capacityToFree);
		void Enqueue(Event *e, int priority);
		Event * Dequeue();
};

#endif