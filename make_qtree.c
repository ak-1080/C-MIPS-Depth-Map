/*
 * PROJ1-1: YOUR TASK B CODE HERE
 *
 * Feel free to define additional helper functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"
#include "make_qtree.h"
#include "utils.h"

#define ABS(x) (((x) < 0) ? (-(x)) : (x))

qNode *build_quad(unsigned char* depth_map, int x, int y, int map_width, int s_width);

int homogenous(unsigned char *depth_map, int map_width, int x, int y, int section_width) {

    int s_y;
    int s_x;
      
    unsigned char value = depth_map[x+y*map_width];
    for(s_y =0 ; s_y <section_width; s_y++)
        for(s_x =0 ; s_x <section_width; s_x++)      
            if(depth_map[x+s_x+(y+s_y)*map_width] != value)              
                return 256;
                      
       
    return value;
}

qNode *depth_to_quad(unsigned char *depth_map, int map_width) {

    int x, y;
    x = 0;
    y =0;
      
    qNode* quad = (qNode *) malloc(sizeof(qNode));
    if(!quad)
        allocation_failed();
   
    quad = build_quad(depth_map, x,y, map_width, map_width);
    return quad;

}

qNode *build_quad(unsigned char* depth_map, int x, int y, int map_width, int s_width){

   
    if(s_width == 0)
        return NULL;
    qNode* quad = (qNode *) malloc(sizeof(qNode));
    if(!quad)
        allocation_failed();
    int hom = homogenous(depth_map, map_width, x,y, s_width);
    
    if( hom != 256 ){
        quad-> leaf = 1;
        quad-> size = s_width;
        quad->x = x;
        quad-> y = y;
        quad-> gray_value = depth_map[x+y*map_width];
        quad-> child_NW = NULL;
        quad-> child_NE = NULL;
        quad-> child_SE = NULL;
        quad-> child_SW = NULL;
     }
     
     else if(hom == 256){
     
        quad-> leaf = 0;
        quad-> size = s_width;
        quad->x = x;
        quad-> y = y;
        quad-> gray_value = 256;
        quad-> child_NW = build_quad(depth_map, x, y, map_width, s_width/2);
        quad-> child_NE = build_quad(depth_map, x+s_width/2, y, map_width, s_width/2);
        quad-> child_SE = build_quad(depth_map, x+s_width/2, y+s_width/2, map_width, s_width/2);
        quad-> child_SW = build_quad(depth_map, x, y+s_width/2, map_width, s_width/2);
     
     
     }
   
    
     return quad;
    

}

void free_qtree(qNode *qtree_node) {
    if(qtree_node) {
        if(!qtree_node->leaf){
            free_qtree(qtree_node->child_NW);
            free_qtree(qtree_node->child_NE);
            free_qtree(qtree_node->child_SE);
            free_qtree(qtree_node->child_SW);
        }
        free(qtree_node);
    }
}
