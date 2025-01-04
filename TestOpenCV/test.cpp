#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;


int amin = 140, amax = 255, bmin = 255, bmax = 255, lmin = 255, lmax = 255;


int main() {
    // Étape 1 : Chargement de l'image
    string path = "Resources/im02.jpg";

    Mat img = imread(path);


    if (img.empty()) {
        cout << "Erreur : Impossible de charger l'image." << endl;
        return -1;
    }

    // Conversion en Lab
    Mat lab;
    cvtColor(img, lab, COLOR_BGR2Lab);

    // Étape 2 : Extraction des pixels rouges (canal a)
    vector<Mat> labChannels(3);
    split(lab, labChannels); // Diviser en L, a, b
    Mat aChannel = labChannels[1];

    //################
    Mat lChannel = labChannels[0];
    Mat bChannel = labChannels[2];

    // Seuils sur les canaux
    Mat lMask, aMask, bMask;

    namedWindow("TrackBar", (620, 200));
    createTrackbar("Amin", "TrackBar", &amin, 130);
    createTrackbar("Amax", "TrackBar", &amax, 255);
    createTrackbar("Bmin", "TrackBar", &bmin, 255);
    createTrackbar("Bmax", "TrackBar", &bmax, 255);
    createTrackbar("Lmin", "TrackBar", &lmin, 255);
    createTrackbar("Lmax", "TrackBar", &lmax, 255);


    while (true) {
        //  inRange(lChannel, 50, 250, lMask);  // Luminosité entre 50 et 200
        //  inRange(aChannel, 140, 200, aMask); // Rouge dans le canal a
        //  inRange(bChannel, 120, 160, bMask); // Exclusion des zones jaunes/bleues

        inRange(lChannel, lmin, lmax, lMask);  // Luminosité entre 50 et 200
        inRange(aChannel, amin, amax, aMask); // Rouge dans le canal a
        inRange(bChannel, bmin, bmax, bMask); // Exclusion des zones jaunes/bleues

        // Combiner les masques
        Mat redMask;
        bitwise_and(aMask, lMask, redMask);
        bitwise_and(redMask, bMask, redMask);

        //#################


        // Détection du rouge : seuils dans le canal a
       // Mat redMask;
        //inRange(aChannel, 140, 200, redMask); // Ajustez les seuils si nécessaire

        // Étape 3 : Morphologie mathématique
        int ddd = 9, dddd = 9;
        Mat kernel = getStructuringElement(MORPH_RECT, Size(ddd, ddd));
        Mat kernel2 = getStructuringElement(MORPH_RECT, Size(dddd, dddd));

        morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour réduire le bruit
        morphologyEx(redMask, redMask, MORPH_CLOSE, kernel2); // Fermeture pour combler les trous

        // Étape 4 : Étiquetage des composantes connexes
        Mat labels, stats, centroids;
        int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 4, CV_32S);

        // **Filtrer les petites composantes**
        int minArea = 150;//109; // Ajustez cette valeur selon les dimensions des bâtiments
        int validComponents = 0;
        Mat filteredMask = Mat::zeros(redMask.size(), CV_8U);

        for (int i = 1; i < numComponents; i++) { // Ignorer l'arrière-plan (i = 0)
            int area = stats.at<int>(i, CC_STAT_AREA);
            if (area >= minArea) {
                validComponents++;
                // Ajouter cette composante au masque filtré
                for (int y = 0; y < labels.rows; y++) {
                    for (int x = 0; x < labels.cols; x++) {
                        if (labels.at<int>(y, x) == i) {
                            filteredMask.at<uchar>(y, x) = 255;
                        }
                    }
                }
            }
        }

        // Affichage du nombre de bâtiments valides détectés
        cout << "Nombre de bâtiments avec toit rouge (valide) : " << validComponents << endl;

        // Étape 5 : Visualisation
        imshow("Image Originale", img);
        imshow("Toits Rouges Détectés (Filtrés)", filteredMask);
        waitKey(1);


    }
        return 0;
    


}
/*
    if (img.empty()) {
        cout << "Erreur : Impossible de charger l'image." << endl;
        return -1;
    }

    // Conversion en Lab
    Mat lab;
    cvtColor(img, lab, COLOR_BGR2Lab);

    // Étape 2 : Extraction des pixels rouges (canal a)
    vector<Mat> labChannels(3);
    split(lab, labChannels); // Diviser en L, a, b
    Mat aChannel = labChannels[1];

    // Détection du rouge : seuils dans le canal a
    Mat redMask;

    namedWindow("TrackBar", (620, 200));
    createTrackbar("Amin", "TrackBar",&amin,255 );
    createTrackbar("Amax", "TrackBar",&amax,255 );

    while(true){

        inRange(aChannel, amin, amax, redMask); // Ajustez les seuils si nécessaire

        // Étape 3 : Morphologie mathématique
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour réduire le bruit
        morphologyEx(redMask, redMask, MORPH_CLOSE, kernel); // Fermeture pour combler les trous

        // Étape 4 : Étiquetage des composantes connexes
        Mat labels, stats, centroids;
        int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 8, CV_32S);

        // Affichage du nombre de bâtiments détectés
        cout << "Nombre de bâtiments avec toit rouge : " << numComponents - 1 << endl;

        // Étape 5 : Visualisation
        Mat output = Mat::zeros(img.size(), CV_8UC3);
        RNG rng(12345);
        for (int i = 1; i < numComponents; i++) { // Ignorer l'étiquette 0 (arrière-plan)
            Vec3b color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            for (int y = 0; y < labels.rows; y++) {
                for (int x = 0; x < labels.cols; x++) {
                    if (labels.at<int>(y, x) == i) {
                        output.at<Vec3b>(y, x) = color;
                    }
                }
            }
        }

        // Affichage des résultats
        imshow("Image Originale", img);
        imshow("Toits Rouges Détectés", redMask);
        imshow("Bâtiments Identifiés", output);


        waitKey(1);


    }


    return 0;
}
*/