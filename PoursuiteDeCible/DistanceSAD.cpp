#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <limits>
#include "CImg.h"

using namespace cimg_library;
namespace fs = std::filesystem;

const unsigned char red[] = {255,0,0};

double SAD(const CImg<double>& motif, const CImg<double>& sous_image) {
    int nx = motif.width(); // Peu importe motif ou sous-image car mêmes dimensions
    int ny = motif.height();
    double somme = 0.0;
    for(int i = 0; i < nx; ++i) {
        for(int j = 0; j < ny; ++j) {
            double val_motif = motif(i,j);
            double val_sous_image = sous_image(i,j);
            somme += std::abs(val_motif - val_sous_image);
        }
    }
    somme /= (nx * ny);
    return somme;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Utilisation : " << argv[0] << " <nom_du_dossier>" << std::endl;
        return 1;
    }

    fs::remove_all("resultsSAD");
    fs::create_directory("resultsSAD");

    std::string dossier = argv[1];
    std::vector<fs::path> images;

    // Lire le dossier et récupérer tous les fichiers d'image
    for (const auto& entry : fs::directory_iterator(dossier)) {
        if (entry.is_regular_file()) {
            images.push_back(entry.path());
        }
    }

    // Trier les fichiers par ordre alphanumérique pour assurer un parcours cohérent
    std::sort(images.begin(), images.end());

    // Vérifier s'il y a au moins une image
    if (images.empty()) {
        std::cerr << "Aucune image trouvée dans le dossier : " << dossier << std::endl;
        return 1;
    }
    
    CImg<unsigned char> Image_lue(images[0].c_str());
    CImgDisplay Image_disp(Image_lue, "Poursuite de cible");

    // Initialisation pour la première image
    CImg<double> motif;
    int nx = 0, ny = 0;  // Dimensions du motif
    int x0 = -1, y0 = -1, x1 = -1, y1 = -1;
    bool is_first_click = true;

    // Sélection de la zone (motif) dans la première image
    while (!Image_disp.is_closed() && !Image_disp.is_keyESC()) {
        Image_disp.wait();
        
        if (Image_disp.button() & 1 && Image_disp.mouse_x() >= 0 && Image_disp.mouse_y() >= 0) {
            if (is_first_click) {
                // Premier clic - coin supérieur gauche de la sélection
                x0 = Image_disp.mouse_x();
                y0 = Image_disp.mouse_y();
                is_first_click = false;
                std::cout << "Premier point sélectionné : (" << x0 << ", " << y0 << ")" << std::endl;
            } else {
                x1 = Image_disp.mouse_x();
                y1 = Image_disp.mouse_y();
                is_first_click = true;

                nx = std::abs(x1 - x0) + 1;
                ny = std::abs(y1 - y0) + 1;
                motif = Image_lue.get_crop(x0, y0, x1, y1);
                
                Image_lue.draw_rectangle(x0, y0, x1, y1, red, 1, ~0U).display(Image_disp);
                Image_lue.save("resultsSAD/result000.png");
                std::cout << "Motif sélectionné aux coordonnées : (" << x0 << ", " << y0 << ") - (" << x1 << ", " << y1 << ")" << std::endl;
                break;
            }
        }
    }

    int meilleure_x = x0, meilleure_y = y0;
    // Boucle sur les images de la séquence
    for (size_t i = 1; i < images.size(); ++i) {
        Image_lue.load(images[i].c_str());

        // Recherche de la position du motif dans l'image courante
        double meilleure_distance = std::numeric_limits<double>::max();
        
        int zone_recherche = 10;
        for (int x = std::max(0, meilleure_x - zone_recherche); x <= std::min(Image_lue.width() - nx, meilleure_x + zone_recherche); ++x) {
            for (int y = std::max(0, meilleure_y - zone_recherche); y <= std::min(Image_lue.height() - ny, meilleure_y + zone_recherche); ++y) {
                
                if (x + nx - 1 >= Image_lue.width() || y + ny - 1 >= Image_lue.height()) continue;
                
                CImg<double> sous_image = Image_lue.get_crop(x, y, x + nx - 1, y + ny - 1);

                double distance = SAD(motif, sous_image);

                if (distance < meilleure_distance) {
                    meilleure_distance = distance;
                    meilleure_x = x;
                    meilleure_y = y;
                }
            }
        }
        Image_lue.draw_rectangle(meilleure_x, meilleure_y, meilleure_x + nx - 1, meilleure_y + ny - 1, red, 1, ~0U).display(Image_disp);
        char filename[256];
        sprintf(filename, "resultsSAD/result%03d.png", (int)i);
        Image_lue.save(filename);
    }
    char command[256];
    sprintf(command, "magick resultsSAD/*.png resultsSAD/output.pdf");
    int command_result = system(command);
    if(command_result != 0) {
        std::cout<<"Erreur lors de la création du PDF de sortie"<<std::endl;
    }
    return 0;
}