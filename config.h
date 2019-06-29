// defines the style of the resulting render
// 1 -> treat iterations as hue in HSV colour space
// 2 -> bi flag alternating each pixel
// 3 -> greyscale for iterations between 100 and 1000
// 4 -> greyscale for more than 1000 iterations
#define style 1

// the x and y lengths of the image in numbers of pixels
// as well as the number of threads to use and the number
// of iterations before declaring a value to be in the set
#define xlen 10000
#define ylen 10000
#define iterations 1000

// the number of threads to use when rendering the image
#define threads 8

// the bottom left (blx, bly) and top right (trx, try) coordinates
// defining the area of the fractal to render
#define radius 0.0000221875
#define centre_x -1.762319
#define centre_y 0.022932

const long double blx = centre_x - radius,
                  bly = centre_y - radius,
                  trx = centre_x + radius,
                  try = centre_y + radius;

// the name of the file to save the image to
#define fname "mand.ff"
