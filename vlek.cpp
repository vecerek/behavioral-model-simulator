#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdbool.h>
#include "calendar.h"
#include "functions.h"
#include "utility.h"
#include "histogram.h"
using namespace std;

// Incilizacia

Utility * Stanoviste = new Utility("Stanoviste");
Utility * Kotvy = new Utility("Sklad kotev", 40);
Histogram * pokusy = new Histogram("Pocet pokusu nastoupit", 1, 1, 10);
Histogram * vlek_doba = new Histogram("Doba stravena lyzarem u vleku", 0, 1, 15);

// Netreba nam histogram (detaily)
Histogram * zsvodniciCakat = new Histogram("Doba cekani zavodniku"); 
int pokus;

const double doba_cesty = 4.0;

void Event::Behaviour()
{
	switch(this->next)
	{
		case 1: // zaber stanoviste
			{
				this->eventStartTime = Calendar::instance->getTime();
				pokus = 0;
				Stanoviste->Enter(this, 1);
				this->next = 2;
				break;
			}
		
		case 2: // pockaj si na kotvu
			{
				Kotvy->Enter(this, 1);
				this->next = 3;
				break;
			}
		
		case 3:  // skus si zobrat kotvu
			{
				Calendar::instance->Wait(this, exponential_gen(0.5));
				this->next = 4;
				break;
			}
		case 4: // uspel si / neuspel si
			{
				pokus++;
				if (random_gen() <= 0.1)
				{
					double actTime = this->getActivationTime();
					int priority = this->getPriority();
					double startTime = this->eventStartTime;
					
					Event * obehKotvy = new Event(this->getActivationTime() + doba_cesty * 2, 2);
					obehKotvy->next = 6;
					Calendar::instance->addEvent(obehKotvy);
					delete(obehKotvy);
					
					Event * kotvaVratenaStanovistu = new Event(actTime, priority);
					kotvaVratenaStanovistu->next = 2;
					kotvaVratenaStanovistu->eventStartTime = startTime;;
					Calendar::instance->addEvent(kotvaVratenaStanovistu);
					delete(kotvaVratenaStanovistu);
				}
				else
				{
					Stanoviste->Leave(this, 1);
					
					if (this->getPriority() == 1) 
						zsvodniciCakat->add_value(Calendar::instance->getTime() - this->eventStartTime);
					
					pokusy->add_value(pokus);
					pokus = 0;
					Calendar::instance->Wait(this, doba_cesty);
					this->next = 5;
				}
				break;
			}
			
		case 5: // kotva ide spat
			{
				vlek_doba->add_value(Calendar::instance->getTime() - this->eventStartTime);
				
				Event * navratKotvy = new Event(this->getActivationTime() + doba_cesty, 2);
				navratKotvy->next = 6;
				Calendar::instance->addEvent(navratKotvy);
				delete(navratKotvy);
				break;
			}

		case 6: // kotva sa vrati
			Kotvy->Leave(this, 1);
			break; 
	}
}


void Calendar::generateEvents()
{
	init_rand_gen();
	double actTime = 0;
	int priority = 0;

	// lyziar
	for (int i = 0; i != 10000; i++)
	{
		actTime += exponential_gen(1.0);
		Event * e = new Event(actTime, priority);
		this->addEvent(e);
		delete e;
	}
	actTime = 0;
	priority = 1;
	// zavodnik
	for (int i = 0; i != 10000; i++)
	{
		actTime += exponential_gen(10.0);
		Event * e = new Event(actTime, priority);
		this->addEvent(e);
		delete e;
	}
}



int main()
{
	string name = create_file("vlek.txt");
	Calendar * eventCalendar = Calendar::Instance(0.0, 1000.0);
	eventCalendar->generateEvents();
	eventCalendar->Run();
	
	// Vypis statistik
	data_out_write(name, Stanoviste->Statistics());
	data_out_write(name, Kotvy->Statistics());
	vlek_doba->data_out(name);
	pokusy->data_out(name);
	zsvodniciCakat->data_out(name);
	
	return EXIT_SUCCESS;
}

