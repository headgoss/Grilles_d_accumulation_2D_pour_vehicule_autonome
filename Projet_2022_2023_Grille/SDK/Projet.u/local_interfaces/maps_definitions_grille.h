////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_definitions_grille_H
#define _Maps_definitions_grille_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSdefinitions_grille : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSdefinitions_grille)
private :
	// Place here your specific methods and attributes
	MAPSTimestamp m_nextTimestamp;
};

#endif
