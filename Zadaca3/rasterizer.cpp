#include <iostream>
#include <cmath>
#include "tgaimage.h"
#include <algorithm>
using namespace std;

// compile with:
// g++ rasterizer.cpp tgaimage.cpp -o out.exe

void set_color(int x, int y, TGAImage &image, TGAColor color, bool invert = false)
{
    image.set(y, x, color);    
}

float line(float x0, float y0, float x1, float y1, float x, float y)
{
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void line_naive(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0; t < 1; t += 0.01)
    {
        int x = x0 * (1.0f - t) + x1 * t;
        int y = x0 * (1.0f - t) + y1 * t;
        set_color(x, y, image, color);
    }
}

void line_midpoint(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, bool invert)
{
    int y = y0;
    int d = line(x0, y0, x1, y1, x0 + 1, y0 + 0.5);
    
    int dx = (x1 - x0);
    int dy = (y1 - y0);
    int increment = 1;
    
    if (dy < 0)
    {
        // pravac ide od gore prema dolje
        dy = -dy;
        increment = -increment;
    }
    
    for (int x = x0; x <= x1; ++x)
    {
        if (invert)
        {
            set_color(x, y, image, color);
        }
        else
        {
            set_color(y, x, image, color);       
        }
        
        if (d < 0)
        {
            y = y + increment;
            d = d + dx - dy;
        }
        else
        {
            d = d - dy;
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 'transponiraj' duzinu ako je veci od 1
    bool invert = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        swap(x0, y0);
        swap(x1, y1);
        invert = true;
    }
    
    // zamijeni tocke ako pravac ide s desna na lijevo
    if (x1 < x0)
    {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    // nacrtaj duzinu
    line_midpoint(x0, y0, x1, y1, image, color, invert);
}

//PRVI ZADATAK

float det(float xa,float ya,float xc,float yc,float x,float y) 
{
    return ((ya-yc)*x+(xc-xa)*y+xa*yc-xc*ya);
}

void draw_triangle_2d(float x0, float y0, float x1, float y1, float x2, float y2 ,TGAImage &image,TGAColor color) {   

    //racunamo bounding box unutar kojeg se nalazi trokut
    int maxX = ceil (max (max(x0,x1), x2));
    int minX = floor (min (min(x0,x1), x2));

    int maxY = ceil (max (max(y0,y1), y2));
    int minY = floor(min (min(y0,y1), y2));
   
   //baricentricne kordinate
    float alpha,beta,gama;

    //ako se nalazi unutar trokuta, oboji taj piksel...provjera za sve piksele bounding
    for (int j =minY; j <= maxY ; ++j){
        for (int i = minX; i <= maxX; ++i){
            
            alpha = det(x1,y1,x2,y2,i,j)/det(x1,y1,x2,y2,x0,y0); 
            beta= det(x2,y2,x0,y0,i,j)/det(x2,y2,x0,y0,x1,y1);  
            gama= det(x0,y0,x1,y1,i,j)/det(x0,y0,x1,y1,x2,y2);  
             
            if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gama >= 0 && gama <= 1) 
            { 
                if (( alpha > 0 ||((det(x1,y1,x2,y2,x0,y0))*(det(x1,y1,x2,y2,-1,-1))) >= 0)
                 &&( beta > 0 || ((det(x2,y2,x0,y0,x1,y1))*(det(x2,y2,x0,y0,-1,-1))) >= 0)
                 &&( gama > 0 || (det(x0,y0,x1,y1,x2,y2))*(det(x0,y0,x1,y1,-1,-1))) >= 0)
                 {
                        image.set(i, j, color);
                }  
            }
        }
    } 
}

//DRUGI ZADATAK

void draw_triangle_2d_gouraurd(TGAImage& image, float x0, float y0, TGAColor c0, float x1, float y1, TGAColor c1, float x2, float y2, TGAColor c2)
{
    //bounding box
    float xmin = min(min(floor(x0), floor(x1)), floor(x2));
    float xmax = max(max(ceil(x0), ceil(x1)), ceil(x2));

    float ymin = min(min(floor(y0), floor(y1)), floor(y2));
    float ymax = max(max(ceil(y0), ceil(y1)), ceil(y2));

    //for petlja...provjerava nalazi li se piksel u trokutu...zatim izracun intenziteta boje za dani piksel
    for (float y = ymin; y <= ymax; ++y) {
        for (float x = xmin; x <= xmax; ++x) {
            float alpha = line(x1, y1, x2, y2, x, y) / line(x1, y1, x2, y2, x0, y0);
            float beta = line(x2, y2, x0, y0, x, y) / line(x2, y2, x0, y0, x1, y1);
            float gamma = line(x0, y0, x1, y1, x, y) / line(x0, y0, x1, y1, x2, y2);

            if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1 && gamma > 0 && gamma < 1) {
                set_color(x, y, image, TGAColor((alpha * c0.r + beta * c1.r + gamma * c2.r), (alpha * c0.g + beta * c1.g + gamma * c2.g), (alpha * c0.b + beta * c1.b + gamma * c2.b), 255));
            }
        }
    }
}

//TRECI ZADATAK

// dimenzije slike
const int width  = 512;
const int height = 512;

void draw_triangle(TGAImage& image, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, TGAColor color) {

    x0 /= z0;
    y0 /= z0;
    x1 /= z1;
    y1 /= z1;
    x2 /= z2;
    y2 /= z2;

    x0 = (1 + x0) * 0.5 * width;
    x1 = (1 + x1) * 0.5 * width;
    x2 = (1 + x2) * 0.5 * width;

    y0 = (1 + y0) * 0.5 * height;
    y1 = (1 + y1) * 0.5 * height;
    y2 = (1 + y2) * 0.5 * height;

    float xmin = min(min(floor(x0), floor(x1)), floor(x2));
    float xmax = max(max(ceil(x0), ceil(x1)), ceil(x2));

    float ymin = min(min(floor(y0), floor(y1)), floor(y2));
    float ymax = max(max(ceil(y0), ceil(y1)), ceil(y2));

    for (float i = ymin; i <= ymax; ++i) {
        for (float j = xmin; j <= xmax; ++j) {
            float alpha = det(x1, y1, x2, y2, j, i) / det(x1, y1, x2, y2, x0, y0);
            float beta = det(x2, y2, x0, y0, j, i) / det(x2, y2, x0, y0, x1, y1);
            float gamma = det(x0, y0, x1, y1, j,i) / det(x0, y0, x1, y1, x2, y2);

            if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1 && gamma > 0 && gamma < 1) {
                set_color(j, i, image, color);
            }
        }
    }
}

// CETVRTI ZADATAK

void draw_triangle_tex(TGAImage& image, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float u0, float v0, float u1, float v1, float u2, float v2, const char* path)
{
    TGAImage texture;
    texture.read_tga_file(path);

    x0 /= z0;
    y0 /= z0;
    x1 /= z1;
    y1 /= z1;
    x2 /= z2;
    y2 /= z2;

    x0 = (1 + x0) * 0.5 * width;
    x1 = (1 + x1) * 0.5 * width;
    x2 = (1 + x2) * 0.5 * width;

    y0 = (1 + y0) * 0.5 * height;
    y1 = (1 + y1) * 0.5 * height;
    y2 = (1 + y2) * 0.5 * height;

    float xmin = min(min(floor(x0), floor(x1)), floor(x2));
    float xmax = max(max(ceil(x0), ceil(x1)), ceil(x2));

    float ymin = min(min(floor(y0), floor(y1)), floor(y2));
    float ymax = max(max(ceil(y0), ceil(y1)), ceil(y2));

    for (float y = ymin; y <= ymax; ++y) {
        for (float x = xmin; x <= xmax; ++x) {
            float alpha = line(x1, y1, x2, y2, x, y) / line(x1, y1, x2, y2, x0, y0);
            float beta = line(x2, y2, x0, y0, x, y) / line(x2, y2, x0, y0, x1, y1);
            float gamma = line(x0, y0, x1, y1, x, y) / line(x0, y0, x1, y1, x2, y2);

            if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1 && gamma > 0 && gamma < 1) {
                int s = round(texture.get_width() * (alpha * u0 + beta * u1 + gamma * u2));
                int t = round(texture.get_height() * (alpha * v0 + beta * v1 + gamma * v2));
                set_color(x, y, image, texture.get(s, t));
            }
        }
    }
}

void draw_triangle_tex_corrected(TGAImage& image, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float u0, float v0, float u1, float v1, float u2, float v2, const char* path)
{
    TGAImage texture;
    texture.read_tga_file(path);

    x0 /= z0;
    y0 /= z0;
    x1 /= z1;
    y1 /= z1;
    x2 /= z2;
    y2 /= z2;

    x0 = (1 + x0) * 0.5 * width;
    x1 = (1 + x1) * 0.5 * width;
    x2 = (1 + x2) * 0.5 * width;

    y0 = (1 + y0) * 0.5 * height;
    y1 = (1 + y1) * 0.5 * height;
    y2 = (1 + y2) * 0.5 * height;

    u0 /= z0;
    u1 /= z1;
    u2 /= z2;

    v0 /= z0;
    v1 /= z1;
    v2 /= z2;

    z0 = 1 / z0;
    z1 = 1 / z1;
    z2 = 1 / z2;

    float xmin = min(min(floor(x0), floor(x1)), floor(x2));
    float xmax = max(max(ceil(x0), ceil(x1)), ceil(x2));

    float ymin = min(min(floor(y0), floor(y1)), floor(y2));
    float ymax = max(max(ceil(y0), ceil(y1)), ceil(y2));

    for (float y = ymin; y <= ymax; ++y) {
        for (float x = xmin; x <= xmax; ++x) {
            float alpha = line(x1, y1, x2, y2, x, y) / line(x1, y1, x2, y2, x0, y0);
            float beta = line(x2, y2, x0, y0, x, y) / line(x2, y2, x0, y0, x1, y1);
            float gamma = line(x0, y0, x1, y1, x, y) / line(x0, y0, x1, y1, x2, y2);

            if (alpha > 0 && alpha < 1 && beta > 0 && beta < 1 && gamma > 0 && gamma < 1) {
                float s = texture.get_width() * (alpha * u0 + beta * u1 + gamma * u2);
                float t = texture.get_height() * (alpha * v0 + beta * v1 + gamma * v2);
                float z = 1 / (alpha * z0 + beta * z1 + gamma * z2);
                s = s * z;
                t = t * z;
                set_color(x, y, image, texture.get(round(s), round(t)));
            }
        }
    }
}

// definirajmo boje
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);
const TGAColor yellow = TGAColor(255,255,0,255);

int main()
{
    // definiraj sliku
    TGAImage task1(width, height, TGAImage::RGB);
    TGAImage task2(width, height, TGAImage::RGB);
    TGAImage task3(width, height, TGAImage::RGB);
    TGAImage task4(width, height, TGAImage::RGB);
    TGAImage task4_1(width,height,TGAImage::RGB);

    // Prvi zadatak
    draw_triangle_2d(50, 100, 200, 300, 400, 100, task1, red);
    draw_triangle_2d(300, 300, 450, 450, 100, 450, task1, blue);
    task1.flip_vertically();
    task1.write_tga_file("task1.tga");

    //Drugi zadatak
    draw_triangle_2d_gouraurd(task2, 20, 30, red, 460, 280, yellow, 100, 500, blue);
    task2.flip_vertically();
    task2.write_tga_file("task2.tga");

    // Treci zadatak
    draw_triangle(task3, -48, -10, 82, 29, -15, 44, 13, 34, 114, blue);
    task3.flip_vertically();
    task3.write_tga_file("task3.tga");

    // trokuti s teksturom
    draw_triangle_tex(task4, -48, -10, 82, 29, -15, 44, 13, 34, 114, 0, 0, 0, 1, 1, 0, "cobble.tga");
    task4.flip_vertically();
    task4.write_tga_file("task4a.tga");

    draw_triangle_tex_corrected(task4_1, -48, -10, 82, 29, -15, 44, 13, 34, 114, 0, 0, 0, 1, 1, 0, "cobble.tga");
    task4_1.flip_vertically();
    task4_1.write_tga_file("task4b.tga");
    
}