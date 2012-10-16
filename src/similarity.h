#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "bdd.h"

using namespace std;
using namespace cv;
using namespace cv::flann;

/*class BDD
{
private:
	Mat audioFeatures;
public :
	Mat getAudioFeatures()
	{
		return audioFeatures;
	}

	BDD(int size1, int size2)
	{
		audioFeatures.create(size1, size2, CV_32F);
		MatIterator_<float> it = audioFeatures.begin<float>(), it_end = audioFeatures.end<float>();
		for ( ; it != it_end; it++)
		{
			*it=(float)rand();
		}
	}

	void BDD::affiche()
	{
		int count=0;
		MatIterator_<float> it = audioFeatures.begin<float>(), it_end = audioFeatures.end<float>();
		printf("bdd matrice features :\n");
		for ( ; it != it_end; it++)
		{
			printf("%f ",*it);
			count++;
			if (count == audioFeatures.size().width)
			{
				printf("\n");
				count = 0;
			}
		}
		printf("\n\n");
	}
} ;
*/


class Similarity 
{
private:
	Bdd *songDB;
	int audioFeaturesSize; // Nombre de valeur issu du modèle gaussien des MFCC (230 pour 20 coefs)
	int nbSongs; // Nombre de chanson dans la base de donnée
	int nbKNN; // Nombre de plus proches voisins pour la requète
	
	// Matrice des audio features de taille nbSongs x audioFeaturesSize
	Mat features;

	// Paramètres pour la recherche
	cv::flann::SearchParams param;
	cv::flann::KDTreeIndexParams KDParams;
	cv::flann::Index *index;

	//char* indexFileName;


public:
	Similarity();
	Similarity(Bdd *db, int nb_knn = 10);
	~Similarity();
	bool buildIndex();
	int audio_knn(int song_index, Mat indice, Mat distance);




} ;

#endif //SIMILARITY_H
