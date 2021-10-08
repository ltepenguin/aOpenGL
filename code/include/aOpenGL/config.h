// Configurations

// Maximum number of texutres for material
#define AGL_MAX_MATERIAL_TEXTURES  25

// Maximum number of materials
#define AGL_MAX_MATERIAL_NUM       5

// All paths are relative to AGL_PATH

// Diffuse irradiance shader
#define AGL_BACKGROUND_HDR_PATH    "/data/textures/hdr_map/quarry_03_2k.hdr"
#define AGL_TOCUBE_VS              "/shaders/to_cubemap.vs"
#define AGL_TOCUBE_FS              "/shaders/to_cubemap.fs"
#define AGL_BACKGROUND_VS          "/shaders/background.vs"
#define AGL_BACKGROUND_FS          "/shaders/background.fs"

// PBR vertex & fragment shader
#define AGL_PBR_VS                 "/shaders/pbr.vs"
#define AGL_PBR_FS                 "/shaders/pbr.fs"
#define AGL_LBS_PBR_VS             "/shaders/pbr_lbs.vs"
#define AGL_SHADOW_VS              "/shaders/shadow.vs"
#define AGL_EMPTY_FS               "/shaders/empty.fs"
#define AGL_TEXT_VS                "/shaders/text3d.vs"
#define AGL_TEXT_FS                "/shaders/text3d.fs"

// Shadow map size
#define AGL_SHADOW_MAP_SIZE        4 * 1024

// Arrow FBX Path
#define AGL_ARROW_FBX               "/data/fbx/etc/arrow.fbx"
#define AGL_AXIS_FBX                "/data/fbx/etc/axis.fbx"

#define AGL_FONT_PATH               "/data/fonts/NotoSans-Regular.ttf"
#define AGL_FONT_RESOLUTION         256