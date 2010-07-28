#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

#include "proceso.h"

using namespace cv;

void imprime_ayuda()
{
    printf( "\nYou must pass at least 2 arguments.\n\n" );
    printf( "Usage:\n" );
    printf( "\tlena args\n" );
    printf( "args: \n" );
    printf( "\tv file \t- process a video\n" );
    printf( "\ti file \t- process an image\n" );
    printf( "\tc \t- process a video from default camera\n" );
    printf( "\td \t- run in debug mode, warning using this parameter will cause to display ten or more windows.\n" );
    printf( "\ts file \t- save the video procesed in file( only with options v or c ).\n\n" );
}

/**
*  main
*
**/
int main( int argc, char *argv[])
{
    bool v = false, i = false, d = false, c = false, s = false, error = true;
    int save_index = 0, file_index = 0;
    for( int k = 1; k < argc; ++k )
        switch( argv[ k ][ 0 ] )
        {
            case 'v':
                error = error && ( v = ++k < argc ); /* si existe el indice con el nombre */
                file_index = k;
                break;
            case 'i':
                error = error && ( i = ++k < argc );
                file_index = k;
                break;
            case 'c':
                c = true;
                break;
            case 'd':
                d = true;
                break;
            case 's':
                error = error && (s = ++k < argc );
                save_index = k; /* index del nombre */
                break;
        };

    proceso proc( d );

    if( ( v && i ) || ( v && c ) || ( i && c ) || !error )
    {
        printf( "\nYou must specify only one of i, v or c.\n\n" );
        imprime_ayuda();
    }
    else if( v )
    {
        if( ( !s && !proc.procesarVideo( argv[ file_index ], NULL ) ) || ( s && !proc.procesarVideo( argv[ file_index ], argv[ save_index ] ) ) )
            printf( "The image %s could not be processed.\n", argv[ file_index ] );
    }
    else if( i )
    {
        if( !proc.procesarImagen( argv[ file_index ] ) )
            printf( "The image %s could not be processed.\n", argv[ file_index ] );
    }
    else if( c )
    {
        if( ( !s && !proc.procesarVideo( NULL, NULL ) ) || ( s && !proc.procesarVideo( NULL, argv[ save_index ] ) ) )
            printf( "The video from the camera could not be processed.\n" );
    }
    else
        imprime_ayuda();

    return 0;
}
