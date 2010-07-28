#include "proceso.h"

#include <cstdio>

proceso::proceso( bool dbg )
{
    DEBUG = dbg;
    RECORD = false;
    PROCESAR = true;
    BORDE = 20;
    frame_num = 0;
    for( int i = 0; i < 3; ++i )
        for( int j = 0; j < 3; ++j )
            kernel[ 0 ][ ( i * 3 ) + j ] = i == 1 ? 2.0 : -1.0;
    for( int i = 0; i < 3; ++i )
        for( int j = 0; j < 3; ++j )
            kernel[ 1 ][ ( i * 3 ) + j ] = i + j == 2 ? 2.0 : -1.0;
    for( int i = 0; i < 3; ++i )
        for( int j = 0; j < 3; ++j )
            kernel[ 2 ][ ( i * 3 ) + j ] = j == 1 ? 2.0 : -1.0;
    for( int i = 0; i < 3; ++i )
        for( int j = 0; j < 3; ++j )
            kernel[ 3 ][ ( i * 3 ) + j ] = i == j ? 2.0 : -1.0;
}

bool proceso::procesarImagen( char *path )
{
    frame f( path );
    if( f.valido() )
        return false;
    if( procesarImagen( f ) )
        for( char c = waitKey( 100 ); true; c = waitKey( 100 ) )
            if( c == 'q' )
                return true;
    return false;
}

bool proceso::procesarImagen( frame &f )
{
    if( f.valido() )
        return false;
    /*
        Aqui se realiza el proceso a la imagen
    */
    f.mostrarImagen( "Original" );
    /*
        Punto A.
    */
    frame E[ 4 ];
    convolucion( f, E );
    if( DEBUG )
    {
        std::string num[] = { "0", "45", "90", "135" };
        for( int i = 0; i < 4; ++i )
            E[ i ].mostrarImagen( "Convolucion " + num[ i ] );
    }

    frame edge_strong_90bw;
    threshold( E[ 2 ].imagen(), edge_strong_90bw.imagen(), 20.0, 0xFF, THRESH_BINARY | THRESH_OTSU );
    if( DEBUG )
        edge_strong_90bw.mostrarImagen( "Edge strong 90 bw" );

    frame dilated;
    dilate( edge_strong_90bw.imagen(), dilated.imagen(), getStructuringElement( MORPH_RECT, Size( 2, 3 ) ) );
    if( DEBUG )
        dilated.mostrarImagen( "Dilated" );

    frame closed;/* TODO: el tamano mencionado en el documento causa error. */
    morphologyEx( dilated.imagen(), closed.imagen(), MORPH_CLOSE, getStructuringElement( MORPH_RECT, Size( 10, 1 ) ) );
    if( DEBUG )
        closed.mostrarImagen( "Closed" );

    frame edge_w_tresh_90( dilated.imagen() - closed.imagen() );
    edge_w_tresh_90.imagen().mul( E[ 2 ].imagen() );

    frame edge_weak_90;
    threshold( edge_w_tresh_90.imagen(), edge_weak_90.imagen(), 20.0, 0xFF, THRESH_BINARY | THRESH_OTSU );


    if( DEBUG )
        edge_weak_90.mostrarImagen( "Edge weak 90" );

    frame edge_bw_90( edge_strong_90bw.imagen() + edge_weak_90.imagen() );
    if( DEBUG )
        edge_bw_90.mostrarImagen( "Edge bw 90" );

    /*
        TODO:
            Hasta aqui el resultado se ve bien. OpenCV no implementa un operador
            thinning. Usando erode no se obtienen buenos resultado. Mi implementacion
            del operador thinning no es muy buena.
            Todavia no se usa este resultado.
    */
    frame prethin;
    dilate( edge_bw_90.imagen(), prethin.imagen(), getStructuringElement( MORPH_RECT, Size( 0, 4 ) ) );
    frame thinned = prethin.thinning(  );
    //erode( edge_bw_90.imagen(), thinned.imagen(), getStructuringElement( MORPH_RECT, Size( 2, 3 ) ) );
    if( DEBUG )
        thinned.mostrarImagen( "Thinned" );
    frame short_ed_bw_90 = edge_bw_90.labeling( 15, 70 );
    if( DEBUG )
        short_ed_bw_90.mostrarImagen( "Short edges" );

    frame candidate;
    dilate( short_ed_bw_90.imagen(), candidate.imagen(), getStructuringElement( MORPH_RECT, Size( 3, 6 ) ) );

    if( DEBUG )
        candidate.mostrarImagen( "Candidatos" );

    frame sums;
    add( E[ 0 ].imagen(), E[ 1 ].imagen(), sums.imagen() );
    add( sums.imagen(), E[ 2 ].imagen(), sums.imagen() );
    add( sums.imagen(), E[ 3 ].imagen(), sums.imagen() );
    frame refined( candidate.imagen().mul( sums.imagen() ) );
    if( DEBUG )
        refined.mostrarImagen( "Refinada" );

    frame idea, idea_label, anded, anded_short;
    morphologyEx( refined.imagen(), idea.imagen(), MORPH_CLOSE, getStructuringElement( MORPH_RECT, Size( 10, 1 ) ) );
    if( DEBUG )
        idea.mostrarImagen( "Idea" );

    idea_label = idea.labeling( 50, 999999 );
    if( DEBUG )
        idea_label.mostrarImagen( "Idea labeled" );

    anded = idea_label & refined;
    anded.mostrarImagen( "Anded" );

    anded_short = idea_label & short_ed_bw_90;
    anded_short.mostrarImagen( "Anded short" );
    /*
    Fin punto A.
    */

    return true;
}

void proceso::convolucion( frame &f, frame *conv )
{
    Mat bw( f.imagen().size(), CV_8UC1 ); /* imagen escala de grises */

    cvtColor( f.imagen(), bw, CV_RGB2GRAY ); /* pone el contenido de f en bw en escala de grises */

    for( int i = 0; i < 4; ++i ) /* convolucion de las cuatro imagenes */
    {
        conv[ i ] = frame( bw );
        conv[ i ].convolucion( kernel[ i ] );
    }
}

void proceso::encontrarContornos( frame &f, frame &canny, frame &fpuntos, bool opt )
{
    std::vector< std::vector< Point > > contours;
    findContours( canny.imagen(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

    canny.imagen().zeros( canny.imageSize(), canny.tipo() );

    for( int i = 0, Nc = contours.size(); i < Nc; ++i )
    {
        int maxX = -1, maxY = -1, minX = 1000, minY = 1000;
        for( int j = 0, s = contours[ i ].size(); j < s; ++j )
        {
            if( contours[ i ][ j ].x > maxX )
                maxX = contours[ i ][ j ].x;
            if( contours[ i ][ j ].y > maxY )
                maxY = contours[ i ][ j ].y;
            if( contours[ i ][ j ].x < minX )
                minX = contours[ i ][ j ].x;
            if( contours[ i ][ j ].y < minY )
                minY = contours[ i ][ j ].y;
        }
        if( opt )
            envoltorio( f, canny, fpuntos, contours[ i ], maxX, maxY, minX, minY );
        else if( !( ( maxX - minX ) >= ( canny.imageSize().width - 10 ) ) )
            for( int j = 0, s = contours[ i ].size(); j < s ; ++j )
                line( canny.imagen(), contours[ i ][ j - 1 < 0 ? ( s - 1 ):( j - 1 ) ], contours[ i ][ j ], Scalar( 255, 255, 255 ), 2, CV_AA );
    }

    contours.clear();
}

void proceso::envoltorio( frame &f, frame &canny, frame &fpuntos, std::vector< Point > &contour, int maxX, int maxY, int minX, int minY )
{
    Size fs = f.imageSize(), size;
    Point off;
    int croi, puntos;
    double puntos_area;
    char bufferTexto[ 15 ];
    if( !( ( ( maxX - minX ) > ( fs.width * 0.8 ) ) || ( ( maxY - minY ) > ( fs.height * 0.8 ) ) ) )
    {
        if( ( maxX - minX ) < 50 )
            return;
        rectangle( f.imagen(), Point( minX, minY ), Point( maxX, maxY ), Scalar( 0, 0, 255 ), 2 );
        puntos = 0;
        for( int x = minX; x <= maxX; ++x )
            for( int y = minY; y <= maxY; ++y )
                if( fpuntos.dataAt( x, y ) > 20 )
                    ++puntos;
                puntos_area = ( ( ( double )puntos ) / ( ( double )( ( maxX - minX ) * ( maxY - minY ) ) ) ) * 100.0;
            if( puntos_area < 2.0 )
                return;
            croi = f.revisarROI( minX, minY, maxX - minX, maxY - minY );
        f.imagen().locateROI( size, off );
        f.imagen().adjustROI( off.y, size.height - ( off.y + fs.height ), off.x, size.width - ( off.x + fs.width ) );
        if( !croi )
            return;
        sprintf( bufferTexto, "%lf", puntos_area );
        std::vector< Point > hull;
        Mat points( contour );
        convexHull( points, hull, true );
        for( int j = 0, s = hull.size(); j < s; ++j )
            line( f.imagen(), hull[ ( j - 1 < 0 ? ( s - 1 ) : ( j - 1 ) ) ], hull[ j ], Scalar( 255, 255, 0 ), 1, CV_AA );
    }
}

bool proceso::procesarVideo( char *path, char *save )
{
    bool s = ( save != NULL );
    if( ( !s && !captura.cargar( path == NULL ? "" : path ) ) || ( s && !captura.cargar( path == NULL ? "" : path, save ) ) )
        return false;

    Mat f;

    while( captura.proxima( f ) )
    {
        frame ft( f );
        procesarImagen( ft );
        captura.escribir( ft.imagen() );
        //capacidad de pausar
        char c = waitKey( 5 );
        if( c == 27 )
            for( char c = waitKey( 100 ); true; c = waitKey( 100 ) )
            {
                if( c == 27 )
                    break;
                else if( c == 'q' )
                    return true;
            }
            else if( c == 'q' )
                return true;
    }
    return true;
}
