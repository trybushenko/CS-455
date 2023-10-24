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
        if (im.data[i] < 0.0) im.data[i] = 0.0;
        else if (im.data[i] > 1.0) im.data[i] = 1.0;
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

void hsv_to_rgb(image im)
{
    float h, s, v, c, max, min, r, g, b, h_temp;
    for (int x = 0; x < im.w; x++) {
	for (int y = 0; y < im.h; y++) {
	    h = im.data[x + y*im.w];
	    s = im.data[x + y*im.w + im.w*im.h];
	    v = im.data[x + y*im.w + 2*im.w*im.h];
	    c = s * v;
	    max = v;
	    if (v != c) {
		min = v - c;
	    }	    	
	    else {
	    	min = 0.0;
	    }
	    h_temp = h * 6;
	    if (c == 0) {
	    	r = v;
	    	g = v;
	    	b = v;
	    }
	    else if (h_temp > 5 && h_temp < 6) {
	    	r = max;
	    	g = min;
	    	b = ((((h_temp /  6) - 1) * 6 * c) - g) * -1;
	    }
	    else if (h_temp == 5) {
	    	r = max;
	    	g = min;
	    	b = max;
	    }
            else if (h_temp < 5 && h_temp > 4) {
	    	g = min;
	    	r = (h_temp - 4) * c + g;
	    	b = max;
	    }
	    else if (h_temp == 4) {
	    	r = min;
	    	g = min;
	    	b = max;
	    }
	    else if (h_temp < 4 && h_temp > 3) {
	    	r = min;
	    	g = (((h_temp - 4) * c) - r) * -1;
	    	b = max;
	    }
            else if (h_temp == 3) {
	    	r = min;
	    	g = max;
	    	b = max;
	    }
            else if (h_temp < 3 && h_temp > 2) {
	    	r = min;
	    	g = max;
	    	b = ((h_temp - 2) * c) + r;
	    }
	    else if (h_temp == 2) {
	    	r = min;
	    	g = max;
	    	b = min;
	    }
            else if (h_temp < 2 && h_temp > 1) {
	    	g = max;
	    	b = min;
	    	r = (((h_temp - 2) * c) - b) * -1;
	    }
            else if (h_temp == 1) {
	    	r = max;
	    	g = max;
	    	b = min;
	    }
             else if (h_temp < 1 && h_temp > 0) {
	    	r = max;
	    	b = min;
	    	g = (h_temp * c) + b;
	    }
            else {
	    	r = max;
	    	g = min;
	    	b = min;
	    }
	    im.data[x + y*im.w] = r;
	    im.data[x + y*im.w + im.w*im.h] = g;
	    im.data[x + y*im.w + 2*im.w*im.h] = b;
	}
    }
}

void scale_image(image im, int c, float v) {
    if (c >= 0 && c < im.c) {
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                im.data[x + y * im.w + c * im.w * im.h] *= v;
            }
        }
    }
}