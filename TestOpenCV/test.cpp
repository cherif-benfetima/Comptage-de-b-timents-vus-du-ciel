#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;




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

    // Détection du rouge : seuils dans le canal a
    Mat redMask;

  
    
        inRange(aChannel, 140, 200, redMask); // Ajustez les seuils si nécessaire

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


        waitKey(0);


    


    return 0;
}
