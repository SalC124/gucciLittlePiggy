#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <3ds/gpu/enums.h>
#include <3ds/types.h>
#include <c3d/texture.h>
#include <c3d/types.h>

typedef struct
{
    float position[3];
    float texcoord[2];
    float normal[3];
} vertex;

// ================================= Meshes ====================================

typedef struct
{
    vertex *vbo;
    int vertex_count;
    GPU_Primitive_t mode;
    int stride;
} Mesh;

Mesh Mesh_New(const vertex *vbo_data, int count, GPU_Primitive_t mode);
void Mesh_Bind(Mesh *mesh);
void Mesh_Decimate(Mesh *mesh);

// =============================== Materials ===================================

typedef enum
{
    MAT_TEXTURED,
    MAT_SOLID_COLOR
} MaterialType;

typedef struct
{
    u32 color;               // AABBGGRR
    C3D_Tex *texture;        // NULL if solid color
    C3D_Mtx const *lighting; // lighting behavior; doesnt change
    MaterialType type;       // color or texture
} Material;

Material Material_CreateTextured(C3D_Tex *tex, const C3D_Mtx *lighting);
Material Material_CreateColor(u32 rgba, const C3D_Mtx *lighting);
void Material_Bind(const Material *m, int uLoc_material);

// =========================== Drawable3dObjects ===============================

typedef struct
{
    Mesh *mesh;         // ptr bc meshes can be shared
    Material *material; // same reasoning here

    float x, y, z;
    float rot_x, rot_y, rot_z;
    float sc_x, sc_y, sc_z;
    float pivot_x, pivot_y, pivot_z;

    C3D_Mtx modelView;
} Drawable3dObject;

Drawable3dObject DrawableObject_New(Mesh *mesh, Material *material);
void DrawableObject_UpdateModel(Drawable3dObject *obj);
void DrawableObject_Draw(const Drawable3dObject *obj, const C3D_Mtx *view, int uLoc_modelView, int uLoc_material);

// ================================= Camera ====================================

typedef struct
{
    float x, y, z;
    float rot_x, rot_y, rot_z;
    C3D_Mtx view;
} Camera;

Camera Camera_New();
void Camera_Update(Camera *cam);

#endif
