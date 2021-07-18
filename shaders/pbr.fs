#version 330 core

// ----------------------------------------------------------------------------
out vec4 FragColor;

// ----------------------------------------------------------------------------
in vec2 fs_uv;
in vec3 fs_worldPos;
in vec3 fs_normal;
in vec3 fs_tangent;
in vec3 fs_bitangent;
in vec4 fs_lightSpacePos;
flat in int fs_materialID;

// ----------------------------------------------------------------------------
uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_viewPosition;
uniform vec3 u_lightDirection;
uniform mat4 u_lightSpace;
uniform vec3 u_lightColor;
uniform bool u_debug;

// ----------------------------------------------------------------------------
#define MAX_MATERIAL_NUM 5
uniform ivec4 u_mat_textureID1[MAX_MATERIAL_NUM]; // albedo, normal, metallic
uniform ivec3 u_mat_textureID2[MAX_MATERIAL_NUM]; // roughness, ao, disp
uniform vec4  u_mat_color[MAX_MATERIAL_NUM];      // albedo colors
uniform vec3  u_mat_attrib[MAX_MATERIAL_NUM];     // metallic, roughness
uniform bvec3 u_mat_isGS_txt[MAX_MATERIAL_NUM];   // is glossiness & specular texture

// ----------------------------------------------------------------------------
uniform samplerCube u_irradianceMap;              // IBL
uniform sampler2D   u_shadowMap;                  // shadow

// ----------------------------------------------------------------------------
// textures
#define MAX_MATERIAL_TEXTURE 25
uniform sampler2D u_textures[MAX_MATERIAL_TEXTURE];
uniform float u_uv_scale;
uniform float u_dispMapScale;

// ----------------------------------------------------------------------------
uniform bool  u_floor_grid;

// ----------------------------------------------------------------------------
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
vec3 Tonemap_ACES(const vec3 x) 
{
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

//gamma correction
vec3 OECF_sRGBFast(const vec3 linear) {
    return pow(linear, vec3(1.0 / 2.2));
}

// ----------------------------------------------------------------------------
float filteredGrid(in vec2 p, in vec2 dpdx, in vec2 dpdy )
{
	float sizeCoeff = 0.75; // grid n
	p *= sizeCoeff;

    const float _N = 800.0; // line size
    vec2 w = max(abs(dpdx), abs(dpdy));
	w *= sizeCoeff;
    vec2 a = p + 0.5 * w;                        
    vec2 b = p - 0.5 * w;           
    vec2 i = (floor(a)+min(fract(a)*_N,1.0)-
              floor(b)-min(fract(b)*_N,1.0))/(_N*w);
    return (1.0-i.x)*(1.0-i.y);
}

// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2D shadow_map)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_map, projCoords.xy).r; 
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(sampler2D normalMap, vec2 TexCoords)
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
#if 0
    vec3 Q1  = dFdx(fs_worldPos);
    vec3 Q2  = dFdy(fs_worldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(fs_normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
#else
    vec3 N   = normalize(fs_normal);
    vec3 T   = normalize(fs_tangent);
    vec3 B   = normalize(fs_bitangent);
    mat3 TBN = mat3(T, B, N);
#endif
    return normalize(TBN * tangentNormal);
}

vec3 getNormalFromMap(sampler2D normalMap, vec2 TexCoords, mat3 TBN)
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
mat3 TBNMatrix()
{
    vec3 N   = normalize(fs_normal);
    vec3 T   = normalize(fs_tangent);
    vec3 B   = normalize(fs_bitangent);
    mat3 TBN = mat3(T, B, N);
    return TBN;
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  
// ----------------------------------------------------------------------------
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, int displacementID)
{
    float heightScale = u_dispMapScale;
    
    // number of depth layers
    const float minLayers = 16;
    const float maxLayers = 128;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(u_textures[displacementID], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(u_textures[displacementID], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(u_textures[displacementID], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
// ----------------------------------------------------------------------------
void main()
{	
    if(!gl_FrontFacing)
    {
        discard;
    }
    
    // find material texture ID
    int albedoID       = u_mat_textureID1[fs_materialID].x;
    int normalID       = u_mat_textureID1[fs_materialID].y;
    int metallicID     = u_mat_textureID1[fs_materialID].z;
    int emissiveID     = u_mat_textureID1[fs_materialID].w;
    int roughnessID    = u_mat_textureID2[fs_materialID].x;
    int aoID           = u_mat_textureID2[fs_materialID].y;
    int displacementID = u_mat_textureID2[fs_materialID].z;
    
    // texture scaling
    vec2 uv = u_uv_scale * fs_uv;

    // find material attributes
    vec3  mat_color     = u_mat_color[fs_materialID].rgb;
    float mat_metallic  = u_mat_attrib[fs_materialID].x;
    float mat_roughness = u_mat_attrib[fs_materialID].y;

    // set normal
    vec3 N = normalize(fs_normal);
    vec3 V = normalize(u_viewPosition - fs_worldPos);

    // TBN
    mat3 TBN = TBNMatrix();

    // displacement mapping
    if(u_mat_textureID2[fs_materialID].z >= 0)
    {
        mat3 TBN_tr = transpose(TBN);
        vec3 V_ = normalize(TBN_tr * V);

        uv = ParallaxMapping(uv, V_, displacementID);
        if(uv.x > u_uv_scale || uv.y > u_uv_scale || uv.x < 0.0 || uv.y < 0.0)
            discard;
    }

    // pbr materials
    vec3  albedo    = pow(mat_color, vec3(2.2));
    float metallic  = mat_metallic;
    float roughness = mat_roughness;
    float ao        = 1.0f;
    vec3  emissive  = vec3(0.0);

    // ---------------------------------------------------------------------------- //
    // albedo texture
    if(u_mat_textureID1[fs_materialID].x >= 0)
    {
        //albedo = texture(u_textures[albedoID], uv).rgb;
        albedo = pow(texture(u_textures[albedoID], uv).rgb, vec3(2.2));
        //albedo = texelFetch(u_textures[albedoID], ivec2(4, 4), 0).rgb;
        //albedo = textureLod(u_textures[albedoID], uv, 0).rgb;
    }

    // metallic texture
    if(u_mat_textureID1[fs_materialID].z >= 0)
    {
        if(u_mat_isGS_txt[fs_materialID].x)
        {
            // metallic  = texture(u_textures[metallicID], uv).r;
        }
        else
        {
            metallic  = texture(u_textures[metallicID], uv).r;
        }
        metallic = clamp(metallic, 0.0, 1.0);
    }

    // roughness texture
    if(u_mat_textureID2[fs_materialID].x >= 0)
    {
        if(u_mat_isGS_txt[fs_materialID].y)
        {
            // texture is glossiness
            //roughness = 1.0 - texture(u_textures[roughnessID], uv).r;
        }
        else
        {
            roughness = texture(u_textures[roughnessID], uv).r;
        }
        roughness = clamp(roughness, 0.0, 1.0);
    }

    // ambient occlusion texture
    if(u_mat_textureID2[fs_materialID].y >= 0)
    {
        ao = texture(u_textures[aoID], uv).r;
    }

    // emissive texture
    if(u_mat_textureID1[fs_materialID].w >= 0)
    {
        emissive = pow(texture(u_textures[emissiveID], uv).rgb, vec3(2.2));
        //emissive = texture(u_textures[emissiveID], uv).rgb;
    }

    // normal texture
    if(u_mat_textureID1[fs_materialID].y >= 0)
    {
        N = getNormalFromMap(u_textures[normalID], uv, TBN);
    }
    // ---------------------------------------------------------------------------- //
    // debug?
    if(u_debug)
    {
        albedo = pow(albedo, vec3(1/2.2));
        FragColor = vec4(albedo, 1.0f);
        return;
    }
    // ---------------------------------------------------------------------------- //

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(u_lightDirection);
        vec3 H = normalize(V + L);
        float distance = 1.0;
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_lightColor;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3  F   = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3  nominator   = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3  specular    = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
#if 0
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
#else
    //vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = texture(u_irradianceMap, N).rgb;
    
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;

    // ambient color
    //ambient = 0.5 * ambient + 0.5 * vec3(0.03) * albedo * ao;
#endif
    vec3 lightDir = normalize(u_lightDirection);
    float shadow = ShadowCalculation(fs_lightSpacePos, N, lightDir, u_shadowMap);
    shadow = clamp(shadow, 0.0, 1.0);

    vec3 gridCol = vec3(0.0);
    if(u_floor_grid)
    {
        vec2 dpdx = dFdx(fs_worldPos.xz);
        vec2 dpdy = dFdy(fs_worldPos.xz);
        float tile = filteredGrid(fs_worldPos.xz, dpdx, dpdy);
        tile = pow(tile, 3.0);
        
        gridCol = vec3(1.0 - tile);
        //Lo = Lo + gridCol;

        //vec3 gridCol = vec3(tile);
        //Lo = Lo * gridCol;
    }
    
    vec3 color = emissive + ambient + (1.0 - shadow) * Lo + (1.0 - 0.95 * shadow) * gridCol;
#if 0
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 
#else
    color = OECF_sRGBFast(color);
    color = Tonemap_ACES(color);
#endif

    // Fog
    float D = length(u_viewPosition - fs_worldPos);
    vec3 fog_color = vec3(0.8, 0.8, 0.82);
    float fog_amount = 1.0f - min(exp(-D * 0.03 + 1.5), 1.0);
    color = mix(color, fog_color, fog_amount);

    FragColor = vec4(color, 1.0);
}
