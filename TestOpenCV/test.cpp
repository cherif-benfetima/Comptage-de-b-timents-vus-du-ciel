#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;




int main() {
        // Étape 1 : Chargement de l'image
        string path = "Resources/im08.jpg";

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

        // Détection du rouge : seuils dans le canal a
        Mat redMask;
        inRange(aChannel, 140, 200, redMask); // Ajustez les seuils si nécessaire

        // Étape 3 : Morphologie mathématique
        int k1 = 9, k2 = 9;
        Mat kernel = getStructuringElement(MORPH_RECT, Size(k1,k1));
        Mat kernel2 = getStructuringElement(MORPH_RECT, Size(k2,k2));

        morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour réduire le bruit
        morphologyEx(redMask, redMask, MORPH_CLOSE, kernel2); // Fermeture pour combler les trous

        // Étape 4 : Étiquetage des composantes connexes
        Mat labels, stats, centroids;
        int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 8, CV_32S);

            // **Filtrer les petites composantes**
            int minArea = 10; // Ajustez cette valeur selon les dimensions des bâtiments
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
    waitKey(0);



    


    return 0;
}
