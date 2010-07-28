
La funcion "cvCreateFileCapture()" regresa nulo sin ninguna advertencia ni error.
http://software.intel.com/en-us/forums/showthread.php?t=61169

Ciertas imagenes arrojan un segmentation fault. Crei que era por el tamaño, pero
reduje 2.jpg hasta un tamaño 4.jpg ( que funciona como es el caso de 1.jpg ),
pero el error sigue ahi. Tal vez sea la compresion o algo.

--------------------------------------------------------------------------------

Ahora se usa la api 2.1 de C++ que es mas sencilla para usar. Las imagenes y
videos que probe ya no dan segmentation fault. Hay cosas por corregir, el texto
no esta bien detectado.

Creo que este error esta en la linea 81, funcion procesarFrame(), cuando se
revisa el valor de cada pixel de la imagen.

La referencia de la api 2.1 de C++ esta en
http://opencv.willowgarage.com/documentation/cpp/index.html

--------------------------------------------------------------------------------

Para un mejor trabajo dividi el proyecto en tres clases( 7 archivos ).

El archivo lena.cpp contiene la funcion main y se encarga de recivir los
parametros para ver que hacer, contiene un objeto de tipo proceso.

La clase proceso se encarga de procesar la informacion que le llega a la funcion
main, procesa una imagen o un video segun sea el caso. Esta en los archivos
proceso.h y proceso.cpp.

La clase frame representa una imagen, realiza algunas operaciones como la convolucion
y tiene un objeto de tipo Mat. Esta en los archivos frame.h y frame.cpp.

La clase video representa un video y le realiza algunas operaciones todavia no
definidas. Esta en los archivos video.h y video.cpp.

*Nota: todavia no estan implementadas bien las clases.

--------------------------------------------------------------------------------

Cambie los include de OpenCV a

    #include <opencv/cv.h>
    #include <opencv/highgui.h>
    #include <opencv/ml.h>

Para compilar uso la siguiente instruccion

g++ -Wall -O2 -o ocr lena.cpp proceso.cpp frame.cpp video.cpp -lcxcore -lcv -lcvaux -lhighgui -lml
--------------------------------------------------------------------------------

Algoritmo

El algoritmo se basa en que los caracteres estan compuestos de bordes. Se examina la densidad
y fuerza de los bordes que se detectan en la imagen para decidir si es texto.

Parte   I - Detectar regiones candidatas a ser texto.

            El resultado es una imagen en escala de grises donde la intensidad del pixel
            representa la probabilidad de texto.

                - Filtro direccional.

                    Calcular la intensidad de los bordes ( edge intensity ) aplicando
                    la convolucion con un operador de compas ( compass operator ). Se
                    obtienen cuatro imagenes que muestran la intensidad de los bordes.

                - Seleccion de aristas.

                    Agrupar los bordes encontrados en el paso anterior en bordes largos
                    y bordes cortos. Eliminar los bordes extremadamente largos.

                    Un thresholding de la imagen opbtenida con el operador de 90 grados,
                    obtenemos una imagen con bordes largos que estan cortados ( por la
                    iluminacion, reflejos ).

                    Para eliminar los bordes cortados se usa un metodo de dos pasos.

                        - Obtener bordes fuertes de la imagen de 90 grados.
                            Edge( strong, 90bw ) = | E( 90 ) |( z )
                          donde | . |( z ) es el operador para realizar un thresholding para
                          obtener una imagen binaria con el metodo de otsu.

                        - Obtener bordes debiles de la imagen de 90 grados.
                            dilated = Dilation( Edge( strong, 90bw ) )( 1x3 )
                            closed = Closing( dilated )( mx1 )
                            Edge( weak, 90 ) = | E( 90 ) x ( closed - dilated ) |( z )

                            donde:
                                dilation: morphological dilation con un elemento rectangular
                                          de 1 x 3.
                                closed: closing operator con un vertical linear structuring
                                        de m x 1.
                                m = ( 1 / 25 ) x ancho de la imagen.

                    Posteriormente

                        Edge( 90bw ) = Edge( weak, 90bw ) + Edge( strong, 90bw )

                        thined = Thinning( Edge( 90bw ) )
                        labeled = BWLabel( thined, 4 )

                        donde:
                            Thinning - Morphological thinning operator.
                                Hace los bordes verticales de 1 pixel de ancho.
                            BWLabel - Connected component labeling.
                                Pone etiquetas a los componentes conectados por sus
                                cuatro vecinos y los identifica y su numero de elementos.

                    Se realiza el algoritmo 3.1 de la pagina 275 para que la intensidad de cada borde
                    refleje su tamano.

                    Separar los bordes largos con un threashold

                        short( 90bw ) = | E( lengthlabeled, 90 ) |( z )

                        donde | . |( z ) es un threashold binario inverso.

                - Generacion de mapas de caracteristicas.

                    candidate = Dilation( short( 90bw ) )mxm

                    refined = candidate x ( E( 0 ) + E( 45 ) + E( 90 ) + E( 135 ) )

                    feature map( i, j ) = ecuacion 10, pagina 276

Parte  II - Localizacion de regiones de texto.
                - Agrupacion de regiones de texto.
                - Filtro heuristico.
                - Generacion de cajas de limites.
Parte III - Extraccion de los caracteres.
                - Adaptar la imagen para un ocr.

--------------------------------------------------------------------------------

Estuve revisando mas a fondo el algoritmo descrito en el documento y lo que hacemos en el codigo
del programa.

Observaciones:

    - La primer parte, donde se ejecuta la convolucion parece no estar implementada como esta
    descrito en el documento, le faltan unas partes.
    - Cuando se marcan los puntos de interes estos no parecen tener importancia sobre el resultado
    p imagen final. La imagen final se ve mas como el resultado de aplicar las funciones canny y blur
    que precisamente lo que hacen es detectar bordes y como las letras de las pruebas realizadas
    son bordes "parece" que si esta funcionando aunque no es completamente verdad esto ya que falla
    en imagenes donde los colores entre las letras y el fondo no son muy distintos( blanco y cafe claro ).

Agregue soporte para guardar video en un archivo y para procesar video desde la camara por default del sistema
como una webcam, para guardar el video solo soporta por el momento una resolucion de 480 x 640 y los fps tienen
que ser mas de 20.0 en formato mpv1( mpeg ).

Realize algunas pruebas y subi un video a youtube http://www.youtube.com/watch?v=1Uf_ISEScWM

Es necesario inspeccionar el algoritmo descrito en el documento y realizar otro documento que describa mas a detalle
los pasos que se deben realizar y por que se deben realizar para poder mejorar la calidad del programa.
