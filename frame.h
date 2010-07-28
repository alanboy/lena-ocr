#ifndef FRAME_H
#define FRAME_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

using namespace cv;

class frame
{
    public:
        /* constructores */
        frame();
        frame( std::string );
        frame( Mat );

        /**
        *   Intenta cargar una imagen
        */
        bool cargar( std::string );

        /**
        *   Muestra una imagen en una ventana
        *   con el titutlo especificado.
        */
        bool mostrarImagen( std::string );

        /**
        *   Regresa la imagen para que se le puedan hacer
        *   operaciones mas especificas.
        */
        Mat& imagen();

        /**
        *   Realiza la convolucion en la imagen
        *   con el kernel.
        */
        bool convolucion( double [ 9 ] );

        /**
        *   retorna imagen_.empty();
        */
        bool valido();

        /**
        *   retorna un caracter sin signo que es contenido de
        *   la matriz en esa posicion.
        */
        unsigned char dataAt( int, int ) const;
        float fDataAt( int, int ) const;

        void setData( int, int, unsigned char );
        void fSetData( int, int, float );

        /**
        *   Retorna el tamano de la imagen.
        */
        Size imageSize();

        /**
        *   Retorna el tipo de datos de la imagen.
        */
        int tipo();

        /**
        *   Operador or.
        */
        frame operator|( const frame & );

        /**
        *   Operador and.
        */
        frame operator&( const frame & );

        /**
        *   Operador thinning.
        *   TODO: No esta funcionando correctamente.
        */
        frame thinning( int level = 2 );

        /**
        *   Labeling operator.
        *
        *   A flood fill operator that counts the number of
        *   pixels set to 0xff and represents that in a new
        *   frame.
        */
        frame labeling( int, int );

        /**
        *   Revisa la imagen para tratar de remover
        *   el ruido que puede afectar el procesamiento
        *   de la imagen.
        */
        frame removeNoise();

        /**
        *   Escribe un cuadro en la imagen en las areas donde
        *   se cree que hay texto.
        */
        void drawTextHull( frame & );

    private:

        Mat imagen_;

};

#endif
