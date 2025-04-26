#include "../../headers/editor/gizmo.h"
#include "../../headers/utilities/geometryUtil.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"


void Gizmo::Initialize()
{
    CreateGeometry();
    _setup.type = GL_DYNAMIC_DRAW;
    OpenglBackend::BindModelEBO(_setup);

    ApplyLocalTransformations();
}


// Purpose: method to move all gizmo parts at once. Basically called inside handle is a
// handle to overall position in world for the gizmo(including all the parts(e.g. x,y,z arrows))
void Gizmo::TranslateGizmoObject(glm::vec3 position)
{
    _gizmoObjectTransform.worldPos = position;
}

// Purpose: Apply local transformation to vertices, to basically create gizmo for every
// axis from only one piece of every shape from the "Create Geometry" method
void Gizmo::ApplyLocalTransformations()
{
    const glm::mat4 defaultModel = glm::mat4(1.0f);

    GizmoPartTransform transformX;
    transformX.rotation = glm::rotate(defaultModel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
    transformX.color    = ColorsType::RED;

    GizmoPartTransform transformY;
    transformY.rotation = defaultModel;
    transformY.color    = ColorsType::GREEN;

    GizmoPartTransform transformZ;
    transformZ.rotation = glm::rotate(defaultModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
    transformZ.color    = ColorsType::BLUE;

    _gizmoPartDescriptors.insert({GizmoGroup::GIZMO_PART_X, transformX});
    _gizmoPartDescriptors.insert({GizmoGroup::GIZMO_PART_Y, transformY});
    _gizmoPartDescriptors.insert({GizmoGroup::GIZMO_PART_Z, transformZ});
}


// Purpose: Create gizmo's geometry and pass it to it's own buffer to draw later
void Gizmo::CreateGeometry()
{
    GizmoPartHandle cube;
    cube.vertices = SamuraiUtils::GenerateCubeVertices();
    cube.indices  = SamuraiUtils::GenerateCubeIndices();

    for(auto& vert : cube.vertices)
    {
        vert *= 0.50f;
    }

    auto functor = [](const auto& lhs, const auto& rhs)
    {
        return lhs.position.y < rhs.position.y;
    };

    const float cubeMaxPoint  = std::max_element(cube.vertices.begin(), cube.vertices.end(), functor)->position.y;
    const uint32_t cubeMaxInd = *std::max_element(cube.indices.begin(), cube.indices.end());

    const int32_t sectorCount = 36;
    const int32_t cylinderHeight = 7.0f;
    const int32_t cylinderRadius = 1.5f;
    GizmoPartHandle cylinder;
    cylinder.vertices = SamuraiUtils::GenerateCylinderVertices(cylinderRadius, cylinderHeight, sectorCount);
    cylinder.indices  = SamuraiUtils::GenerateCylinderIndices(sectorCount);

    for(auto& vert : cylinder.vertices)
    {
        vert.position *= 0.2f;
        // Making it longer
        vert.position.y *= 4.5f;
        vert.position.y += cubeMaxPoint;
    }
    for(auto& ind : cylinder.indices)
    {
        ind += cubeMaxInd + 1;
    }


    const float cylinderMaxPoint  = std::max_element(cylinder.vertices.begin(), cylinder.vertices.end(), functor)->position.y;
    const uint32_t cylinderMaxInd = *std::max_element(cylinder.indices.begin(), cylinder.indices.end());

    const int32_t segments = 20;
    const int32_t coneHeight = 2.5f;
    const int32_t coneRadius = 1.5f;
    GizmoPartHandle cone;
    cone.vertices = SamuraiUtils::GenerateConeVertices(coneRadius, coneHeight, segments);
    cone.indices  = SamuraiUtils::GenerateConeIndices(segments);
    for(auto& vert : cone.vertices)
    {
        vert *= 0.35f;
        vert.position.y += cylinderMaxPoint;
    }
    for(auto& ind : cone.indices)
    {
        ind += cylinderMaxInd + 1;
    }


    _setup.vertices.insert(_setup.vertices.end(), cube.vertices.begin(), cube.vertices.end());
    _setup.indices.insert(_setup.indices.end(),   cube.indices.begin(),  cube.indices.end());

    _setup.vertices.insert(_setup.vertices.end(), cylinder.vertices.begin(), cylinder.vertices.end());
    _setup.indices.insert(_setup.indices.end(),   cylinder.indices.begin(),  cylinder.indices.end());

    _setup.vertices.insert(_setup.vertices.end(), cone.vertices.begin(), cone.vertices.end());
    _setup.indices.insert(_setup.indices.end(),   cone.indices.begin(),  cone.indices.end());

}

void Gizmo::RenderLoop()
{
    Update();
    Render();
}

void Gizmo::Update()
{
    
}

void Gizmo::Render()
{
    if(SamuraiCameras::g_activeCamera->GetCameraType() != CameraType::CAMERA_TYPE_EDIT)
        return;

    // Important
    glDisable(GL_CULL_FACE);

    glBindVertexArray(_setup.VAO);

    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_SCENE_EDITOR);
    if(shaderIt == RenderManager::_shaderTypes.end())
    {
        std::cout << "Shader for gizmo draw is not  found\n";
        return;
    }
    
    const Matrices& matrices = SamuraiCameras::g_activeCamera->GetMVP();
    shaderIt->second.UseShader();
    shaderIt->second.SetMat4x4("view", matrices.view);
    shaderIt->second.SetMat4x4("projection", matrices.projection);

    constexpr size_t cubeIndices = 24;
    for(const auto& gizmoDesc : _gizmoPartDescriptors)
    {
        auto modelMat = gizmoDesc.second.rotation;

        // Applying overall world transformation of gizmo
        modelMat[3] = glm::vec4(_gizmoObjectTransform.worldPos, 1.0f);
        // To do: translate
        shaderIt->second.SetMat4x4("model", modelMat);
        shaderIt->second.SetVec4("color", gizmoDesc.second.color.GetColor());
        glDrawElements(GL_TRIANGLES, _setup.indices.size() - cubeIndices, GL_UNSIGNED_INT, _setup.indices.data() + cubeIndices);
    }


    glEnable(GL_CULL_FACE);

}