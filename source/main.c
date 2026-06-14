#include "sal_t3x.h"
#include "vshader_shbin.h"
#include <3ds.h>
#include <3ds/gpu/enums.h>
#include <3ds/services/hid.h>
#include <c3d/buffers.h>
#include <c3d/texenv.h>
#include <citro3d.h>
#include <string.h>
#include <tex3ds.h>

#include "../include/biblio.h"

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS                                                                                         \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |                                   \
     GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                     \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

static vertex *cube_vbo_data;
static const vertex cube_vertex_list[] = {
    // First face (PZ)
    // First triangle
    {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f}},
    {{+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f}},
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}},
    // Second triangle
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}},
    {{-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f}},
    {{-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f}},

    // Second face (MZ)
    // First triangle
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
    {{+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    // Second triangle
    {{+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

    // Third face (PX)
    // First triangle
    {{+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}},
    {{+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}},
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}},
    // Second triangle
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}},
    {{+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}},

    // Fourth face (MX)
    // First triangle
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    // Second triangle
    {{-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

    // Fifth face (PY)
    // First triangle
    {{-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f}},
    {{-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f}},
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}},
    // Second triangle
    {{+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}},
    {{+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f}},
    {{-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f}},

    // Sixth face (MY)
    // First triangle
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    {{+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    // Second triangle
    {{+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
};
#define CUBE_VERTEX_LIST_COUNT (sizeof(cube_vertex_list) / sizeof(cube_vertex_list[0]))

#define CIRCLE_SEGMENTS        30
static vertex *circle_vbo_data;
#define CIRCLE_VERTEX_COUNT (CIRCLE_SEGMENTS + 2) // Center + segments + closing vertex

static vertex *generatePolygon(int vertices, float radius)
{
    vertex *data = (vertex *)linearAlloc(sizeof(vertex) * (vertices + 2));

    // Focus
    data[0].position[0] = 0.0f;
    data[0].position[1] = 0.0f;
    data[0].position[2] = 0.0f;

    data[0].texcoord[0] = 0.5f;
    data[0].texcoord[1] = 0.5f;

    data[0].normal[0] = 0.0f;
    data[0].normal[1] = 0.0f;
    data[0].normal[2] = 1.0f; // Facing the camera

    // Outer Points
    for (int i = 0; i <= vertices; i++)
    {
        float angle = i * M_TAU / vertices;
        float x     = cosf(angle) * radius;
        float y     = sinf(angle) * radius;

        data[i + 1].position[0] = x;
        data[i + 1].position[1] = y;
        data[i + 1].position[2] = 0.0f;

        // Map texture coords (0.0 to 1.0 range)
        data[i + 1].texcoord[0] = (cosf(angle) + 1.0f) * 0.5f;
        data[i + 1].texcoord[1] = (sinf(angle) + 1.0f) * 0.5f;

        data[i + 1].normal[0] = 0.0f;
        data[i + 1].normal[1] = 0.0f;
        data[i + 1].normal[2] = 1.0f;
    }

    return data;
}

static vertex *generateCircle(float radius)
{
    return generatePolygon(CIRCLE_SEGMENTS, radius);
}

static DVLB_s *vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx projection;
static C3D_Mtx material = {{
    {{0.0f, 0.2f, 0.2f, 0.2f}}, // Ambient
    {{0.0f, 0.4f, 0.4f, 0.4f}}, // Diffuse
    {{0.0f, 0.8f, 0.8f, 0.8f}}, // Specular
    {{1.0f, 0.0f, 0.0f, 0.0f}}, // Emission
}};

static C3D_Tex sal_tex;

static float angleX = 0.0, angleY = 0.0;
static float xPos = 0.0, yPos = 0.0, zPos = 0.0;

// Helper function for loading a texture from memory
static bool loadTextureFromMem(C3D_Tex *tex, C3D_TexCube *cube, const void *data, size_t size)
{
    Tex3DS_Texture t3x = Tex3DS_TextureImport(data, size, tex, cube, false);
    if (!t3x)
        return false;

    // Delete the t3x object since we don't need it
    Tex3DS_TextureFree(t3x);
    return true;
}

static void sceneInit(void)
{
    // Load the vertex shader, create a shader program and bind it
    vshader_dvlb = DVLB_ParseFile((u32 *)vshader_shbin, vshader_shbin_size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    // Get the location of the uniforms
    uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
    uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");
    uLoc_lightVec     = shaderInstanceGetUniformLocation(program.vertexShader, "lightVec");
    uLoc_lightHalfVec = shaderInstanceGetUniformLocation(program.vertexShader, "lightHalfVec");
    uLoc_lightClr     = shaderInstanceGetUniformLocation(program.vertexShader, "lightClr");
    uLoc_material     = shaderInstanceGetUniformLocation(program.vertexShader, "material");

    // Configure attributes for use with the vertex shader
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal

    // Compute the projection matrix
    Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, false);

    // Create the VBO (vertex buffer object) and put them in linear memory
    cube_vbo_data = linearAlloc(sizeof(cube_vertex_list));
    memcpy(cube_vbo_data, cube_vertex_list, sizeof(cube_vertex_list));
    circle_vbo_data = generateCircle(0.5f);

    // Configure buffers
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);

    // Load the texture and bind it to the first texture unit
    if (!loadTextureFromMem(&sal_tex, NULL, sal_t3x, sal_t3x_size))
        svcBreak(USERBREAK_PANIC);
    C3D_TexSetFilter(&sal_tex, GPU_LINEAR, GPU_NEAREST);
    C3D_TexBind(0, &sal_tex);
}

static void sceneRender(void)
{
    // Update global camera uniforms
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec, 0.0f, 0.0f, -1.0f, 0.0f);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr, 1.0f, 1.0f, 1.0f, 1.0f);

    C3D_BufInfo *bufInfo   = C3D_GetBufInfo();
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();

    // ============================= Draw the Cube =============================
    C3D_TexBind(0, &sal_tex);

    // Explicitly reset attribute info layout for Cube
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal

    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, cube_vbo_data, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(bufInfo);

    // Restore the cube's shiny reflective material profile
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material, &material);

    C3D_Mtx cubeModelView;
    Mtx_Identity(&cubeModelView);
    Mtx_Translate(&cubeModelView, 0.0 + xPos, 0.0 + yPos, -2.0f, true);
    Mtx_RotateX(&cubeModelView, angleX, true);
    Mtx_RotateY(&cubeModelView, angleY, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &cubeModelView);

    // Completely reset and decouple TexEnv Stage 0 and Stage 1 for Cube
    C3D_TexEnv *cubeEnv0 = C3D_GetTexEnv(0);
    C3D_TexEnvInit(cubeEnv0);
    C3D_TexEnvSrc(cubeEnv0, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
    C3D_TexEnvFunc(cubeEnv0, C3D_Both, GPU_MODULATE);

    C3D_TexEnv *cubeEnv1 = C3D_GetTexEnv(1);
    C3D_TexEnvInit(cubeEnv1); // Forces stage 1 into a clean pass-through state

    C3D_DrawArrays(GPU_TRIANGLES, 0, CUBE_VERTEX_LIST_COUNT);

    // ============================ Draw the Circle ============================
    C3D_TexBind(0, NULL);

    // Explicitly reset attribute info layout for Circle
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3);

    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, circle_vbo_data, sizeof(vertex), 3, 0x210);
    C3D_SetBufInfo(bufInfo);

    // FORCE FLAT LIGHTING: Slam the material to pure emissive white before drawing circle
    static const C3D_Mtx flatWhiteMaterial = {{
        {{1.0f, 1.0f, 1.0f, 1.0f}}, // Ambient
        {{1.0f, 1.0f, 1.0f, 1.0f}}, // Diffuse
        {{0.0f, 0.0f, 0.0f, 0.0f}}, // Specular (Disabled to stop tan/yellow shifts!)
        {{1.0f, 1.0f, 1.0f, 1.0f}}, // Emission
    }};
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material, &flatWhiteMaterial);

    C3D_Mtx circleModelView;
    Mtx_Identity(&circleModelView);
    Mtx_Translate(&circleModelView, 0.0 - xPos, 0.0 - yPos, -2.0f - zPos, true);
    Mtx_RotateX(&circleModelView, angleX, true);
    Mtx_RotateY(&circleModelView, angleY, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &circleModelView);

    // Completely reset and decouple TexEnv Stage 0 and Stage 1 for Circle
    C3D_TexEnv *circleEnv0 = C3D_GetTexEnv(0);
    C3D_TexEnvInit(circleEnv0);
    C3D_TexEnvSrc(circleEnv0, C3D_Both, GPU_PRIMARY_COLOR, 0, 0); // Pull directly from white material
    circleEnv0->srcRgb = GPU_PRIMARY_COLOR;                       // Fix fallback assignments explicitly
    C3D_TexEnvFunc(circleEnv0, C3D_Both, GPU_REPLACE);

    C3D_TexEnv *circleEnv1 = C3D_GetTexEnv(1);
    C3D_TexEnvInit(circleEnv1);

    C3D_DrawArrays(GPU_TRIANGLE_FAN, 0, CIRCLE_VERTEX_COUNT);
}

static void sceneExit(void)
{
    // Free the texture
    C3D_TexDelete(&sal_tex);

    // Free the VBO
    linearFree(cube_vbo_data);
    linearFree(circle_vbo_data);

    // Free the shader program
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
}

int main()
{
    // Initialize graphics
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    // Initialize the render target
    C3D_RenderTarget *target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    // Initialize the scene
    sceneInit();

    PrintConsole bottomScreen;

    // Initialize console for both screen using the two different PrintConsole we have defined
    consoleInit(GFX_BOTTOM, &bottomScreen);
    consoleSelect(&bottomScreen);
    printf("Salutations! :D\n");
    printf("...or hi, i guess :(");

    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        if (kHeld & KEY_DDOWN)
            yPos += -0.1;
        if (kHeld & KEY_DUP)
            yPos += 0.1;
        if (kHeld & KEY_DLEFT)
            xPos += -0.1;
        if (kHeld & KEY_DRIGHT)
            xPos += 0.1;

        if (kHeld & KEY_X)
            angleX += -0.1;
        if (kHeld & KEY_B)
            angleX += 0.1;
        if (kHeld & KEY_A)
            angleY += 0.1;
        if (kHeld & KEY_Y)
            angleY += -0.1;

        if (kHeld & KEY_ZR)
            zPos += -0.1;
        if (kHeld & KEY_ZL)
            zPos += 0.1;

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
        C3D_FrameDrawOn(target);
        sceneRender();
        C3D_FrameEnd(0);
    }

    // Deinitialize the scene
    sceneExit();

    // Deinitialize graphics
    C3D_Fini();
    gfxExit();
    return 0;
}
