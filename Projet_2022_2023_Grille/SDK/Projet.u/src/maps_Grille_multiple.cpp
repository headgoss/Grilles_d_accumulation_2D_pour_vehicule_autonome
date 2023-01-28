////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Grille_multiple.h"	// Includes the header of this component
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <array>

using namespace std;

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSGrille_multiple)
MAPS_INPUT("input", MAPS::FilterFloat64, MAPS::FifoReader)
MAPS_INPUT("col_size", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_INPUT("case_size", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_INPUT("x_in", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_INPUT("y_in", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_INPUT("facteur", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_INPUT("vitesse_in", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSGrille_multiple)
//On sort une image pour l'affichage
MAPS_OUTPUT("output", MAPS::IplImage, NULL, NULL, 0)
MAPS_OUTPUT("vit", MAPS::Integer32, NULL, NULL, 1)
MAPS_OUTPUT("alpha", MAPS::Float32, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSGrille_multiple)
//Couleur de l'image
MAPS_PROPERTY("pColorModel", "RGB", false, false)
//Référence sur la hauteur 
MAPS_PROPERTY("z_min", -0.1, false, true)
MAPS_PROPERTY("z_max", 0.5, false, true)
//Vitesse max choisi
MAPS_PROPERTY("vit_ref", 20, false, true)
//Distance à l'arrêt max que l'on chosit
MAPS_PROPERTY("distance_arret", 5, false, true)
//Valeur de détection d'un obstacle
MAPS_PROPERTY("threashold25", 3, false, true)
MAPS_PROPERTY("threashold5", 6, false, true)
MAPS_PROPERTY("threashold1", 9, false, true)
//Distance sélectionné selon les vitesses
MAPS_PROPERTY("distance1", 80, false, false)
MAPS_PROPERTY("distance5", 60, false, false)
MAPS_PROPERTY("distance25", 40, false, false)
//Valeur utilisé pour l'importance de l'angle
MAPS_PROPERTY("val", 4.0, false, true)
//Distance entre le devant du véhicule et le lidar approximative
MAPS_PROPERTY("lidar_voiture", 2.0, false, true)
//Où que l'on souhaite commencer notre affichage
MAPS_PROPERTY("x_start", 0, false, true)
//Valeur de la fonction linéaire
MAPS_PROPERTY("k_max",10.0,false,true)
MAPS_PROPERTY("k_min", 1.0, false, true)
MAPS_PROPERTY("angle_max", 90.0, false, true)
MAPS_PROPERTY_ENUM("critere_de_vitesse", "produit|fonction_lineaire", 0, false, true)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSGrille_multiple)
//MAPS_ACTION("aName",MAPSGrille::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Grille) behaviour
MAPS_COMPONENT_DEFINITION(MAPSGrille_multiple, "Grille_multiple", "1.0", 128,
	MAPS::Threaded, MAPS::Threaded,
	-1, // Nb of inputs. Leave -1 to use the number of declared input definitions
	-1, // Nb of outputs. Leave -1 to use the number of declared output definitions
	-1, // Nb of properties. Leave -1 to use the number of declared property definitions
	-1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSGrille_multiple::Birth()
{
	// Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
	ReportInfo("Passing through Birth() method");
	m_firstTime = true;
	vitesse_out = 0;
	//Envoie de la première vitesse modifié soit 0
	MAPSIOElt* ioEltOut = StartWriting(Output("vit"));
	ioEltOut->Integer32() = vitesse_out;
	ioEltOut->Timestamp() = 0;
	StopWriting(ioEltOut);

	//IplImage model;
	// Get a model of image according to properties specifications
	//model = MAPS::IplImageModel(1600, 120, GetStringProperty("pColorModel"));
	// Allocate output image according to the model
	//Output("output").AllocOutputBufferIplImage(model);
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
void MAPSGrille_multiple::Core()
{
	MAPSInt64 current_index = GetIntegerProperty("critere_de_vitesse");
	//Request data on the input called "input".
	//This function will block until a new data is available on the input.
	//The MAPSIOElt class is the generic object that contains a piece of data (numbers, packets of bytes, images, etc...)	
	vector<MAPSIOElt*> inputs = {
	StartReading(Input("input")),
	StartReading(Input("col_size")),
	StartReading(Input("case_size")),
	StartReading(Input("x_in")),
	StartReading(Input("y_in")),
	StartReading(Input("vitesse_in")),
	StartReading(Input("facteur"))
	};

	if (std::any_of(inputs.begin(), inputs.end(), [](MAPSIOElt * elt) { return elt == NULL; })) {
		return;
	}

	int col_size = inputs[1]->Integer32();
	int case_size = inputs[2]->Integer32();
	int x_in = inputs[3]->Integer32();
	int y_in = inputs[4]->Integer32();
	int vitesse_in = inputs[5]->Integer32();
	int facteur = inputs[6]->Integer32();



	//Bound the vector size to 32 if the input vector is larger that 32 elements.
	int vectorsizeout = inputs[0]->VectorSize();

	//Un vecteur pour lire l'entrée qui sera répartie sur x,y et z
	vector<float> data;
	vector<float> x;
	vector<float> y;
	vector<float> z;

	
	//For each element in the input MAPSIOElt (up to 32 if input vector is larger), compute the result
	//and write it in the output MAPSIOElt.
	//#pragma omp parallel for //pas sur de OMP ou non
	for (int i = 0; i < vectorsizeout; i = i + 1) {
		data.push_back(inputs[0]->Float64(i));
		//On prend toutes les données directements
	}

	//On répartie sur les vecteurs propres x,y et z

	for (int i = 0; i < vectorsizeout; i = i + 3) {
		x.push_back(data.at(i));
		y.push_back(data.at(i+1));
		z.push_back(data.at(i+2));
	}
	/*
	for (int i = 1; i < vectorsizeout; i = i + 3) {
		y.push_back(data.at(i));
	}

	for (int i = 2; i < vectorsizeout; i = i + 3) {
		z.push_back(data.at(i));
	}*/

	int size = z.size();
	int ii = 0;
	vector<float> xx;
	vector<float> yy;
	float z_min = (float)GetFloatProperty("z_min");
	float z_max = (float)GetFloatProperty("z_max");

	/*
	while (ii < size)
	{
		//On garde que les x et y au niveaux des z dont nous avons besoin
		if (z.at(ii) > z_min && z.at(ii) < z_max)
		{
			xx.push_back(x.at(ii));
			yy.push_back(y.at(ii));
		}
		ii = ii + 1;
	}
	
	for (auto it = z.begin(); it != z.end(); ++it) {
		//On garde que les x et y au niveaux des z dont nous avons besoin
		if (*it > z_min && *it < z_max) {
			xx.push_back(x.at(it - z.begin()));
			yy.push_back(y.at(it - z.begin()));
		}
	}*/
	xx.reserve(size);
	yy.reserve(size);
	auto it_x = x.begin(), it_y = y.begin(), it_z = z.begin();
	while (it_z != z.end()) {
		if (*it_z > z_min & *it_z < z_max) {
			xx.push_back(*it_x);
			yy.push_back(*it_y);
		}
		++it_x;
		++it_y;
		++it_z;
	}

	int taille = xx.size();
	/*
	vector<float> total;

	for (int i = 0; i < taille*2; i=i+2) {
		total.push_back(xx.at(i / 2));
		total.push_back(yy.at(i / 2));
	}
	*/
	/*
	MAPSIOElt* ioEltIn1 = StartReading(Input("col_size"));
	if (ioEltIn1 == NULL)
		return;
	int col_size = ioEltIn1->Integer32();

	MAPSIOElt * ioEltIn2 = StartReading(Input("case_size"));
	if (ioEltIn2 == NULL)
		return;
	int case_size = ioEltIn2->Integer32();

	MAPSIOElt * ioEltIn3 = StartReading(Input("x_in"));
	if (ioEltIn3 == NULL)
		return;
	int x_in = ioEltIn3->Integer32();

	MAPSIOElt * ioEltIn4 = StartReading(Input("y_in"));
	if (ioEltIn4 == NULL)
		return;
	int y_in = ioEltIn4->Integer32();

	MAPSIOElt * ioEltIn5 = StartReading(Input("vitesse_in"));
	int vitesse_in;
	if (ioEltIn5 == NULL)
		return;
	vitesse_in = ioEltIn5->Integer32();

	MAPSIOElt* ioEltIn6 = StartReading(Input("facteur"));
	int facteur;
	if (ioEltIn6 == NULL)
		return;
	facteur = ioEltIn6->Integer32();*/

	

	const int vitesse_ref = (int)GetIntegerProperty("vit_ref");
	//vitesse à soustraire
	vitesse_out = 0;
	//angle en ° de la position de l'objet par rapport à la position
	float a;
	float val = (float)GetFloatProperty("val");
	float k_max = (float)GetFloatProperty("k_max");
	float k_min = (float)GetFloatProperty("k_min");
	float angle_max = (float)GetFloatProperty("angle_max");
	int distance_arret = (int)GetIntegerProperty("distance_arret") + (int)GetFloatProperty("lidar_voiture");

	//First get a MAPSIOElt object in which we can write the result to be output.
	//The StartWriting function is non-blocking.
	MAPSIOElt * ioElt = StartWriting(Output("output"));

	//Allocation de la sortie de l'image pour la taille défini au démarage
	if (m_firstTime == true) {
		IplImage model;
		// Get a model of image according to properties specifications
		int tx = x_in * facteur;
		int ty = y_in * 2 * facteur; 
		//MAPSStreamedString sx;
		//sx << "facteur : " << facteur << " tx: " << tx << " ty: " << ty;
		//ReportInfo(sx);
		//sx.Clear();
		model = MAPS::IplImageModel(tx, ty, GetStringProperty("pColorModel"));
		// Allocate output image according to the model
		Output("output").AllocOutputBufferIplImage(model);
		m_firstTime = false;
	}

	// Get the image structure encapsulated into the MAPSIOElt
	IplImage& iplimage = ioElt->IplImage();

	// The image structure contains a pointer to the image bytes (pixels) : imageData
	unsigned char* imageData = (unsigned char*)iplimage.imageData;

	int taille_ligne = iplimage.width;
	int taille_colonne = iplimage.height;
	//MAPSStreamedString sx;
	
	//sx << "taille_ligne : " << taille_ligne << " taille_colonne: " << taille_colonne ;
	//ReportInfo(sx);
	//sx.Clear();

	//Une vitesse donné de base qui par la suite peut être prise du véhicule
	//float vitesse = 10; //km / h
	
	//Distance de freinage pour zone de sécurité
	int dfreinage;
	if (vitesse_in < 10)
		dfreinage = (int)GetFloatProperty("lidar_voiture") + 4;
	else
		dfreinage = (int)GetFloatProperty("lidar_voiture") + (vitesse_in / 10) * 6;
	//Distance d'arret
	int darret;
	if (vitesse_in < 10)
		darret = distance_arret + 4 + vitesse_in * 2000 / 3600;
	else
		darret = distance_arret + (vitesse_in / 10) * (vitesse_in / 10) + vitesse_in*2000/3600;


	//Boucle for variable
	int sortir = 0;
	int stop = 0;
	int res = 0;
	int pos = 0;
	int position = 0;

	float y_;
	float x_;
	int distance;
	int threashold;

	if (vitesse_in < 10) {
		x_ = 0.25;
		y_ = 0.25;
		distance = (int)GetIntegerProperty("distance25");
		threashold = (int)GetIntegerProperty("threashold25");
	}

	else if (vitesse_in >= 10 & vitesse_in < 20)
	{
		x_ = 0.5;
		y_ = 0.5;
		distance = (int)GetIntegerProperty("distance5");
		threashold = (int)GetIntegerProperty("threashold5");
	}

	else {
		x_ = 1;
		y_ = 1;
		distance = (int)GetIntegerProperty("distance1");
		threashold = (int)GetIntegerProperty("threashold1");
	}


	if (distance > x_in)
	{
		distance = x_in;
	}
	int cpt = 0;

	/* Code pour vérification sur une ligne
	for (float y = 3-y_ ; y >= -3; y=y-y_) {

		position = taille_ligne * case_size * pos;
		cpt = 0;

		for (float x = 0; x < distance; x= x + x_) {

			int col_size_start = col_size*cpt;
			int col_size_end = col_size_start  + col_size;
			res = 0;

			for (int i = 0; i < taille; i = i + 1) {
				if (x<=xx.at(i) & x+1>=xx.at(i) & y<=yy.at(i) & y+1>=yy.at(i)) {
					res += 1;
				}
			}


			if (res >= 6 && x < dfreinage)
			{
				//sx <<"Freinage d'urgence";
				for (int posy = 0; posy < case_size ; posy++) {
					for (int posx = col_size_start; posx < col_size_end; posx++) {
						imageData[(posx + posy*taille_ligne + position)*3] = (unsigned char)(255);
						imageData[(posx + posy * taille_ligne + position) * 3+1] = (unsigned char)(0);
						imageData[(posx + posy * taille_ligne + position) * 3+2] = (unsigned char)(0);
					}
				}
			}
			else if (res >= 6 && x < dfreinage * 3)
			{
				for (int posy = 0; posy < case_size ; posy++) {
					for (int posx = col_size_start; posx < col_size_end; posx++) {
						imageData[(posx + posy * taille_ligne+ position)*3] = (unsigned char)(255);
						imageData[(posx + posy * taille_ligne + position)*3+1] = (unsigned char)(128);
						imageData[(posx + posy * taille_ligne + position)*3+2] = (unsigned char)(0);
					}
				}
			}
			else if (res >= 6 && x > dfreinage * 3)
			{
				//sx << "changement de trajectoire";
				for (int posy = 0; posy < case_size; posy++) {
					for (int posx = col_size_start; posx < col_size_end; posx++) {
						imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(255);
						imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(255);
						imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
					}
				}
			}
			else {
				for (int posy = 0; posy < case_size ; posy++) {
					for (int posx = col_size_start; posx < col_size_end; posx++) {
						imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(0);
						imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(255);
						imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
					}
				}
			}
		cpt += 1;
		}
	pos+= 1;
	}
	*/

	double pi = 2 * acos(0.0);
	float tempy = 3;
	float alpha = -1.0;
	float temp = 0;

	int x_start = (int)GetIntegerProperty("x_start");
	map<std::pair<int, int>, std::array<unsigned char, 3>> couleurs;
	MAPSStreamedString sx;
	for (float x = x_start; x < distance + x_start; x = x + x_) {
		pos = 0;

		if (x >= darret)
			stop = 1;

		if (sortir == 1 & stop==1)
			tempy = -100;

		for (float y = y_in - y_; y >= -y_in; y = y - y_) {
			if (tempy != -100)
				tempy = y;

			position = taille_ligne * case_size * pos;
			res = 0;
			int col_size_start = col_size * cpt;
			int col_size_end = col_size_start + col_size;

			if (sortir == 0 | tempy != -100) {
				for (int i = 0; i < taille; i = i + 1) {
					if (x <= xx.at(i) & x + x_ >= xx.at(i) & y <= yy.at(i) & y + y_ >= yy.at(i)) {
						res ++;
					}
				}

				if (res >= threashold & x < dfreinage)
				{
					sortir = 1;
					//sx <<"Freinage d'urgence";
					for (int posy = 0; posy < case_size; posy++) {
						for (int posx = col_size_start; posx < col_size_end; posx++) {
							imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(255);
							imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(0);
							imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
						}
					}
					if (x == 0)
						a = atan(y / 0.0001) * (180 / pi);
					else
						a = atan(y / x) * (180 / pi);

					if (a < 0)
						a = -a;
					if (current_index == 0)
					{
						temp = (vitesse_ref * distance_arret) / ((a / val) + x);
					}
					else if (current_index == 1)
					{
						float f_angle = k_max - k_min * a / angle_max; //Fonction linéaire importance position
						temp = abs(f_angle * (x - darret) / (dfreinage - distance_arret));
					}
					if (int(temp) > vitesse_out) {
						vitesse_out = int(temp);
						alpha = a;
					}
				}

				else if (res >= threashold & x < dfreinage * 3)
				{
					sortir = 1;
					for (int posy = 0; posy < case_size; posy++) {
						for (int posx = col_size_start; posx < col_size_end; posx++) {
							imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(255);
							imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(128);
							imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
						}
					}
					a = atan(y / x) * (180 / pi);
					if (a < 0)
						a = -a;
					if (current_index == 0)
					{
						temp = (vitesse_ref * distance_arret) / ((a / val) + x);
					}
					else if (current_index == 1)
					{
						float f_angle = k_max - k_min * a / angle_max; //Fonction linéaire importance position
						temp = f_angle * (x - darret) / (dfreinage - distance_arret);
					}
					if (int(temp) > vitesse_out) {
						vitesse_out = int(temp);
						alpha = a;
					}
				}
				else if (res >= threashold & x > dfreinage * 3)
				{
					sortir = 1;
					//sx << "changement de trajectoire";
					for (int posy = 0; posy < case_size; posy++) {
						for (int posx = col_size_start; posx < col_size_end; posx++) {
							imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(255);
							imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(255);
							imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
						}
					}
					a = atan(y / x) * (180 / pi);
					if (a < 0)
						a = -a;
					if (current_index == 0)
					{
						temp = (vitesse_ref * distance_arret) / ((a / val) + x);
					}
					else if (current_index == 1)
					{
						float f_angle = k_max - k_min * a / angle_max; //Fonction linéaire importance position
						temp = f_angle * (x - darret) / (dfreinage - distance_arret);
						if (temp < 0)
							temp = vitesse_ref;
					}
					if (int(temp) > vitesse_out) {
						vitesse_out = int(temp);
						alpha = a;
					}
				}

				else {
					for (int posy = 0; posy < case_size; posy++) {
						for (int posx = col_size_start; posx < col_size_end; posx++) {
							imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(0);
							imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(255);
							imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
						}
					}
				}
			}
			else {
				for (int posy = 0; posy < case_size; posy++) {
					for (int posx = col_size_start; posx < col_size_end; posx++) {
						imageData[(posx + posy * taille_ligne + position) * 3] = (unsigned char)(0);
						imageData[(posx + posy * taille_ligne + position) * 3 + 1] = (unsigned char)(0);
						imageData[(posx + posy * taille_ligne + position) * 3 + 2] = (unsigned char)(0);
					}
				}
			}
			pos += 1;
		}
		cpt += 1;
	}


	ioElt->Timestamp() = inputs[0]->Timestamp();
	// The timeOfIssue field is not changed and will be set in the StopWriting to MAPS::CurrentTime()
	// Send the output data
	StopWriting(ioElt);

	if (vitesse_out >= vitesse_ref)
		vitesse_out = vitesse_ref;

	MAPSIOElt * ioEltOut = StartWriting(Output("vit"));
	ioEltOut->Integer32() = vitesse_out;
	ioEltOut->Timestamp() = inputs[0]->Timestamp();
	StopWriting(ioEltOut);

	MAPSIOElt* ioEltOut1 = StartWriting(Output("alpha"));
	ioEltOut1->Float32() = alpha;
	ioEltOut1->Timestamp() = inputs[0]->Timestamp();
	StopWriting(ioEltOut1);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSGrille_multiple::Death()
{
	// Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
	ReportInfo("Passing through Death() method");
}
