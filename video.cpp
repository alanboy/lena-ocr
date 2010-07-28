#include "video.h"

#include <cstdio>

video::video()
{
    RECORD = false;
}

video::video( std::string path )
{
    RECORD = false;
    cargar( path );
}

video::video( std::string path, std::string arch )
{
    RECORD = true;
    escritor.open( arch, 0, 20.0, Size( 640, 480 ) );
    cargar( path );
}

bool video::cargar( std::string path )
{
    if( captura.isOpened() )
        captura.release();
    if( path.size() < 2 )
        captura = VideoCapture( 0 );
    else
        captura = VideoCapture( path );
    return captura.isOpened();
}

bool video::cargar( std::string path, std::string arch )
{
    RECORD = true;
    if( captura.isOpened() )
        captura.release();
    if( path.size() < 2 )
        captura = VideoCapture( 0 );
    else
        captura = VideoCapture( path );
    escritor.open( arch, CV_FOURCC( 'M','P','1','V'), 25.0, Size( 640, 480 ) );
    return captura.isOpened() && escritor.isOpened();
}

bool video::escribir( Mat &frame )
{
    if( escritor.isOpened() )
    {
        escritor << frame;
        return true;
    }
    return false;
}

bool video::proxima( Mat &img )
{
    return captura.isOpened() && captura.retrieve( img );
}

bool video::valido()
{
    return captura.isOpened();
}
