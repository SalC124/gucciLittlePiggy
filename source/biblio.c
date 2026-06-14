#include "../include/biblio.h"
#include <3ds/allocator/linear.h>
#include <c3d/buffers.h>
#include <c3d/texenv.h>
#include <c3d/uniforms.h>
#include <string.h>

// ================================= Meshes ====================================
Mesh Mesh_New(const vertex *vbo_data, int count, GPU_Primitive_t mode)
{
    Mesh mesh;

    mesh.stride       = sizeof(vertex);
    int vbo_size      = count * mesh.stride;
    mesh.vbo          = linearAlloc(vbo_size);
    mesh.mode         = mode;
    mesh.vertex_count = count;
    memcpy(mesh.vbo, vbo_data, vbo_size);

    return mesh;
}

void Mesh_Bind(Mesh *mesh)
{
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, mesh->vbo, mesh->stride, 3, 0x210);
    C3D_SetBufInfo(bufInfo);
}

void Mesh_Decimate(Mesh *mesh)
{
    if ((*mesh).vbo != NULL)
    {
        linearFree((*mesh).vbo);
        (*mesh).vbo = NULL;
    }
}

// =============================== Materials ===================================
Material Material_CreateTextured(C3D_Tex *tex, const C3D_Mtx *lighting)
{
    Material m;

    m.type     = MAT_TEXTURED;
    m.texture  = tex;
    m.color    = 0xFFFFFFFF;
    m.lighting = lighting;

    return m;
}

Material Material_CreateColor(u32 rgba, const C3D_Mtx *lighting)
{
    Material m;

    m.type     = MAT_SOLID_COLOR;
    m.texture  = NULL;
    m.color    = rgba;
    m.lighting = lighting;

    return m;
}

void Material_Bind(const Material *m, int uLoc_material)
{
    // material set on a per-object basis
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material, m->lighting);

    C3D_TexEnv *env0 = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env0);
    if (m->type == MAT_TEXTURED)
    {
        C3D_TexBind(0, m->texture);
        // Use texture *modulate* mode
        C3D_TexEnvSrc(env0, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
        C3D_TexEnvFunc(env0, C3D_Both, GPU_MODULATE);
    }
    else
    {
        // Configure TexEnv to completely ignore textures/lighting and use a solid color
        C3D_TexEnvSrc(env0, C3D_Both, GPU_CONSTANT, 0, 0);
        C3D_TexEnvColor(env0, 0xFFFFFFFF); // AABBGGRR
        C3D_TexEnvFunc(env0, C3D_Both, GPU_REPLACE);
    }
    C3D_TexEnv *env1 = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env1);
}
