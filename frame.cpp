#include "frame.h"

/**
*   Para revisar los 8 vecinos mas rapidamente.
*/
const int X[] = { 1, 1, 1, -1, -1, -1, 0, 0 };
const int Y[] = { 1, 0, -1, 1, 0, -1, 1, -1 };

frame::frame()
{
}

frame::frame( std::string img )
{
    cargar( img );
}

frame::frame( Mat mt )
{
    imagen_ = mt.clone();
}

bool frame::cargar( std::string img )
{
    if( !imagen_.empty() )
        imagen_.release();
    imagen_ = imread( img );
    return !imagen_.empty();
}

bool frame::mostrarImagen( std::string titulo )
{
    if( imagen_.empty() )
        return false;
    namedWindow( titulo, CV_WINDOW_AUTOSIZE );
    imshow( titulo, imagen_ );
    return true;
}

bool frame::convolucion( double K[ 9 ] )
{
    if( imagen_.empty() )
        return false;
    Mat d, correlational_ker( 3, 3, CV_64FC1, K, 3 * sizeof( double ) ), convolution_ker;
    flip( correlational_ker, convolution_ker, -1 );
    filter2D( imagen_, d, imagen_.depth(), convolution_ker );
    imagen_.release();
    imagen_ = d.clone();
    d.release();
    return true;
}

bool frame::valido()
{
    return imagen_.empty();
}

Mat& frame::imagen()
{
    return imagen_;
}

unsigned char frame::dataAt( int x, int y ) const
{
    return imagen_.at< unsigned char >( x, y );
}

float frame::fDataAt( int x, int y ) const
{
    return imagen_.at< float >( x, y );
}

void frame::setData( int x, int y, unsigned char d )
{
    imagen_.at< unsigned char >( x, y ) = d;
}

void frame::fSetData( int x, int y, float d )
{
    imagen_.at< float >( x, y ) = d;
}

Size frame::imageSize()
{
    return imagen_.size();
}

int frame::tipo()
{
    return imagen_.type();
}

frame frame::operator|( const frame &a )
{
    int w = imageSize().width, h = imageSize().height;
    frame res( Mat( imageSize(), CV_8UC1 ) ); /* soporta solo un canal y de 8 bits */
    unsigned char data;
    for( int i = 0; i < h; ++i )
        for( int j = 0; j < w; ++j )
        {
            data = ( dataAt( i, j ) == 0xff || a.dataAt( i, j ) == 0xff ) ? 0xff : 0x0;
            res.setData( i, j, data );
        }
    return res;
}

frame frame::operator&( const frame &a )
{
    int w = imageSize().width, h = imageSize().height;
    frame res( Mat( imageSize(), CV_8UC1 ) ); /* soporta solo un canal y de 8 bits */
    unsigned char data;
    for( int i = 0; i < h; ++i )
        for( int j = 0; j < w; ++j )
        {
            data = ( dataAt( i, j ) == 0xff && a.dataAt( i, j ) == 0xff ) ? 0xff : 0x0;
            res.setData( i, j, data );
        }
    return res;
}

frame frame::thinning( int level )
{
    frame target( imagen_ );
    int w = imageSize().width, h = imageSize().height;
    for( int i = 0; i < h; ++i )
        for( int j = 0; j < w; ++j )
            if( dataAt( i, j ) == 0xff )
            {
                int cn = 2;
                if( j > 0 && dataAt( i, j - 1 ) != 0xff )
                    --cn;
                if( j + 1 < w && dataAt( i, j + 1 ) != 0xff )
                    --cn;
                if( cn < level )
                    target.setData( i, j, 0x00 );
            }
    return target;
}

bool comparador( const std::vector< Point > &a, const std::vector< Point > &b )
{
    return a.size() < b.size();
}

frame frame::labeling( int min, int max )
{
    Mat resource( imagen().clone() );
    /*
        Usar findContour para encontrar los grupos de componentes
        conectados. Vamos a trabajar sobre una copia de this para no alterar
        la imagen.
    */
    std::vector< std::vector< Point > > contours;
    std::vector< Vec4i > hierarchy;
    findContours( resource, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
    std::cerr << "Se encontraron " << contours.size() << " grupos de componentes\n";
    std::sort( contours.begin(), contours.end(), comparador );
    while( !contours.empty() && contours[ 0 ].size() < min )
        contours.erase( contours.begin() );
    while( !contours.empty() && contours.back().size() > max )
        contours.pop_back();
    std::cerr << "Se filtro a " << contours.size() << " grupos de componentes\n";
    Mat tresh( imageSize(), CV_8UC1 );
    tresh = Mat::zeros( tresh.size(), CV_8UC1 );
    drawContours( tresh, contours, -1, Scalar( 0xff ), CV_FILLED );
    return frame( tresh );
}

frame frame::removeNoise()
{
    int w = imageSize().width, h = imageSize().height;
    frame res( Mat( imageSize(), CV_8UC1 ) );
    int promedio, suma, vecinos;
    for( int i = 0; i < h; ++i )
        for( int j = 0; j < w; ++j )
        {
            promedio = suma = vecinos = 0;
            for( int k = 0; k < 8; ++k )
                if( i + X[ k ] >= 0 && i + X[ k ] < h && j + Y[ k ] >= 0 && j + Y[ k ] < w )
                {
                    ++vecinos;
                    suma += dataAt( i + X[ k ], j + Y[ k ] );
                }
                promedio = ( int )ceil( suma / ( double )vecinos );
            if( dataAt( i, j ) == 0xff && promedio < 80 )
                res.setData( i, j, 0x0 );
            else if( dataAt( i, j ) == 0xff )
                res.setData( i, j, 0xff );
            else
                res.setData( i, j, 0x0 );
        }
    return res;
}
