#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;




int main() {
    // �tape 1 : Chargement de l'image
    string path = "Resources/im02.jpg";

    Mat img = imread(path);


    if (img.empty()) {
        cout << "Erreur : Impossible de charger l'image." << endl;
        return -1;
    }

    // Conversion en Lab
    Mat lab;
    cvtColor(img, lab, COLOR_BGR2Lab);

    // �tape 2 : Extraction des pixels rouges (canal a)
    vector<Mat> labChannels(3);
    split(lab, labChannels); // Diviser en L, a, b
    Mat aChannel = labChannels[1];

    // D�tection du rouge : seuils dans le canal a
    Mat redMask;

  
    
        inRange(aChannel, 140, 200, redMask); // Ajustez les seuils si n�cessaire

        // �tape 3 : Morphologie math�matique
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour r�duire le bruit
        morphologyEx(redMask, redMask, MORPH_CLOSE, kernel); // Fermeture pour combler les trous

        // �tape 4 : �tiquetage des composantes connexes
        Mat labels, stats, centroids;
        int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 8, CV_32S);

        // Affichage du nombre de b�timents d�tect�s
        cout << "Nombre de b�timents avec toit rouge : " << numComponents - 1 << endl;

        // �tape 5 : Visualisation
        Mat output = Mat::zeros(img.size(), CV_8UC3);
        RNG rng(12345);
        for (int i = 1; i < numComponents; i++) { // Ignorer l'�tiquette 0 (arri�re-plan)
            Vec3b color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            for (int y = 0; y < labels.rows; y++) {
                for (int x = 0; x < labels.cols; x++) {
                    if (labels.at<int>(y, x) == i) {
                        output.at<Vec3b>(y, x) = color;
                    }
                }
            }
        }

        // Affichage des r�sultats
        imshow("Image Originale", img);
        imshow("Toits Rouges D�tect�s", redMask);
        imshow("B�timents Identifi�s", output);


        waitKey(0);


    


    return 0;
}
