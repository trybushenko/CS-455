#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    for (int i = 0; i < im.w * im.h * im.c; i++) {
        sum += im.data[i];
    } 

    for (int i = 0; i < im.w * im.h * im.c; i++) {
        im.data[i] /= sum;
    } 
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);

    for (int i = 0; i < w * w; i++) {
        filter.data[i] = 1.0;
    }

    l1_normalize(filter);

    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c);

    image result, result_;
    float q;
    result = make_image(im.w, im.h, im.c);

    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                q = 0.0;
                int fil_c = 0;
                if (filter.c != 1) fil_c = c;

                for (int filter_x = 0; filter_x < filter.w; filter_x++) {
                    for (int filter_y = 0; filter_y < filter.h; filter_y++) {
                        float value = filter.data[filter_x + filter_y * filter.w + filter.w * filter.h * fil_c];
                        
                        int im_x = x - filter.w / 2 + filter_x;
                        int im_y = y - filter.h / 2 + filter_y;

                        q += get_pixel(im, im_x, im_y, c) * value;
                    }
                }
                result.data[x + y * im.w + im.w * im.h * c] = q;
            }
        }
    }
    
    if (!preserve) {
        result_ = make_image(im.w, im.h, 1);
        for (int w = 0; w < im.w; w++) {
            for (int h = 0; h < im.h; h++) {
                q = 0.0;
                for (int c = 0; c < im.c; c++) {
                    q += result_.data[w + h*im.w + im.w*im.h*c];
                }
                result_.data[w + h*im.w] = q;
            }
        }
        return result_;
    }
    return result;

}

image make_highpass_filter() {
    image filter = make_box_filter(3);

    int filter_vals[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
    for (int i = 0; i < sizeof(filter_vals) / sizeof(filter_vals[0]); i++) {
        filter.data[i] = filter_vals[i];
    }
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_box_filter(3);

    int filter_vals[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
    for (int i = 0; i < sizeof(filter_vals) / sizeof(filter_vals[0]); i++) {
        filter.data[i] = filter_vals[i];
    }
    return filter;
}

image make_emboss_filter()
{
    image filter = make_box_filter(3);

    int filter_vals[] = {-2, -1, 0, -1,  1,  1, 0, 1, 2};
    for (int i = 0; i < sizeof(filter_vals) / sizeof(filter_vals[0]); i++) {
        filter.data[i] = filter_vals[i];
    }
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: we should preserve emboss and sharpen filter, but shouldn't for all others. 

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: clamping image in order to limit a value to a range between a minimum and a maximum value.

image make_gaussian_filter(float sigma)
{
    int size = (int)roundf(sigma * 6) + 1;
    if (size % 2 == 0) size++;
    image kernel = make_box_filter(size);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {

            int center = size / 2;
            float value = 1 / (TWOPI * pow(sigma, 2));
            float power = -(pow(x - center, 2) + pow(y - center, 2)) / (2 * pow(sigma, 2));

            kernel.data[x + y * size] = value * exp(power);
        }
    }

    l1_normalize(kernel);
    return kernel;
}

image add_image(image a, image b)
{
    assert (a.h == b.h && a.w == b.w && a.c == b.c);
    image res_img = make_image(a.w, a.h, a.c);
    for (int x = 0; x < a.w; x++) {
        for (int y = 0; y < a.h; y++) {
            for (int c = 0; c < a.c; c++) {
                int pixel = x + y * a.w + a.w * a.h * c;
                res_img.data[pixel] = a.data[pixel] + b.data[pixel];
            }
        }
    }
    return res_img;
}

image sub_image(image a, image b)
{
    assert (a.h == b.h && a.w == b.w && a.c == b.c);
    image res_img = make_image(a.w, a.h, a.c);
    for (int x = 0; x < a.w; x++) {
        for (int y = 0; y < a.h; y++) {
            for (int c = 0; c < a.c; c++) {
                int pixel = x + y * a.w + a.w * a.h * c;
                res_img.data[pixel] = a.data[pixel] - b.data[pixel];
            }
        }
    }
    return res_img;
}

image make_gx_filter()
{
    image filter = make_box_filter(3);

    int filter_vals[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    for (int i = 0; i < sizeof(filter_vals) / sizeof(filter_vals[0]); i++) {
        filter.data[i] = filter_vals[i];
    }
    return filter;
}

image make_gy_filter()
{
    image filter = make_box_filter(3);

    int filter_vals[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    for (int i = 0; i < sizeof(filter_vals) / sizeof(filter_vals[0]); i++) {
        filter.data[i] = filter_vals[i];
    }
    return filter;
}

void feature_normalize(image im)
{
    float min, max;
    min = INFINITY;
    max = -1.0;

    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                int pixel = x + y * im.w + im.w * im.h * c;
                if (im.data[pixel] > max) max = im.data[pixel];
                if (min > im.data[pixel]) min = im.data[pixel];
            }
        }
    }
    if (max - min != 0) {
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                for (int c = 0; c < im.c; c++) {
                    int pixel = x + y * im.w + im.w * im.h * c;
                    im.data[pixel] = (im.data[pixel] - min) / (max - min);
                }
            }
        }
    }
    else {
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                for (int c = 0; c < im.c; c++) {
                    int pixel = x + y * im.w + im.w * im.h * c;
                    im.data[pixel] = 0;
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image gx = convolve_image(im, gx_filter, 0);
    image gy = convolve_image(im, gy_filter, 0);
    image *ret = calloc(2, sizeof(image));
    ret[0] = make_image(im.w, im.h, 1);
    ret[1] = make_image(im.w, im.h, 1);
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            int pixel = x + y * im.w;
            float magnitude = sqrtf(pow(gx.data[pixel], 2) + pow(gy.data[pixel], 2));
            float direction = atan2(gy.data[pixel], gx.data[pixel]);
            ret[0].data[pixel] = magnitude;
            ret[1].data[pixel] = direction;
        }
    }
    return ret;
}

image colorize_sobel(image im)
{
    image *ret = sobel_image(im);
    feature_normalize(ret[0]);
    feature_normalize(ret[1]);
    image color = make_image(im.w, im.h, 3);
    for (int x = 0; x < color.w; x++) {
        for (int y = 0; y < color.h; y++) {
            for (int c = 0; c < color.c; c++) {
                if (c) color.data[x + y * im.w + im.w * im.h * c] = ret[0].data[x + y * im.w];
                else color.data[x + y * im.w + im.w * im.h * c] = ret[1].data[x + y * im.w];
            }
        }
    }
    hsv_to_rgb(color);
    return convolve_image(color, make_gaussian_filter(1), 1);
}