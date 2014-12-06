#ifndef EVENT_CALENDAR
#define EVENT_CALENDAR

#include <map>
#include <stdbool.h>

using namespace std;

class Calendar;
class Event;

//EVENT

class Event
{
	protected:
		double aTime;
		int priority;
		double queueStartTime;
	
	public:
		Event(double aTime, int priority);
		double getActivationTime();
		void setActivationTime(double aTime);
		int getPriority();
		void setPriority(int priority);
		void startWaitingInQueue(double startTime);
		double waitingSince();

		void Behaviour();
};

//Stores Event and it's priority in a multimap as key-value pair.
//We use multimap, thus it can store several Events with the same priority
typedef multimap<int, Event> eventPriority;

//must be implemented as singleton
class Calendar
{
	private:
		double sim_start_t;
		double sim_end_t;
		double sim_actual_t;
		map<double, eventPriority> e_calendar;

	public:
		static Calendar *instance;
		static Calendar *Instance(double start_t, double end_t);
		
		Calendar(double start_t, double end_t);
		bool isEmpty();
		Event * getEvent();
		void addEvent(Event *e);
		void Wait(Event *e, double aTime);
		double getStart_t();
		double getEnd_t();
		double getTime();
		//virtual, so it can be customized
		virtual void generateEvents();
		void Run();
};

#endif