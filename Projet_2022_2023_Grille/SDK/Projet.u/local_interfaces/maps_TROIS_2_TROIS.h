////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_TROIS_2_TROIS_H
#define _Maps_TROIS_2_TROIS_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSTROIS_2_TROIS : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSTROIS_2_TROIS)
private :
	// Place here your specific methods and attributes
	bool m_firstTime;
};

#endif
