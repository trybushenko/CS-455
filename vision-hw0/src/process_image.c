#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c) {   
    int imC = im.c;
    int imH = im.h;
    int imW = im.w;

    x = (x < 0) ? 0 : x;
    x = (x >= imW) ? imW - 1 : x;
    y = (y < 0) ? 0 : y;
    y = (y >= imH) ? imH - 1 : y;
    c = (c < 0) ? 0 : c;
    c = (c >= imC) ? imC - 1 : c;

    int idx = c * imW * imH + y * imW + x;

    return im.data[idx];
}

void set_pixel(image im, int x, int y, int c, float v) {
    int imW = im.w;
    int imH = im.h;
    int imC = im.c;
    
    if (x < 0 || x >= imW || y < 0 || y >= imH || c < 0 || c >= imC) return;

    int idx = c * imW * imH + y * imW + x;

    im.data[idx] = v;
}

image copy_image(image im) {
    image copy = make_image(im.w, im.h, im.c);
    
    memcpy(copy.data, im.data, sizeof(float)*im.w*im.h*im.c);
    return copy;
}

image rgb_to_grayscale(image im) {
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            gray.data[y * im.w + x] = 0.299 * im.data[y * im.w + x] + 
                                      0.587 * im.data[im.w * im.h + y * im.w + x] + 
                                      0.114 * im.data[2 * im.w * im.h + y * im.w + x];
        }
    }
    return gray;
}

void shift_image(image im, int c, float v) {
    if (c < 0 || c >= im.c) return;

    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            im.data[im.h * im.w * c + y * im.w + x] += v;
        }
    }
}

void clamp_image(image im) {
    for (int i = 0; i < im.c * im.h * im.w; ++i) {
        if (im.data[i] < 0.0) fmax(im.data[i], 0.0);
        else if (im.data[i] > 1.0) fmin(im.data[i], 1.0);
    }
}


// These might be handy
float three_way_max(float a, float b, float c) {
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c) {
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im) {

    float r, g, b, min, max, c, h, s, v;
    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            r = im.data[x + y * im.w];
            g = im.data[x + y * im.w + im.w * im.h];
            b = im.data[x + y * im.w + 2 * im.w * im.h];
            
            max = three_way_max(r, g, b); // this is Value
            min = three_way_min(r, g, b);

            v = max;
            c = max - min;

            if (v == 0) s = 0; // this is Saturation
            else s = c / v; // this is Saturation

            if (c == 0) h = 0;
            else {
                if (v == r) h = (g - b) / c;
                else if (v == g) h = (b - r) / c + 2;
                else h = (r - g) / c + 4;
            }
            h = h / 6;
            if (h < 0) h++;

            im.data[x + y * im.w] = h;
            im.data[x + y * im.w + im.w * im.h] = s;
            im.data[x + y * im.w + 2 * im.w * im.h] = v;
        }
    }
}

void hsv_to_rgb(image im) {
    float h, s, v, c, x, m;
    float r, g, b;

    for (int y = 0; y < im.h; ++y) {
        for (int x = 0; x < im.w; ++x) {
            h = im.data[x + y * im.w];
            s = im.data[x + y * im.w + im.w * im.h];
            v = im.data[x + y * im.w + 2 * im.w * im.h];

            c = v * s;
            x = c * (1 - fabs(fmodf(h * 6, 2) - 1));
            m = v - c;

            if (h < 1) {
                r = c;
                g = x;
                b = 0;
            } else if (h < 2) {
                r = x;
                g = c;
                b = 0;
            } else if (h < 3) {
                r = 0;
                g = c;
                b = x;
            } else if (h < 4) {
                r = 0;
                g = x;
                b = c;
            } else if (h < 5) {
                r = x;
                g = 0;
                b = c;
            } else {
                r = c;
                g = 0;
                b = x;
            }

            im.data[x + y * im.w] = r + m;
            im.data[x + y * im.w + im.w * im.h] = g + m;
            im.data[x + y * im.w + 2 * im.w * im.h] = b + m;
        }
    }
}
