#include "../include/biblio.h"
#include <3ds/allocator/linear.h>
#include <c3d/buffers.h>
#include <string.h>

// ================================= Meshes ====================================
Mesh Mesh_New(const vertex *vbo_data, int count, GPU_Primitive_t mode)
{
    Mesh mesh;

    mesh.stride  = sizeof(vertex);
    int vbo_size = count * mesh.stride;
    mesh.vbo     = linearAlloc(vbo_size);
    memcpy(mesh.vbo, vbo_data, vbo_size);
    mesh.mode         = mode;
    mesh.vertex_count = count;

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
    Material material;
}

Material Material_CreateColor(u32 rgba, const C3D_Mtx *lighting)
{
    Material material;
}
