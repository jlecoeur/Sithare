#include "similarity.h"

using namespace std;
using namespace cv;
using namespace cv::flann;


Similarity::Similarity()
{

}


Similarity::Similarity(Bdd *db, int nb_knn)
{
	songDB = db;
	nbKNN = nb_knn;
	
}

bool Similarity::buildIndex()
{
	features = songDB->getAudioFeatures();
	Size size_mat_features = features.size();
	audioFeaturesSize = size_mat_features.width;
	nbSongs = size_mat_features.height;

	// Création de l'index pour la recherche
	index = new cv::flann::Index(features, KDParams);

	return true;
}


int Similarity::audio_knn(int song_index, Mat indice, Mat distance)
{
	if ( song_index < 0 || song_index >= nbSongs)
	{
		printf("song_index out of range");
		return 1;
	}

	// Récupère le vecteur de la chanson dont on cherche les voisins.
	Mat audioQuery = features.row(song_index);


		//~ for (int i = 0; i < distance.cols; i++)
		//~ {
			//~ cout<< distance.at<float>(0,i) << endl;
		//~ }
		//~ cout << endl;

	// Search nearest neighbour
	index->knnSearch(audioQuery, indice, distance, nbKNN, param);


		//~ for (int i = 0; i < distance.cols; i++)
		//~ {
			//~ cout<< distance.at<float>(0,i) << endl;
		//~ }
		//~ cout << endl;


	return 0;
}


Similarity::~Similarity()
{

}
