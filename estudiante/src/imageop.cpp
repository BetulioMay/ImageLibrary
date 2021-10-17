/**
 * @file imageop.cpp
 * @brief Fichero con definiciones para el resto de métodos de la clase Image a hacer por el estudiante
 */

#include <iostream>
#include <cmath>
#include <image.h>

#include <cassert>


// Genera una subimagen de la original
Image Image::Crop(int nrow, int ncol, int height, int width) const {
    // Ancho y alto de la imagen original
    int orig_width = this->get_rows();
    int orig_height= this->get_cols();

    // Inicializamos la imagen recortada
    Image croppedImage(width, height);

    // Para cada pixel iniciando en $nrow y $ncol asignamos el respectivo valor de
    // la imagen original
    for (int i = nrow; (i-nrow) < width && i < orig_width; ++i) {
        for (int j = ncol; (j-ncol) < height && j < orig_height; ++j) {
            byte value = this->get_pixel(i, j);
            croppedImage.set_pixel(i-nrow, j-ncol, value);
        }
    }

    // Retorna una nueva subimagen de la original
    return croppedImage;
}

Image Image::Zoom2X() const {
    int n = (2 * this->get_rows()) - 1;
    Image zoomedImage(n,n);

    // Copiamos valores de la original e interpolamos por las columnas
    for (int i = 0, i_orig = 0; i < n; i+=2, ++i_orig) {

        byte value = this->get_pixel(i_orig, 0);
        zoomedImage.set_pixel(i, 0, value);

        for (int j = 2, j_orig = 1; j < n; j+=2, ++j_orig) {
            // Copiamos el valor
            value = this->get_pixel(i_orig, j_orig);
            zoomedImage.set_pixel(i, j, value);

            // Interpolamos por las filas
            byte left = this->get_pixel(i_orig, j_orig - 1);
            byte right = this->get_pixel(i_orig, j_orig);
            byte interpol = (byte)round(((double)left + (double)right) / 2.0);

            zoomedImage.set_pixel(i, j - 1, interpol);
        }
    }
    // Interpolamos por las filas
    for (int i = 1; i < n; i+=2) {
        for(int j = 0; j < n; ++j) {
            double up;
            double down;

            // Filas pares
            if (j % 2 == 0) {
                up = (double)zoomedImage.get_pixel(i-1, j);
                down = (double)zoomedImage.get_pixel(i+1, j);
            } else {
                // Filas impares, calculamos la interpolacion de los valores de 'arriba' y 'abajo'
                // para evitar perdidas de precision
                up = ((double)zoomedImage.get_pixel(i-1, j-1) + (double)zoomedImage.get_pixel(i-1, j+1)) / 2.0;
                down = ((double)zoomedImage.get_pixel(i+1, j-1) + (double)zoomedImage.get_pixel(i+1, j+1)) / 2.0;
            }
            byte value = (byte)round((up + down) / 2.0);
            zoomedImage.set_pixel(i, j, value);
        }
    }

    // Retornamos imagen con zoom x2
    return zoomedImage;
}

void Image::AdjustContrast(byte in1, byte in2, byte out1, byte out2) {

    // Calculamos las pendientes de las rectas de la funcion a trozos
    const auto slope1 = (double)(((double)out1 - 0) / ((double)in1 - 0));
    const auto slope2 = (double)(((double)out2 - (double)out1) / ((double)in2 - (double)in1));
    const auto slope3 = (double)((255 - (double)out2) / (255 - (double)in2));

    byte value = 0;
    byte z = 0;

    for (int k = 0; k < rows*cols; ++k) {
        z = this->get_pixel(k);
        if (z < in1) {
            // El valor esta por debajo del intervalo
            value = (byte)round(0 + (slope1 * ((double)z - 0)));
            this->set_pixel(k, value);
        } else if (z > in2) {
            // El valor esta por encima del intervalo
            value = (byte)round((double)out2 + (slope3 * ((double)z - (double)in2)));
            this->set_pixel(k, value);
        } else {
            // El valor esta dentro del intervalo
            value = (byte)round((double)out1 + (slope2 * ((double)z - (double)in1)));
            this->set_pixel(k, value);
        }
    }
}

void Image::ShuffleRows() {
    // Implementacion 1 proporsionada por el profesorado
/*
    const int p =  9973  ;
    Image temp(rows,cols);
    int newr;
    for (int r=0; r<rows; r++){
        newr = (r*p) % rows;
        for (int c=0; c<cols;c++)
            temp.set_pixel(r,c,get_pixel(newr,c));
    }
    Copy(temp);
*/
    /*
     * TODO: Change ADT internal representation to put this on work
     */
    // Implementacion 2
    const int p = 9973;
    int newr;

    // Creamos un nuevo puntero a punteros
    byte ** n_img = new byte * [rows];

    // Asignamos las filas barajadas de img a n_img
    for (int r = 0; r < this->rows; ++r) {
        newr = (r*p) % this->rows;
        n_img[r] = this->img[newr];
    }

    // Le asignamos a img la nueva imagen con filas barajadas
    this->img = n_img;
    n_img = nullptr;
}

Image Image::Subsample(int factor) const {
    int n_rows = floor(rows * 1.0 / factor * 1.0), n_cols = floor(cols * 1.0 / factor * 1.0);
    Image icon(n_rows, n_cols);
    byte value;

    for (int i = 0, i_icon = 0; i_icon < n_rows; i+=factor, ++i_icon) {
        for (int j = 0, j_icon = 0; j_icon < n_cols; j+=factor, ++j_icon) {
            // Tomamos submatrices de orden factor x factor y calculamos la media de todos sus elementos
            value = (byte)round(this->Mean(i, j, factor, factor));
            // Asignamos el valor de la media redondeada al valor mas proximo a la imagen icono
            icon.set_pixel(i_icon, j_icon, value);
        }
    }

    // Retornamos la imagen icono
    return icon;
}

double Image::Mean(int row, int col, int height, int width) const {
    double sum = 0;

    for(int i = row; i-row < height; ++i) {
        for(int j = col; j-col < width; ++j) {
            sum += (double)this->get_pixel(i, j);
        }
    }

    return sum / (height * width * 1.0);
}
