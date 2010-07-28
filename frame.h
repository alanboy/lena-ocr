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
        *   Hace un ajuste del area de interes
        *   de la imagen.
        *   Por el momento esta funcion no hace nada.
        */
        bool revisarROI( int, int, int, int );

        /**
        *   retorna imagen_.empty();
        */
        bool valido();

        /**
        *   retorna un caracter sin signo que es contenido de
        *   la matriz en esa posicion.
        */
        unsigned char dataAt( int, int ) const;

        void setData( int, int, unsigned char );

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

    private:

        /**
        *   operador hit-and-miss
        *   Se usa el centro del kernel.
        *   El kernel contiene 0x0, 0x66 y 0xff.
        *   0x66 es un punto que carece de interes.
        */
        frame hit_and_miss( const Mat & );

        /**
        *   Rotar un objeto Mat 90 grados.
        */
        void rotar( Mat & );

        Mat imagen_;

};

#endif
