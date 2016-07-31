/*
 * PROJ1-1: Akram B.
            Ankur K.
 *
 * Feel free to define additional helper functions.
 */

#include "calc_depth.h"
#include "utils.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

//function prototype
unsigned char scan (unsigned char* left, unsigned char* right, int im_width,int im_height,
    int f_x, int f_y, int maximum_displacement, 
    int f_width,  int f_height, int orig_fh, int orig_fw, int orig_x, int orig_y);
    
    
void locate_feature(  int f_width, int f_height, int x, int y, int*f_x, int*f_y, 
    int im_height, int im_width, int* n_height, int * n_width);

/* Implements the normalized displacement function */
unsigned char normalized_displacement(int dx, int dy,
        int maximum_displacement) {

    double squared_displacement = dx * dx + dy * dy;
    double normalized_displacement = round(255 * sqrt(squared_displacement) / sqrt(2 * maximum_displacement * maximum_displacement));
    return (unsigned char) normalized_displacement;

}

void calc_depth(unsigned char *depth_map, unsigned char *left,
        unsigned char *right, int image_width, int image_height,
        int feature_width, int feature_height, int maximum_displacement) {

    int x;
    int y;
   
    int f_x;//new x coordinae for feature
    int f_y;// new y coordinae for feature
    
    int n_height;//new height of feature 
    int n_width;// new height of feature
   
    char n_distanc;
    for(y = 0; y <image_height; y++ )
        for(x =0; x<image_width; x++){ 
        
             locate_feature(feature_width, //call locate feature  on given input
                feature_height, x, y, &f_x, &f_y, image_height, image_width, &n_height, &n_width);
             
             if(maximum_displacement == 0){
                
                depth_map[x+y*image_width] = 0;
             
             }
             else if(f_x == -1 || f_y == -1){
                depth_map[x+y*image_width] = 0;
             }
             else{
             
                n_distanc = scan(left,right,image_width, image_height, f_x, f_y,
                    maximum_displacement, n_width, n_height, feature_height, feature_width, x, y);
                    
                depth_map[x+y*image_width] = n_distanc;
                     
             }
          
      
     }//end for

}


void locate_feature(  int f_width, int f_height, int x, int y, int*f_x, int*f_y, 
    int im_height, int im_width, int* n_height, int * n_width){


    //if feature height is above image threshold then a sentinal value is assigned to feature coordinates
    //this will be checked when pixel coordinates are assigned 
    if(x-f_width < 0 || y-f_height <0 || x+f_width > im_width-1 || y+f_height > im_height-1){
        *f_x = -1;
        *f_y  = -1;
    }
       
    else{
        
        int height = (2*f_height+1);
        int width = (2*f_width+1);
        
        *n_height = height;
        *n_width = width;
      
        
        if(f_width == 0)//if feature width is zero, the f_x is same as original pixel x coordinates
            *f_x = x;
        else
            *f_x = x-f_width;
       
       if(f_height == 0){//if feature height is zero, the f_x is same as original pixel y coordinates
            *f_y = y;

         }
        else
            *f_y = y-f_height;
 
    }
   
}

unsigned char scan (unsigned char* left, unsigned char* right, int im_width,int im_height,
    int f_x, int f_y, int maximum_displacement, 
    int f_width,  int f_height, int orig_fh, int orig_fw, int orig_x, int orig_y){
  
 
    
    int s_x = f_x - maximum_displacement;//x coordinate for search space
    int s_y = f_y - maximum_displacement;//y coordinate for search space
    int low_s_x = orig_x + orig_fw+ maximum_displacement;//corner x coordinate for search space
    int low_s_y = orig_y+ orig_fw+maximum_displacement;   // corner y coordinate for search space
    
    
    
   //if maximum displacement is over image bounds
   //each coordinate is set its respected maximum image coordinate
    if(s_x < 0)
        s_x = 0;
    if(s_y < 0)
        s_y = 0;
    if(low_s_x > im_width-1)
        low_s_x = im_width-1;
    if(low_s_y > im_height-1)
        low_s_y = im_height-1;
        
    int  s_x_width = low_s_x-s_x  +1;//width of the search space
    int s_y_height = low_s_y- s_y +1 ;//height of the search space
     
    //variables to hold the euclidean distance
    unsigned int e_dis =0;
    unsigned int e_dis_cmp = pow(2,31);
    
    //increment for the number of the times the feature
    //will cover the search space both horizontally and vertically
    int x_inc = 0;
    int y_inc = 0;
    
    //bunch of variables
    int i, j;
    int i_x;
    int i_y;
    
  
    //variables to hold the new location of the pixel
    //with the feature of the smallest e_distance
    int x;
    int y;
    
   
        
    while(y_inc != s_y_height-f_height+1 ){//will span the space vertically
        
        x_inc = 0;
       
        while(x_inc!=s_x_width- f_width+1 ){//will span the space horizontally first
            
             e_dis = 0;
            for(i = f_y, i_y=0; i_y<f_height; i_y++ )//compares one feature at a time
                  for(j = f_x, i_x =0; i_x < f_width;  i_x++) { 
              
                     e_dis += pow(left[(j+i_x)+(i+i_y)*im_width] - 
                        right[s_x+x_inc+i_x +(s_y+y_inc+i_y)*im_width],2);                                
            
                  }
                  
              //cases of ties: will not happen at first run.
              if(e_dis == e_dis_cmp){
             
                unsigned char dis_1;
                unsigned char dis_2;
             
                //hold new location of pixel
                int temp_x;
                int temp_y;
                
                if(orig_fh == 0)
                    temp_y =  s_y+y_inc;
                else    
                    temp_y = s_y+y_inc+orig_fh;
          
              
                if(orig_fw == 0)
                    temp_x= s_x+x_inc;
                else
                    temp_x=s_x+x_inc+orig_fw;
            
                //distance 1: old pixel location with smallest feature
                //distance 2: new pixel location 
                //will be compared to see which pixel has the smallest normalized_displacement
                dis_1 = normalized_displacement(abs(orig_x-x), abs(orig_y-y), maximum_displacement);
                dis_2 = normalized_displacement(abs(orig_x-temp_x), abs(orig_y-temp_y),
                    maximum_displacement);
             
                if(dis_2 <= dis_1){
                    
                    x = temp_x;
                    y = temp_y;
                 }
             
    
              }
              if(e_dis < e_dis_cmp){      
                        
                       if(orig_fh == 0)
                            y =  s_y+y_inc;
                        else    
                            y = s_y+y_inc+orig_fh;
                       
                        if(orig_fw == 0)
                            x= s_x+x_inc;
                        else
                            x = s_x+x_inc+orig_fw;
                     
                        e_dis_cmp = e_dis;//assigned the smallest distance to a temporary variable
       
                    }
    
            x_inc++;//number of times each feature was scanned in the space horizontally
        }
        
          
        y_inc++;//number of times each feature was scanned in the space verically
    }
    
    return normalized_displacement(abs(orig_x-x), abs(orig_y-y), maximum_displacement);
 
    
}




