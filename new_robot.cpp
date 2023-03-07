/* File: base_roulante.cpp
 * Author: Philippe Latu
 * Source: https://github.com/platu/libsensehat-cpp
 *
 * Patron du code source des séances de travaux pratiques
 */

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sensehat.h>

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // system_clock, seconds, milliseconds

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Début constantes
const unsigned int MDROIT = 1;
const unsigned int MGAUCHE = 0;
const unsigned int PERIOD = 10000;
const unsigned int CAPTEUR_G = 16;
const unsigned int CAPTEUR_D = 17;
// Fin constantes
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Début sous-programmes

bool initialiser()
{	
	pwmInit(0);
	pwmInit(1);
	pwmPeriod(0, 10000);
	pwmPeriod(1, 10000);
	pwmEnable(0);
	pwmEnable(1);
	gpioSetConfig(16,in);
	gpioSetConfig(17,in);
	return(true);
}

void tournerMoteur(unsigned int chan,unsigned int vitesse)
{
	pwmDutyCycle(chan,vitesse);
}
int detectionCapteur(unsigned int pin)
{
	return(gpioGetInput(pin));
}

void avancerDroit()
{
	tournerMoteur(0,80);
	tournerMoteur(1,80);
}
void tournerDroite()
{
	tournerMoteur(0,100);
	tournerMoteur(1,40);
}
void tournerGauche()
{
	tournerMoteur(0,40);
	tournerMoteur(1,100);
}
void tournerDroitePlus()
{
	tournerMoteur(0,100);
	tournerMoteur(1,15);
}
void tournerGauchePlus()
{
	tournerMoteur(0,15);
	tournerMoteur(1,100);
}

void demiTour()
{
	int memoCapteur = 'N';
	int capteurD, capteurG;
	cout << "demi-tour \n";
	tournerGauchePlus();
	sleep_for(milliseconds(3000));
	do
	{
		capteurD = detectionCapteur(17);
		capteurG = detectionCapteur(16);
		if (capteurD == 1 && capteurG == 1)
		{	
			avancerDroit();
		} 
		if (capteurG == 0)
		{
			tournerDroite();
		}
		if (capteurG == 0 && capteurD == 0)
		{

			memoCapteur = 'O';
		}
		sleep_for(milliseconds(20));
	}while(memoCapteur == 'N');
	//cout << "Position initial : " << pos << endl;
	//cout << "Position finale : " << posNeed << endl;
}

void arret()
{
	pwmDutyCycle(0,0);
	pwmDutyCycle(1,0);
}

void suivreLigne()
{
	int capteurG;
	int capteurD;
	char memoCapteur;
	int cpt;
	cpt = 0;
	memoCapteur = ' ';
	while(true)
	{
		capteurD = detectionCapteur(17);
		capteurG = detectionCapteur(16);
		//cout << "DetectionCapteurGauche : " << capteurG << endl;
		//cout << "DetectionCapteurDroit : " << capteurD << endl;

		if(capteurG == 0 && capteurD == 0)
		{
			if(memoCapteur == 'G')
			{
				tournerGauche();
				sleep_for(milliseconds(300));
			}
			if(memoCapteur == 'D')
			{
				tournerDroite();
				sleep_for(milliseconds(300));
			}
			memoCapteur = 'M';
			avancerDroit();
		}
		else if(capteurG == 0 && capteurD == 1)
		{	
			memoCapteur= 'D';
			tournerGauche();
		}
		else if(capteurG == 1 && capteurD == 0)
		{	
			memoCapteur = 'G';
			tournerDroite();
		}
		
		else if(capteurG == 1 && capteurD == 1)
		{
			if(cpt == 1)
			{
				arret();
				cpt = 0;
			}
			else if (memoCapteur == 'G')
			{
				tournerDroitePlus();
			}
			else if (memoCapteur == 'D')
			{
				tournerGauchePlus();
			}
			else
			{
				cpt += 1;
				demiTour();
			}
		}
		sleep_for(milliseconds(70));
	}
}

// Fin sous-programmes
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main() {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Début variables
	//int cpt;
	// Fin variables
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if(senseInit()) {
		cout << "Sense Hat initialization Ok." << endl;

		if(initialiser())
		{
			pwmDutyCycle(0,0);
			pwmDutyCycle(1,0);
			suivreLigne();
		}

		
		// Fin instructions
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		cout << "Press joystick button to quit." << endl;
		senseWaitForJoystickEnter();

		senseShutdown();
		cout << "Sense Hat shut down." << endl;
	}
	return EXIT_SUCCESS;
}
