#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdbool.h>
#include "calendar.h"
#include "utility.h"
#include "functions.h"
#include "histogram.h"
using namespace std;

////////////////////////////////////////////////////////////////////////
// GLOBAL VARS - GLOBALNE PREMENNE -------------------------------------

Utility * Pocitace = new Utility( "Pocitace", 10 );
Histogram * dobaStudiu = new Histogram( "Doba stravena v ucebne", 0, 10, 50 );
double dobaUceni = 0;

// ---------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////


void Event::Behaviour()
{	
	switch( this->next )
	{
		case 1:
			if ( Pocitace->remainingCap == 0 )
			{
				if ( random_gen() <= 0.6 )
				{
					this->setPriority( 2 );
					Pocitace->Enter( this, 1 );
					this->next = 2;
				}
				else
				{
					if ( random_gen() <= 0.2 )
					{
						Calendar::instance->Wait( this, uniform_gen( 30, 60 ) );
						this->next = 1;
					}
				}
			}
			else
			{
				Pocitace->Enter( this, 1 );
				this->next = 2;
			}
			break;
		
		case 2:
			dobaUceni  = exponential_gen( 100 );
			dobaStudiu->add_value( dobaUceni );
			Calendar::instance->Wait( this, dobaUceni );
			this->next = 3;
			break;
		
		case 3:
			Pocitace->Leave( this, 1 );
			break;
	}
}

void Calendar::generateEvents( )
{
	init_rand_gen();
	int actTime = 0;
	int priority = 0;
	// STUDENTI
	for ( int i = 0; i != 84796; i++ )
	{
		double r = exponential_gen( 10 );
		actTime += r;
		Event * e = new Event( actTime, priority );
		this->addEvent( e );
		delete e;
	}
}

int main()
{
	string name = create_file( "ucebna.txt" );
	// Inicializacia kalendaru 
	Calendar * cal = Calendar::Instance( 0, 1000000 );
	// Vygenerovanie udalosti
	cal->generateEvents();
	// Spustenie simulacie
	cal->Run();
	
	// Vypis statistik
	data_out_write(name, Pocitace->Statistics());
	dobaStudiu->data_out(name);
	return EXIT_SUCCESS;
}
