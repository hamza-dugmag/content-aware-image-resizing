#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>


int main(void)
{
    // // PART 1 =================================================
    // struct rgb_img *im;
    // read_in_img(&im, "6x5.bin");

    // struct rgb_img *grad;
    // calc_energy(im, &grad);
    // print_grad(grad);
    // printf("\n");

    // // PART 2 =================================================
    // double *best_arr;
    // dynamic_seam(grad, &best_arr);

    // for (int i = 0; i < grad->height; i++) {
    //     for (int j = 0; j < grad->width; j++) {
    //         printf("%lf ", best_arr[i*(grad->width)+j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    // // PART 3 =================================================
    // int *path;
    // recover_path(best_arr, grad->height, grad->width, &path);

    // for (int i = 0; i < grad->height; i++) {
    //     printf("%d ", path[i]);
    // }
    // printf("\n");

    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "img.bin");
    
    for(int i = 0; i < 250; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "seams2/img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);

    return 0;
}
