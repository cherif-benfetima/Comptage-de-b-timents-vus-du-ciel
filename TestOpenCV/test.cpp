#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;



// l’espace RGB a nécessité 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
// l’espace RGB a nécessité 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
// l’espace RGB a nécessité 6 seuils, rgb et HSV 4 seuils et le Lab juste 2 seuils.
int main() {
    // Étape 1 : Chargement de l'image
    string path = "Resources/im05.jpg";

    Mat img = imread(path);


    if (img.empty()) {
        cout << "Erreur : Impossible de charger l'image." << endl;
        return -1;
    }

    // Conversion en Lab
    Mat lab;
    cvtColor(img, lab, COLOR_BGR2Lab);

    // Étape 2 : Extraction des pixels rouges 
    vector<Mat> labChannels(3);
    split(lab, labChannels); // Diviser en L, a, b
    Mat lChannel = labChannels[0];
    Mat aChannel = labChannels[1];
    Mat bChannel = labChannels[2];

    // Détection du rouge : seuils dans le canal a
    Mat redMask;
    inRange(aChannel, 140, 200, redMask); // Ajustez les seuils si nécessaire
    // inRange(lChannel, 50, 200, redMask);  // Limiter aux zones de luminosité moyenne
    // inRange(bChannel, 120, 160, redMask);  // Exclusion des zones jaunes ou bleues
    // imshow("Image avec seuil", redMask);


     // Étape 3 : Morphologie mathématique
    int k1 = 3, k2 = 7;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(k1, k1));
    Mat kernel2 = getStructuringElement(MORPH_RECT, Size(k2, k2));

    morphologyEx(redMask, redMask, MORPH_CLOSE, kernel2); // Fermeture pour combler les trous
    morphologyEx(redMask, redMask, MORPH_OPEN, kernel);  // Ouverture pour réduire le bruit

    // imshow("Image apres math", redMask);


     // Étape 4 : Étiquetage des composantes connexes
    Mat labels, stats, centroids;
    int numComponents = connectedComponentsWithStats(redMask, labels, stats, centroids, 8, CV_32S);// 8 Connexité 

    // **Filtrer les petites composantes** ++ colorisation
    int minArea = 300; //  Valeur  
    int validComponents = 0;
    Mat filteredMask = Mat::zeros(redMask.size(), CV_8U);

    Mat colorRegions = Mat::zeros(img.size(), CV_8UC3); // Pour la colorisation
    RNG rng(12345); // Générateur de couleurs aléatoires


    for (int i = 1; i < numComponents; i++) { // Ignorer l'arrière-plan (i = 0)
        int area = stats.at<int>(i, CC_STAT_AREA);

        Vec3b color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); // Générer une couleur aléatoire


        // Ajouter la composante colorée au masque
        for (int y = 0; y < labels.rows; y++) {
            for (int x = 0; x < labels.cols; x++) {
                if (labels.at<int>(y, x) == i) {
                    colorRegions.at<Vec3b>(y, x) = color; // Colorisation
                    if (area >= minArea) {
                        filteredMask.at<uchar>(y, x) = 255; // Ajouter au masque filtré
                    }
                }
            }
        }

        if (area >= minArea) {
            validComponents++; // Compter les composantes valides
        }
    }


    // Affichage du nombre de bâtiments valides détectés
    cout << "Nombre de bâtiments avec toit rouge : " << validComponents << endl;

    // Étape 5 : Visualisation
    imshow("Image Originale", img);
    imshow("Régions Colorisées", colorRegions);
    imshow("Toits Rouges Détectés (Filtrés)", filteredMask);
    waitKey(0);

    return 0;
}
