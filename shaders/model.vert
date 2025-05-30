#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 lightMatrix;
uniform mat4 lightMatrixDirLight;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;


const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT = 1;

const uint MAX_CONCURRENT_SHADOWS_SOURCES = 4;
struct LightsDescShadows
{
    vec4 lightsShadowsData;
    int  type;
    int lightForShadowsPresence;
};
uniform LightsDescShadows lightsForShadows[MAX_CONCURRENT_SHADOWS_SOURCES];

out VERTEX_OUT
{
    vec2 texCoord;
    mat3 TBN;
    vec3 worldFragPos;
    vec3 backupNormals;

    // Can't output structure, sadge
    vec3 lightsShadowsData[MAX_CONCURRENT_SHADOWS_SOURCES];
    vec3 lightViewFragPos[MAX_CONCURRENT_SHADOWS_SOURCES];
    int  lightShadowsType[MAX_CONCURRENT_SHADOWS_SOURCES];

    vec3 dirLightShadowsData;
    vec3 dirLightFragPos;
} vertex_out;

uniform vec3 directionalLightDir;

void main()
{
    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    vertex_out.texCoord = aTexCoord;
    // This is basically only for lights which affects shadow mapping(e.g should produce shadows)
    // for(uint i = 0; i < MAX_CONCURRENT_SHADOWS_SOURCES; ++i)
    // {
    //     if(lightsForShadows[i].lightForShadowsPresence > 0)
    //     {
    //         vertex_out.lightShadowsType[i] = lightsForShadows[i].type;
    //         if(lightsForShadows[i].type == LIGHT_TYPE_POINT)
    //         {
    //             vec3 lightDir = vertex_out.lightViewFragPos[i] - lightsForShadows[i].lightsShadowsData.xyz;
    //             vertex_out.lightViewFragPos[i] = vec3(lightMatrix * model * vec4(aPos, 1.0));
    //             vertex_out.lightsShadowsData[i] = vec3(mat3(lightMatrix) * lightDir);
    //         }
    //         else if(lightsForShadows[i].type == LIGHT_TYPE_DIRECTIONAL)
    //         {
    //             vertex_out.lightsShadowsData[i] = vec3(mat3(lightMatrixDirLight) * lightsForShadows[i].lightsShadowsData.xyz);
    //             vertex_out.lightViewFragPos[i] = vec3(lightMatrixDirLight * model * vec4(aPos, 1.0));
    //         }
    //     }
    // }

    vertex_out.dirLightShadowsData = vec3(mat3(lightMatrixDirLight) * directionalLightDir);
    vertex_out.dirLightFragPos = vec3(lightMatrixDirLight * model * vec4(aPos, 1.0));

    // normal mapping
    // calculating tangents, normals via normal matrix because if model would have heterogeneous
    // scale, would break normals, tangents etc
    vec3 normals = normalize(mat3(model) * aNormal);
    vec3 tangents = normalize(mat3(model) * aTangent);
    // Gram-Schmidt process to make vectors orthogonal back
    tangents = normalize(tangents - dot(tangents, normals) * normals);
    vec3 bitangents = normalize(cross(normals, tangents));

    vertex_out.TBN = mat3(tangents, bitangents, normals);
    vertex_out.worldFragPos = vec3(model * vec4(aPos, 1.0));
    vertex_out.backupNormals = aNormal;

}
