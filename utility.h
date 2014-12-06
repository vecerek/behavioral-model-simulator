#ifndef UTILITY
#define UTILITY

#include <string>
#include <stdbool.h>
#include <map>
#include "calendar.h"

using namespace std;

/*
 * Facility and Store have been merged into one class as the only difference
 * shows up to be the capacity.
 */
typedef struct
{
	//Only Store stats
	double capacityTotalUsage;
	int minUsedCapacity;
	int maxUsedCapacity;
	double lastUsed;
	int capacityRequests;
	
	//Both utilities stats
	double startUsage, endUsage, totalUsedTime;
	int totalRequests;

	int queueIn, queueOut;
	double transTotalWaitingInQueue;
	double transTotalWaitingInQueue_2;
	double transMaxTimeWaitingInQueue;
	double transMinTimeWaitingInQueue;
	
	int queueMaxLen;
	int queueLenSummed;
	double queueLastAltered;

} Stats;

typedef multimap<int, Event> samePriorityQueue;
			
//UTILITY
class Utility
{
	private:
		int capacity;
		int remainingCap;
		map<int, samePriorityQueue> queue;

		void initStats();
		bool isFacility();
		bool canBeSeized(int capacityToUse);
		int usedCapacity();
		void queueAltered(double Time);

	public:
		string name;
		Stats stats;
		string utilityStats;

		Utility(string name);
		Utility(string name, int capacity);
		void Enter(Event *e, int capacityToUse);
		void Leave(Event *e, int capacityToFree);
		void Enqueue(Event *e);
		Event * Dequeue();
		void Statistics();
};

#endif