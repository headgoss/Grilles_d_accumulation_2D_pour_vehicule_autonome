////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_add.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSadd)
	MAPS_INPUT("vitesse_mod", MAPS::FilterInteger32, MAPS::FifoReader)
	MAPS_INPUT("vitesse_actuelle", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSadd)
	MAPS_OUTPUT("output", MAPS::Integer32, NULL, NULL, 1)
	MAPS_OUTPUT("freinage", MAPS::Integer32, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSadd)
    MAPS_PROPERTY("vit_ref",20,false,true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSadd)
    //MAPS_ACTION("aName",MAPSadd::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (add) behaviour
MAPS_COMPONENT_DEFINITION(MAPSadd,"add","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSadd::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSadd::Core() 
{

	MAPSIOElt* ioEltIn = StartReading(Input("vitesse_mod"));
	int vitesse_mod = 0;
	if (ioEltIn == NULL)
		vitesse_mod = 0;
		//return;
	else
		vitesse_mod = ioEltIn->Integer32();

	MAPSIOElt* ioEltIn1 = StartReading(Input("vitesse_actuelle"));
	int vitesse_actuelle = 0;
	if (ioEltIn1 == NULL)
		vitesse_actuelle = 0;
	//return;
	else
		vitesse_actuelle = ioEltIn1->Integer32();

	int vit_ref = (int)GetIntegerProperty("vit_ref");
	int vitesse = vit_ref - vitesse_mod;
	
	MAPSIOElt* ioEltOut = StartWriting(Output("output"));
	ioEltOut->Integer32() = vitesse;
	ioEltOut->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut);

	MAPSIOElt* ioEltOut1 = StartWriting(Output("freinage"));
	if (vitesse >= vitesse_actuelle)
		ioEltOut1->Integer32() = 0;
	else
		ioEltOut1->Integer32() = -18000;
	ioEltOut1->Timestamp() = ioEltIn1->Timestamp();
	StopWriting(ioEltOut1);	
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSadd::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}
