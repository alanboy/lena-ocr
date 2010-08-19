#ifndef PROCESO_H
#define PROCESO_H

#include "frame.h"
#include "video.h"

class proceso
{
    public:
        proceso( bool );

        /**
        *   Carga la imagen en un objeto de tipo frame y llama a
        *   procesarImagen( frame & );
        */
        bool procesarImagen( char * );

        /**
        *   Detecta zonas con texto en la imagen.
        *   Retorna true si proceso la imagen de forma
        *   correcta.
        */
        bool procesarImagen( frame & );

        /**
        *   Procesa un viedo y para cada frame llama a
        *   procesarImagen( frame & );
        *   Si el parametro tiene una longitud menor a 2
        *   se intenta tomar el video desde la camara por
        *   default del sistema.
        */
        bool procesarVideo( char *, char * );

    private:

        void convolucion( frame &, frame * );

        int weight( frame &, int, int );

        video captura;

        double kernel[ 4 ][ 9 ];

        int BORDE, frame_num;

        bool DEBUG, RECORD, PROCESAR;

};

#endif
