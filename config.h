// defines the style of the resulting render
// 1 -> treat iterations as hue in HSV colour space
// 2 -> bi flag alternating each pixel
// 3 -> greyscale for iterations between 100 and 1000
// 4 -> greyscale for more than 1000 iterations
int8_t style = 0;

// the x and y lengths of the image in numbers of pixels
// as well as the number of threads to use and the number
// of iterations before declaring a value to be in the set
uint32_t xlen = 500,
         ylen = 500,
         iterations = 100,
         threads=8;

// the bottom left (blx, bly) and top right (trx, try) coordinates
// defining the area of the fractal to render
long double blx = -2,
            bly = -2,
            trx =  2,
            try =  2;

// the name of the file to save the image to
char * fname = "mand.ff";
