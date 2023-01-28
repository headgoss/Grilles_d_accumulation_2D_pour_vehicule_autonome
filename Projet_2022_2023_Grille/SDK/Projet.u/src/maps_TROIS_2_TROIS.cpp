////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_TROIS_2_TROIS.h"	// Includes the header of this component
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <array>
using namespace std;

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSTROIS_2_TROIS)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("input", MAPS::FilterFloat64, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSTROIS_2_TROIS)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT("output", MAPS::Float64, NULL, NULL, 0)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSTROIS_2_TROIS)
    //MAPS_PROPERTY("pName",128,false,false)
	//Données LIDAR
	MAPS_PROPERTY("Dimensions_m", 0.01, false, true)
	MAPS_PROPERTY_ENUM("LIDAR", "Lidar_gauche|Lidar_droite", 0, false, true)
	//Référence sur la hauteur 
	MAPS_PROPERTY("z_min", -0.1, false, true)
	MAPS_PROPERTY("z_max", 0.5, false, true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSTROIS_2_TROIS)
    //MAPS_ACTION("aName",MAPSTROIS_2_TROIS::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (TROIS_2_TROIS) behaviour
MAPS_COMPONENT_DEFINITION(MAPSTROIS_2_TROIS,"TROIS_2_TROIS","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSTROIS_2_TROIS::Birth()
{
	m_firstTime = true;
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSTROIS_2_TROIS::Core() 
{
	MAPSInt64 current_index1 = GetIntegerProperty("LIDAR");
	MAPSIOElt* ioEltIn = StartReading(Input("input"));
	if (ioEltIn == NULL)
		return;


	//Bound the vector size to 32 if the input vector is larger that 32 elements.
	int vectorsizeout = ioEltIn->VectorSize();

	//Un vecteur pour lire l'entrée qui sera répartie sur x,y et z
	vector<float> data;
	vector<float> x;
	vector<float> y;
	vector<float> z;

	float dim = (float)GetFloatProperty("Dimensions_m");

	for (int i = 0; i < vectorsizeout; i = i + 1) {
		data.push_back(ioEltIn->Float64(i)*dim);
		//On prend toutes les données directements
	}

	//On répartie sur les vecteurs propres x,y et z

	for (int i = 0; i < vectorsizeout; i = i + 3) {

		if (current_index1 == 0) {
			y.push_back(data.at(i));
			x.push_back(data.at(i + 1));
			z.push_back(-data.at(i + 2));
		}
		else {
			y.push_back(-data.at(i));
			x.push_back(-data.at(i + 1));
			z.push_back(-data.at(i + 2));
		}
	}

	int size = z.size();
	int ii = 0;
	vector<float> xx;
	vector<float> yy;
	vector<float> zz;
	float z_min = (float)GetFloatProperty("z_min");
	float z_max = (float)GetFloatProperty("z_max");

	xx.reserve(size);
	yy.reserve(size);
	zz.reserve(size);
	auto it_x = x.begin(), it_y = y.begin(), it_z = z.begin();
	while (it_z != z.end()) {
		if (*it_z > z_min & *it_z < z_max) {
			xx.push_back(*it_x);
			yy.push_back(*it_y);
			zz.push_back(*it_z);
		}
		++it_x;
		++it_y;
		++it_z;
	}

	int taille = xx.size();

	if (m_firstTime) {
		m_firstTime = false; 
		Output(0).AllocOutputBuffer(ioEltIn->BufferSize());
	}

	//Now the output buffers have been allocated, we can request an element to write in.
	MAPSIOElt* ioEltOut = StartWriting(Output("output"));
	vector<float>newdata;
	int vectorsize = ioEltIn->VectorSize();
	for (int i = 0; i < taille; i++) {
		newdata.push_back(xx.at(i));
		newdata.push_back(yy.at(i));
		newdata.push_back(zz.at(i));
	}
	for (int i = 0; i < newdata.size(); i++) {
		ioEltOut->Float64(i) = newdata.at(i);
	}
	ioEltOut->VectorSize() = vectorsize;
	ioEltOut->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut);

}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSTROIS_2_TROIS::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}
