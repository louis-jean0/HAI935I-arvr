#include <iostream>
#include "CImg.h"
using namespace cimg_library;

const unsigned char red[] = {255, 0, 0};
const unsigned char green[] = {0, 255, 0};
const unsigned char blue[] = {0, 0, 255}; 

int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout<<"Utilisation : "<<argv[0]<<" image_gauche.tif image_droite.tif"<<std::endl;
        exit(EXIT_FAILURE);
    }
    char nom_image_gauche[255];
    char nom_image_droite[255];
    sscanf("%s", argv[1], nom_image_gauche);
    sscanf("%s", argv[2], nom_image_droite);
    CImg<unsigned char> image("bobheadshat.png");
    CImgDisplay main_disp(image, "Click a point");
    while(!main_disp.is_closed() && !main_disp.is_keyESC()) {
        main_disp.wait();
        if(main_disp.button()&1 && main_disp.mouse_x() >= 0 && main_disp.mouse_y() >= 0) {
            const int x = main_disp.mouse_x();
            const int y = main_disp.mouse_y();
            const int red_value = image(x,y,0,0);
            const int green_value = image(x,y,0,1);
            const int blue_value = image(x,y,0,2);
            std::cout<<"Pixel position = ("<<x<<","<<y<<"), value = "<<"("<<red_value<<","<<green_value<<","<<blue_value<<")"<<std::endl;
        }
        if(main_disp.button()&2) {

        }
    }
    return 0;
}