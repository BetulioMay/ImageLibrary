/**
 * @file image.cpp
 * @brief Fichero con definiciones para los métodos primitivos de la clase Image
 *
 */

#include <cstring>
#include <cassert>
#include <iostream>

#include <image.h>
#include <imageIO.h>
#include <cmath>

using namespace std;

/********************************
      FUNCIONES PRIVADAS
********************************/
void Image::Allocate(int nrows, int ncols, byte * buffer){
    rows = nrows;
    cols = ncols;

    img = new byte * [rows];

    if (buffer != 0)
        img[0] = buffer;
    else
        img[0] = new byte [rows * cols];

    for (int i=1; i < rows; i++)
        img[i] = img[i-1] + cols;
}

// Función auxiliar para inicializar imágenes con valores por defecto o a partir de un buffer de datos
void Image::Initialize (int nrows, int ncols, byte * buffer){
    if ((nrows == 0) || (ncols == 0)){
        rows = cols = 0;
        img = 0;
    }
    else Allocate(nrows, ncols, buffer);
}

// Función auxiliar para copiar objetos Imagen

void Image::Copy(const Image & orig){
    Initialize(orig.rows,orig.cols);
    for (int k=0; k<rows*cols;k++)
        set_pixel(k,orig.get_pixel(k));
}

// Función auxiliar para destruir objetos Imagen
bool Image::Empty() const{
    return (rows == 0) || (cols == 0);
}

void Image::Destroy(){
    if (!Empty()){
        delete [] img[0];
        delete [] img;
    }
}

LoadResult Image::LoadFromPGM(const char * file_path){
    if (ReadImageKind(file_path) != IMG_PGM)
        return LoadResult::NOT_PGM;

    byte * buffer = ReadPGMImage(file_path, rows, cols);
    if (!buffer)
        return LoadResult::READING_ERROR;

    Initialize(rows, cols, buffer);
    return LoadResult::SUCCESS;
}

/********************************
       FUNCIONES PÚBLICAS
********************************/

// Constructor por defecto

Image::Image(){
    Initialize();
}

// Constructores con parámetros
Image::Image (int nrows, int ncols, byte value){
    Initialize(nrows, ncols);
    for (int k=0; k<rows*cols; k++) set_pixel(k,value);
}

bool Image::Load (const char * file_path) {
    Destroy();
    return LoadFromPGM(file_path) == LoadResult::SUCCESS;
}

// Constructor de copias

Image::Image (const Image & orig){
    assert (this != &orig);
    Copy(orig);
}

// Destructor

Image::~Image(){
    Destroy();
}

// Operador de Asignación

Image & Image::operator= (const Image & orig){
    if (this != &orig){
        Destroy();
        Copy(orig);
    }
    return *this;
}

// Métodos de acceso a los campos de la clase

int Image::get_rows() const {
    return rows;
}

int Image::get_cols() const {
    return cols;
}

int Image::size() const{
    return get_rows()*get_cols();
}

// Métodos básicos de edición de imágenes
void Image::set_pixel (int i, int j, byte value) {
    img[i][j] = value;
}
byte Image::get_pixel (int i, int j) const {
    return img[i][j];
}

// This doesn't work if representation changes
void Image::set_pixel (int k, byte value) {
    // TODO this makes assumptions about the internal representation
    // TODO Can you reuse set_pixel(i,j,value)?
    img[0][k] = value;
}

// This doesn't work if representation changes
byte Image::get_pixel (int k) const {
    // TODO this makes assumptions about the internal representation
    // TODO Can you reuse get_pixel(i,j)?
    return img[0][k];
}

// Métodos para almacenar y cargar imagenes en disco
bool Image::Save (const char * file_path) const {
    // TODO this makes assumptions about the internal representation
    byte * p = img[0];
    return WritePGMImage(file_path, p, rows, cols);
}

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

Image Image::Zoom2X(int row, int col, int size) const {
    int n = 2 * size - 1;
    Image zoomedImage(n,n);

    // Interpolamos por las columnas
    for (int i = 0, j = row; j-row < size; i+=2, ++j) {
        for (int k = 0, l = col; l-col < size; k+=2, ++l) {
            byte value = this->get_pixel(j, l);
            zoomedImage.set_pixel(i, k, value);

            if (k > 0) {
                byte left = this->get_pixel(j, l-1);
                byte right = this->get_pixel(j, l);

                byte val_interpol = (byte)round((left + right) / 2);

                zoomedImage.set_pixel(i, k-1, val_interpol);
            }
        }
    }

    // Interpolamos por las filas
    for (int i = 1; i < n; i+=2) {
        for(int j = 0; j < n; ++j) {
            byte up = zoomedImage.get_pixel(i-1, j);
            byte down = zoomedImage.get_pixel(i+1, j);

            byte val_interpol = (byte)round((up + down) / 2);

            zoomedImage.set_pixel(i, j, val_interpol);
        }
    }


//    for(int i = 0, k = row; i < n; i+=2, ++k) {
//        for(int j = 0, l = col; j < n; j+=2, ++l) {
//            zoomedImage.set_pixel(i, j, this->get_pixel(k, l));
//        }
//    }
//
//    for(int i = 0; i < n; i+=2) {
//        for(int j = 1; j < n; j+=2) {
//            byte value = (zoomedImage.get_pixel(i, j-1) + zoomedImage.get_pixel(i, j+1)) / 2;
//            zoomedImage.set_pixel(i, j, value);
//        }
//    }
//
//    for(int i = 1; i < n; i+=2) {
//        for(int j = 0; j < n; ++j) {
//            byte value = (zoomedImage.get_pixel(i-1, j) + zoomedImage.get_pixel(i+1, j)) / 2;
//            zoomedImage.set_pixel(i, j, value);
//        }
//    }

    return zoomedImage;
}

void Image::AdjustContrast(byte in1, byte in2, byte out1, byte out2) {
    const double rate = static_cast<double>((out2 - out1) / (in2 - in1));
    const int rows = this->get_rows();
    const int cols = this->get_cols();

    byte value = 0;
    byte z = 0;

    for (int k = 0; k < rows*cols; ++k) {
        z = this->get_pixel(k);
        if (z <= in2 && z >= in1) {
            value = (byte)round(out1 + (rate * (z - in1)));
            this->set_pixel(k, value);
        }
    }
}

double Image::Mean(int row, int col, int height, int width) const {
    int orig_width = this->get_rows();
    int orig_height = this->get_cols();
    double sum;

    for(int i = row; i-row < height && i < orig_height; ++i) {
        for(int j = col; j-col < width && j < orig_width; ++j) {
            sum += this->get_pixel(i, j);
        }
    }

    return sum / (height * width);
}
