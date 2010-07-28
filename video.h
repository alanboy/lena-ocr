#ifndef VIDEO_H
#define VIDEO_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace cv;

class video
{
    public:

        video();
        video( std::string );
        video( std::string, std::string );

        bool cargar( std::string );
        bool cargar( std::string, std::string );

        bool escribir( Mat & );

        bool proxima( Mat & );

        bool valido();

    private:

        VideoCapture captura;
        VideoWriter escritor;

        bool RECORD, PROCESAR;
        int BORDE;
};

#endif
