// Maximum number of texutres for material
#define A_GL_MAX_MATERIAL_TEXTURES  25

// Maximum number of materials
#define A_GL_MAX_MATERIAL_NUM       5

// Diffuse irradiance shader
//#define A_GL_BACKGROUND_HDR_PATH    "../textures/Desert_Highway/Road_to_MonumentValley_Env.hdr"
//#define A_GL_BACKGROUND_HDR_PATH    "../data/textures/hdr_map/wide_street_02_2k.hdr"
//#define A_GL_BACKGROUND_HDR_PATH    "../textures/hdr_map/Panorama_hdr_0.hdr"
#define A_GL_BACKGROUND_HDR_PATH    "../data/textures/hdr_map/quarry_03_2k.hdr"
#define A_GL_TOCUBE_VS              "../shaders/to_cubemap.vs"
#define A_GL_TOCUBE_FS              "../shaders/to_cubemap.fs"
#define A_GL_BACKGROUND_VS          "../shaders/background.vs"
#define A_GL_BACKGROUND_FS          "../shaders/background.fs"

// PBR vertex & fragment shader
#define A_GL_PBR_VS                 "../shaders/pbr.vs"
#define A_GL_PBR_FS                 "../shaders/pbr.fs"
#define A_GL_LBS_PBR_VS             "../shaders/pbr_lbs.vs"
#define A_GL_SHADOW_VS              "../shaders/shadow.vs"
#define A_GL_SHADOW_FS              "../shaders/shadow.fs"

// Shadow map size
#define A_GL_SHADOW_MAP_SIZE        4 * 1024
