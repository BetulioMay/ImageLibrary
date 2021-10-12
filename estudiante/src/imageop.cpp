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
    for (int i = 0, j = 0; i < n; i+=2, ++j) {

        byte value = this->get_pixel(j, 0);
        zoomedImage.set_pixel(i, 0, value);

        for (int k = 2, l = 1; k < n; k+=2, ++l) {
            // Copiamos el valor
            value = this->get_pixel(j, l);
            zoomedImage.set_pixel(i, k, value);

            byte left = this->get_pixel(j, l-1);
            byte right = this->get_pixel(j, l);

            // Interpolamos por las filas
            byte interpol = (byte)round(((double)left + (double)right) / 2.0);

            zoomedImage.set_pixel(i, k-1, interpol);
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
                // Filas impares
                up = ((double)zoomedImage.get_pixel(i-1, j-1) + (double)zoomedImage.get_pixel(i-1, j+1)) / 2.0;
                down = ((double)zoomedImage.get_pixel(i+1, j-1) + (double)zoomedImage.get_pixel(i+1, j+1)) / 2.0;
            }
            byte value = (byte)round((up + down) / 2.0);
            zoomedImage.set_pixel(i, j, value);
        }
    }

//    // Copia todos los bytes de la porcion de imagen descrita (filas y columnas pares)
//    for(int i = 0, k = row; i < n; i+=2, ++k) {
//        for(int j = 0, l = col; j < n; j+=2, ++l) {
//            byte value = this->get_pixel(k, l);
//            zoomedImage.set_pixel(i, j, value);
//        }
//    }
//
//    // Interpolamos por las columnas (columnas impares)
//    for(int i = 0; i < n; i+=2) {
//        for(int j = 1; j < n; j+=2) {
//            auto left = (double)zoomedImage.get_pixel(i, j-1);
//            auto right = (double)zoomedImage.get_pixel(i, j+1);
//
//            byte value = (byte)((left + right) / 2.0);
//            zoomedImage.set_pixel(i, j, value);
//        }
//    }
//
//    // Interpolamos por las filas (filas impares)
//    for(int i = 1; i < n; i+=2) {
//        for(int j = 0; j < n; ++j) {
//
//            double up;
//            double down;
//
//            // Filas pares
//            if (j % 2 == 0) {
//                up = (double)zoomedImage.get_pixel(i-1, j);
//                down = (double)zoomedImage.get_pixel(i+1, j);
//            } else {
//                // Filas impares
//                up = ((double)zoomedImage.get_pixel(i-1, j-1) + (double)zoomedImage.get_pixel(i-1, j+1)) / 2.0;
//                down = ((double)zoomedImage.get_pixel(i+1, j-1) + (double)zoomedImage.get_pixel(i+1, j+1)) / 2.0;
//            }
//            byte value = (byte)((up + down) / 2.0);
//            zoomedImage.set_pixel(i, j, value);
//        }
//    }

    // Retornamos imagen con zoom x2
    return zoomedImage;
}

void Image::AdjustContrast(byte in1, byte in2, byte out1, byte out2) {
    const auto rate1 = (double)(((double)out1 - 0) / ((double)in1 - 0));
    const auto rate2 = (double)(((double)out2 - (double)out1) / ((double)in2 - (double)in1));
    const auto rate3 = (double)((255 - (double)out2) / (255 - (double)in2));
//    const int rows = this->get_rows();
//    const int cols = this->get_cols();

    byte value = 0;
    byte z = 0;

    for (int k = 0; k < rows*cols; ++k) {
        z = this->get_pixel(k);
        if (z < in1) {
            // El valor esta por debajo del intervalo
            value = (byte)round(0 + (rate1 * ((double)z - 0)));
            this->set_pixel(k, value);
        } else if (z > in2) {
            // El valor esta por encima del intervalo
            value = (byte)round((double)out2 + (rate3 * ((double)z - (double)in2)));
            this->set_pixel(k, value);
        } else {
            // El valor esta dentro del intervalo
            value = (byte)round((double)out1 + (rate2 * ((double)z - (double)in1)));
            this->set_pixel(k, value);
        }
    }
}

void Image::ShuffleRows() {
    const int p = 9973;
    int newr;

    // Creamos un nuevo puntero a punteros
    byte ** n_img = new byte * [rows];

    // Asignamos las filas barajadas de img a n_img
    for (int r = 0; r < this->rows; ++r) {
        newr = (r*p) % this->rows;
        n_img[r] = this->img[newr];
    }

    // Le asignamos a img la nueva imagen con filas barajadas y liberamos memoria
    this->img = n_img;
    n_img = nullptr;
}

Image Image::Subsample(int factor) const {
    if(factor > 0){
        int nf = rows/factor;
        int nc = cols/factor;

        Image image(nf, nc);

        bool factor_fils = (rows % nf == 0);
        bool factor_cols = (cols % nc == 0);

        double prop_fils = rows/nf;
        double prop_cols = cols/nc;

        int comp_fils = (int) nf * (prop_fils - (int) prop_fils);
        int comp_cols = (int) nc * (prop_cols - (int) prop_cols);

        int elem_filcomp = prop_fils;
        int elem_colcomp = prop_cols;

        if(!prop_fils) {
            elem_filcomp++; //Elementos que se cogen de una fila o columna

            if (!prop_cols)
                elem_colcomp++;

            //1º paso para filas completas y columnas completas

            for (int i = 0; i < comp_fils; i++) {
                for (int j = 0; j < comp_cols; j++)
                    image.img[i][j] = (byte) Mean(i * elem_filcomp, j * elem_colcomp, (i + 1) * (elem_filcomp) - 1,
                                                  (j + 1) * (elem_colcomp) - 1);
            }
        }
        //i+1 y j+1 es hasta el lugar donde calcularemos
        //2º paso hasta la columna final y filas enteras

        if(!prop_cols){
            elem_colcomp--;

            for(int i = 0; i < comp_fils; i++){
                for(int j = 0; j < nc; j++)
                    image.img[i][j] = (byte) Mean(i*elem_filcomp, j*(elem_colcomp)+comp_cols,(i+1)*(elem_colcomp)-1,(j+1)*(elem_colcomp)-1+comp_cols);
            }
        }
        //sumamos cols completas porque tenemos que tener en cuenta las columnas procesadas

        //3º zona hasta la fila final y columnas completas
        if(!prop_fils){
            elem_filcomp--;
            if(!prop_cols)
                elem_colcomp++; //Incremento el valor y llegamos a columnas completas
            for(int i = comp_fils; i < nf; i++){
                for(int j = 0; j < comp_cols; j++)
                    image.img[i][j] = (byte) Mean(i*elem_filcomp+comp_fils, j*elem_colcomp,(i+1)*(elem_filcomp)-1+comp_fils, (j+1)*(elem_colcomp)-1);
            }
        }

        //4º zona, hasta la fila final y columna final. Es el resto de la imagen

        if(!prop_cols){
            elem_colcomp--;

            for(int i = comp_fils; i < nf; i++){
                for(int j = comp_cols; j < nc; j++)
                    image.img[i][j] = (byte) Mean(i*elem_filcomp+comp_fils, j*elem_colcomp+comp_cols, (i+1)*(elem_filcomp)-1+comp_fils, (j+1)*(elem_colcomp)-1+comp_cols);
            }
        }

        return image;

    }
    else
        exit(1);
}

double Image::Mean(int row, int col, int height, int width) const {
    int orig_width = this->get_rows();
    int orig_height = this->get_cols();
    double sum = 0;

    for(int i = row; i-row < height && i < orig_height; ++i) {
        for(int j = col; j-col < width && j < orig_width; ++j) {
            sum += (double)this->get_pixel(i, j);
        }
    }

    return sum / (height * width);
}
