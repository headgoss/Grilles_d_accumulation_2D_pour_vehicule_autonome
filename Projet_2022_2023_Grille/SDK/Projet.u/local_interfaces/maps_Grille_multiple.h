////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Grille_multiple_H
#define _Maps_Grille_multiple_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSGrille_multiple : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGrille_multiple)
private :
	// Place here your specific methods and attributes
	bool m_firstTime;
	int vitesse_out;
};

#endif
