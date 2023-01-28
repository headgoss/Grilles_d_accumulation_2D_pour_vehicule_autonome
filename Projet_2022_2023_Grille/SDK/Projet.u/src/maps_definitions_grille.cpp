////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_definitions_grille.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSdefinitions_grille)
    MAPS_INPUT("vitesse",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSdefinitions_grille)
    MAPS_OUTPUT("col_size",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT("case_size", MAPS::Integer32, NULL, NULL, 1)
	MAPS_OUTPUT("x_out", MAPS::Integer32, NULL, NULL, 1)
	MAPS_OUTPUT("y_out", MAPS::Integer32, NULL, NULL, 1)
	MAPS_OUTPUT("facteur_out", MAPS::Integer32, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSdefinitions_grille)
    MAPS_PROPERTY("x",80,false,false)
	MAPS_PROPERTY("y", 3, false, false)
	MAPS_PROPERTY("facteur", 20, false, false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSdefinitions_grille)
    //MAPS_ACTION("aName",MAPSdefinitions_grille::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (definitions_grille) behaviour
MAPS_COMPONENT_DEFINITION(MAPSdefinitions_grille,"definitions_grille","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSdefinitions_grille::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
	m_nextTimestamp = MAPS::CurrentTime();
	MAPSIOElt* ioEltOut2 = StartWriting(Output("x_out"));
	ioEltOut2->Integer32() = (int)GetIntegerProperty("x");
	ioEltOut2->Timestamp() = 0;
	StopWriting(ioEltOut2);

	MAPSIOElt* ioEltOut3 = StartWriting(Output("y_out"));
	ioEltOut3->Integer32() = (int)GetIntegerProperty("y");
	ioEltOut3->Timestamp() = 0;
	StopWriting(ioEltOut3);

	MAPSIOElt* ioEltOut4 = StartWriting(Output("facteur_out"));
	ioEltOut4->Integer32() = (int)GetIntegerProperty("facteur");
	ioEltOut4->Timestamp() = 0;
	StopWriting(ioEltOut4);
}

//ATTENTION: 
//	Make sure there is ONE and ONLY ONE blocking function inside this Core method.
//	Consider that Core() will be called inside an infinite loop while the diagram is executing.
//	Something similar to: 
//		while (componentIsRunning) {Core();}
//
//	Usually, the one and only blocking function is one of the following:
//		* StartReading(MAPSInput& input); //Data request on a single BLOCKING input. A "blocking input" is an input declared as FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader (declaration happens in MAPS_INPUT: see the beginning of this file). A SamplingReader input is non-blocking: StartReading will not block with a SamplingReader input.
//		* StartReading(int nCount, MAPSInput* inputs[], int* inputThatAnswered, int nCountEvents = 0, MAPSEvent* events[] = NULL); //Data request on several BLOCKING inputs.
//		* SynchroStartReading(int nb, MAPSInput** inputs, MAPSIOElt** IOElts, MAPSInt64 synchroTolerance = 0, MAPSEvent* abortEvent = NULL); // Synchronized reading - waiting for samples with same or nearly same timestamps on several BLOCKING inputs.
//		* Wait(MAPSTimestamp t); or Rest(MAPSDelay d); or MAPS::Sleep(MAPSDelay d); //Pauses the current thread for some time. Can be used for instance in conjunction with StartReading on a SamplingReader input (in which case StartReading is not blocking).
//		* Any blocking grabbing function or other data reception function from another API (device driver,etc.). In such case, make sure this function cannot block forever otherwise it could freeze RTMaps when shutting down diagram.
//**************************************************************************/
//	In case of no blocking function inside the Core, your component will consume 100% of a CPU.
//  Remember that the StartReading function used with an input declared as a SamplingReader is not blocking.
//	In case of two or more blocking functions inside the Core, this is likely to induce synchronization issues and data loss. (Ex: don't call two successive StartReading on FifoReader inputs.)
/***************************************************************************/
void MAPSdefinitions_grille::Core() 
{
	//Wait(m_nextTimestamp);
	MAPSIOElt* ioEltIn = StartReading(Input("vitesse"));
	int vitesse;
	if (ioEltIn == NULL)
		return;
	vitesse = ioEltIn->Integer32();
	
	int taille_ligne = (int)GetIntegerProperty("x")* (int)GetIntegerProperty("facteur");
	int taille_colonne = (int)GetIntegerProperty("y")*2* (int)GetIntegerProperty("facteur");

	int col_div = (int)GetIntegerProperty("x");
	int	case_div = (int)GetIntegerProperty("y")*2;
	int Width;
	int Height;

	if (vitesse < 10) {
		col_div = col_div*4;
		case_div = case_div*4;
	}
	else if (vitesse >= 10 & vitesse < 20)
	{
		col_div = col_div * 2;
		case_div = case_div * 2;
	}
	else {
		col_div = col_div;
		case_div = case_div;
	}
	int col_size = (int)(taille_ligne / col_div);
	int case_size = (int)(taille_colonne / case_div);


	//MAPSStreamedString sx;
	//sx << "taille_ligne : " << taille_ligne << " taille_colonne: " << taille_colonne << " col_size: " << col_size << " case_size: " << case_size;
	//ReportInfo(sx);
	//sx.Clear();

	MAPSIOElt* ioEltOut = StartWriting(Output("col_size"));
	ioEltOut->Integer32() = col_size;
	ioEltOut->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut);
	MAPSIOElt* ioEltOut1 = StartWriting(Output("case_size"));
	ioEltOut1->Integer32() = case_size;
	ioEltOut1->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut1);

	MAPSIOElt* ioEltOut2 = StartWriting(Output("x_out"));
	ioEltOut2->Integer32() = (int)GetIntegerProperty("x");
	ioEltOut2->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut2);
	MAPSIOElt* ioEltOut3 = StartWriting(Output("y_out"));
	ioEltOut3->Integer32() = (int)GetIntegerProperty("y");
	ioEltOut3->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut3);
	MAPSIOElt* ioEltOut4 = StartWriting(Output("facteur_out"));
	ioEltOut4->Integer32() = (int)GetIntegerProperty("facteur");
	ioEltOut4->Timestamp() = ioEltIn->Timestamp();
	StopWriting(ioEltOut4);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSdefinitions_grille::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}
