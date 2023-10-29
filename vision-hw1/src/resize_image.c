#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, round(x), round(y), c);
}

float offset_coordinate(float scale) {
    return -0.5 + 0.5 * scale;
}

image nn_resize(image im, int w, int h) {
    // Step 1: Create a new image that is w x h and the same number of channels as im
    image new_im = make_image(w, h, im.c);

    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;

    float offset_x = -0.5 + 0.5 * scale_x;
    float offset_y = -0.5 + 0.5 * scale_y;

    // Step 2: Loop over the pixels and map back to the old coordinates
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            for (int c = 0; c < im.c; c ++) {
                float x_new = scale_x * x + offset_x;
                float y_new = scale_y * y + offset_y;
                
                new_im.data[x + y*w + c*w*h] = nn_interpolate(im, x_new, y_new, c);

            }
        }
    }
    return new_im;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int top = floorf(y);
    int bottom = ceilf(y);

    int left = floorf(x);
    int right = ceilf(x);

    float v1 = get_pixel(im, left, top, c);
    float v2 = get_pixel(im, right, top, c);
    float v3 = get_pixel(im, left, bottom, c);
    float v4 = get_pixel(im, right, bottom, c);

    float d1 = bottom - y;
    float d2 = y - top;
    float d3 = x - left;
    float d4 = right - x;

    float q1 = v1 * d1 + v3 * d2;
    float q2 = v2 * d1 + v4 * d2;

    float q = q1 * d4 + q2 * d3;
    return q;
}

image bilinear_resize(image im, int w, int h)
{
    // Step 1: Create a new image that is w x h and the same number of channels as im
    image new_im = make_image(w, h, im.c);

    float scale_x = (float)im.w / w;
    float scale_y = (float)im.h / h;

    float offset_x = -0.5 + 0.5 * scale_x;
    float offset_y = -0.5 + 0.5 * scale_y;

    // Step 2: Loop over the pixels and map back to the old coordinates
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            for (int c = 0; c < im.c; c ++) {
                float x_new = scale_x * x + offset_x;
                float y_new = scale_y * y + offset_y;
                
                new_im.data[x + y*w + c*w*h] = bilinear_interpolate(im, x_new, y_new, c);

            }
        }
    }
    return new_im;
}

