////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Grille_H
#define _Maps_Grille_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSGrille : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGrille)
private :
	// Place here your specific methods and attributes
	// This member variable will be used for allocating once the output buffers at first time.
	bool m_firstTime;
	int vitesse_out;
};

#endif
