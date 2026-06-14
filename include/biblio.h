#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <3ds/types.h>

typedef struct
{
    float position[3];
    float texcoord[2];
    float normal[3];
} vertex;

typedef struct
{
    vertex* vertex_list;
    u32 color;
    float x, y, z;
    float rot_x, rot_y, rot_z;

} Drawable3dObject;

// ====================== functions for Drawable3dObjects ======================

#endif
