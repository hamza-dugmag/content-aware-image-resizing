#include "seamcarving.h"
#include "c_img.h"
#include <math.h>


#define INF 390150  // 6*255^2


void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    // initialize
    create_img(grad, im->height, im->width);

    for (int x = 0; x < im->width; x++)
    {
        for (int y = 0; y < im->height; y++)
        {
            // define
            int right = x + 1;
            int left = x - 1;
            int down = y + 1;
            int up = y - 1;

            // verify
            if (x == 0)
                left = im->width - 1;
            else if (x == im->width - 1)
                right = 0;
            
            if (y == 0)
                up = im->height - 1;
            else if (y == im->height - 1)
                down = 0;

            // assign
            double Rx = get_pixel(im, y, right, 0) - get_pixel(im, y, left, 0);
            double Gx = get_pixel(im, y, right, 1) - get_pixel(im, y, left, 1);
            double Bx = get_pixel(im, y, right, 2) - get_pixel(im, y, left, 2);

            double Ry = get_pixel(im, down, x, 0) - get_pixel(im, up, x, 0);
            double Gy = get_pixel(im, down, x, 1) - get_pixel(im, up, x, 1);
            double By = get_pixel(im, down, x, 2) - get_pixel(im, up, x, 2);

            // calculate
            double energy = sqrt(Rx*Rx + Gx*Gx + Bx*Bx + Ry*Ry + Gy*Gy + By*By);
            uint8_t gradient = (uint8_t) (energy/10);

            // create
            set_pixel(*grad, y, x, gradient, gradient, gradient);
        }
    }
}


void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
    // allocate
    int width = (int) grad->width;
    int height = (int) grad->height;
    *best_arr = (double *) malloc(width * height * sizeof(double));

    // initialize (i, j)
    for (int j = 0; j < width; j++) {
        (*best_arr)[j] = (double) get_pixel(grad, 0, j, 0);
    }

    for (int i = 1; i < height; i++)
        for (int j = 0; j < width; j++)
            (*best_arr)[i*width+j] = INF;

    // update
    for (int i = 1; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            double smallest = (*best_arr)[(i-1)*width+j];
            double left = INF;
            double right = INF;

            if (j == 0 && width != 1)
                right = (*best_arr)[(i-1)*width+(j+1)];

            else if (j == width - 1)
                left = (*best_arr)[(i-1)*width+(j-1)];
            
            else {
                right = (*best_arr)[(i-1)*width+(j+1)];
                left = (*best_arr)[(i-1)*width+(j-1)];
            }

            if (left < smallest)
                smallest = left;
            if (right < smallest)
                smallest = right;
            
            (*best_arr)[i*width+j] = get_pixel(grad, i, j, 0) + smallest;
        }
    }
}


void recover_path(double *best, int height, int width, int **path)
{
    // allocate
    *path = (int *) malloc(height * sizeof(int));
    
    // initialize
    double min = INF;
    double min_j;

    for (int j = 0; j < width; j++) {
        double energy = best[(height-1)*width+j];
        if (energy < min) {
            min = energy;
            min_j = j;
        }
    }

    (*path)[height-1] = min_j;
    int id = 1;

    // trace
    for (int i = height-1; i > 0; i--)
    {
        int candidates[3] = {min_j, min_j, min_j};
        
        if (min_j == 0 && width != 1)
            candidates[1] = min_j + 1;
        else if (min_j == width - 1)
            candidates[1] = min_j - 1;
        else {
            candidates[1] = min_j - 1;
            candidates[2] = min_j + 1;
        }

        int best_j;
        double smallest_energy = INF;

        for (int e = 0; e < 3; e++) {
            double energy = best[(i-1)*width+candidates[e]];
            if (energy < smallest_energy) {
                best_j = candidates[e];
                smallest_energy = energy;
            }
        }

        min_j = best_j;
        (*path)[height-1-id] = min_j;
        id++;
    }
}


void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    create_img(dest, src->height, src->width-1);

    for (int i = 0; i < src->height; i++)
    {
        int removed = 0;
        for (int j = 0; j < src->width-1; j++)
        {
            if (j == path[i])
                removed = 1;
            
            int r = get_pixel(src, i, j+removed, 0);
            int g = get_pixel(src, i, j+removed, 1);
            int b = get_pixel(src, i, j+removed, 2);

            set_pixel(*dest, i, j, r, g, b);
        }
    }
}
