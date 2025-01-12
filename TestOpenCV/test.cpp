#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;



// l�espace RGB a n�cessit� 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
// l�espace RGB a n�cessit� 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
// l�espace RGB a n�cessit� 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
int main() {
    // �tape 1 : Chargement de l'image
    string path = "Resources/im05.jpg";
    Mat img = imread(path);
    if (img.empty()) {
        cout << "Erreur : Impossible de charger l'image." << endl;
        return -1;
    }

    //�tape 2  Conversion en Lab
    Mat lab;
    cvtColor(img, lab, COLOR_BGR2Lab);

    // �tape 3 : Extraction des pixels rouges 
    vector<Mat> labChannels(3);
    split(lab, labChannels); // Diviser en L, a, b
    Mat lChannel = labChannels[0];
    Mat aChannel = labChannels[1];
    Mat bChannel = labChannels[2];

    // D�tection du rouge : seuils dans le canal a
    Mat redMask;
    inRange(aChannel, 140, 200, redMask); // 
    // inRange(lChannel, 50, 200, redMask);  // Limiter aux zones de luminosit� moyenne
    // inRange(bChannel, 120, 160, redMask);  // Exclusion des zones jaunes ou bleues


     // �tape 4 : Morphologie math�matique
    int k1 = 3, k2 = 7;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(k1, k1));
    Mat kernel2 = getStructuringElement(MORPH_RECT, Size(k2, k2));

    morphologyEx(redMask, redMask, MORPH_CLOSE, kernel2); // Fermeture pour combler les trous
    morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour r�duire le bruit

     // �tape 5 : �tiquetage des composantes connexes
    Mat labels, stats, centroids;
    int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 8, CV_32S);// 8 Connexit� 

    // �tape 6 Filtrer les petites composantes** ++ colorisation
    int minArea = 300;   
    int validComponents = 0;
    Mat filteredMask = Mat::zeros(redMask.size(), CV_8U);
    Mat colorRegions = Mat::zeros(img.size(), CV_8UC3); 
    RNG rng(12345); // G�n�rateur de couleurs al�atoires

    for (int i = 1; i < numComponents; i++) { // Ignorer l'arri�re-plan (i = 0)
        int area = stats.at<int>(i, CC_STAT_AREA);
        Vec3b color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); // G�n�rer une couleur al�atoire
        // Ajouter la composante color�e au masque
        for (int y = 0; y < labels.rows; y++) {
            for (int x = 0; x < labels.cols; x++) {
                if (labels.at<int>(y, x) == i) {
                    colorRegions.at<Vec3b>(y, x) = color; 
                    if (area >= minArea) {
                        filteredMask.at<uchar>(y, x) = 255; // Ajouter au masque filtr�
                    }
                }
            }
        }
        if (area >= minArea) {
            validComponents++; 
        }
    }

    cout << "Nombre de b�timents avec toit rouge : " << validComponents << endl;

    // �tape 7 : Visualisation
    imshow("Image Originale", img);
    imshow("R�gions Coloris�es", colorRegions);
    imshow("Toits Rouges D�tect�s (Filtr�s)", filteredMask);
    waitKey(0);

    return 0;
}
