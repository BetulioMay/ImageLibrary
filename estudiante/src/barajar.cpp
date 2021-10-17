//
// Created by Cesar A. Mayora on 25/9/21.
//

/**
 * @file Fichero barajar.cpp, baraja de manera pseudoaleatoria las filas de una imagen
 */

#include <iostream>
#include <cstring>
#include <cstdlib>

#include <image.h>
#include <ctime>
#include <iomanip>

using namespace std;

void test_rows() {
    /*
     * Test diferentes filas
     * Para cada nuevo numero de filas creamos una nueva imagen con dicho numero de filas
     * y un numero de columnas constante.
     * Se medira e informara del tiempo empleado en barajar las filas de la imagen.
     */
    const int ncols = 500;

    clock_t tini;
    clock_t tfin;

    int nrows;
    for (nrows = 1; nrows < 500; ++nrows) {
        Image img(nrows, ncols);

        tini = clock();
        img.ShuffleRows();
        tfin = clock();

        cout << setprecision(10) << "N filas: " << nrows << "\t" << ((double)tfin - (double)tini) / (double)CLOCKS_PER_SEC << " sec" << "\n";
    }

}

void test_cols() {
    /*
     * Test diferentes cols
     * Para cada nuevo numero de columnas creamos una nueva imagen con dicho numero de columnas
     * y un numero de filas constante.
     * Se medira e informara del tiempo empleado en barajar las filas de la imagen.
     */
    const int nfilas = 500;

    clock_t tini;
    clock_t tfin;

    int ncols;
    for (ncols = 0; ncols < 500; ++ncols) {
        Image img(nfilas, ncols);

        tini = clock();
        img.ShuffleRows();
        tfin = clock();

        cout << "N cols: " << ncols << "\t" << ((double)tfin - (double)tini) / (double)CLOCKS_PER_SEC << " sec" << "\n";
    }

}

void test_calls(Image image) {
    /*
     * Test diferente numero de llamadas
     */
    int i = 0;
    clock_t tini;
    clock_t tfin;

    for (int n = 0; n < 1000; n+=100){
        tini = clock();
        while (i < n) {
            image.ShuffleRows();
            i++;
        }
        tfin = clock();
        cout << "N llamadas: " << n << " \t" << (double) (tfin - tini) << " ms" << "\n";
    }
}

int main (int argc, char *argv[]){

    char *origen, *destino; // nombres de los ficheros
    Image image;

    // Comprobar validez de la llamada
    if (argc != 3){
        cerr << "Error: Numero incorrecto de parametros.\n";
        cerr << "Uso: barajar <FichImagenOriginal> <FichImagenDestino>";
        exit (1);
    }

    // Obtener argumentos
    origen  = argv[1];
    destino = argv[2];

    // Mostramos argumentos
    cout << endl;
    cout << "Fichero origen: " << origen << endl;
    cout << "Fichero resultado: " << destino << endl;

    // Leer la imagen del fichero de entrada
    if (!image.Load(origen)){
        cerr << "Error: No pudo leerse la imagen." << endl;
        cerr << "Terminando la ejecucion del programa." << endl;
        return 1;
    }

    // Mostrar los parametros de la Imagen
    cout << endl;
    cout << "Dimensiones de " << origen << ":" << endl;
    cout << "   Imagen   = " << image.get_rows()  << " filas x " << image.get_cols() << " columnas " << "\n";


    int n_test;
    cout << "Choose test (0 to skip): "; cin >> n_test;
    switch(n_test) {
        case 0:
            cout << "Continuing with program..." << "\n";
            break;
        case 1:
            test_rows();
            break;
        case 2:
            test_cols();
            break;
        case 3:
            test_calls(image);
            break;
        default:
            cout << "Wrong test number." << "\n";
            cout << "Continuing with program" << "\n";
            break;
    }

    // Barajar la imagen
    image.ShuffleRows();

    // Guardar la imagen resultado en el fichero
    if (image.Save(destino))
        cout  << "La imagen se guardo en " << destino << endl;
    else{
        cerr << "Error: No pudo guardarse la imagen." << endl;
        cerr << "Terminando la ejecucion del programa." << endl;
        return 1;
    }

    return 0;
}
