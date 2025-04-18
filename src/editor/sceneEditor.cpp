#include "../../headers/editor/sceneEditor.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/editor/editor.h"
#include "../../headers/math/math.h"

void SceneEditor::PassWindow(Window* wnd)
{
    if(wnd == nullptr)
    {
        std::cout << "Window instance is empty. Can't use editor\n";
        return;
    }

    _window = wnd;
}

void SceneEditor::PassEditorDebug(EditorDebug& debug)
{
    _editorDebug = &debug;
}


void SceneEditor::PassCollisionDependency(const CollisionDependency& dependencies)
{
    if(dependencies.collisionDebug != nullptr)
    {
        _collisionDependency.collisionDebug = dependencies.collisionDebug;
    }
    else std::cout << "Cannot inject collision. Collision debug is null\n";
    
    if(dependencies.physSystem != nullptr)
    {
        _collisionDependency.physSystem = dependencies.physSystem;
    }
    else std::cout << "Cannot inject collision. Physics system is null\n";

}
// Purpose: as I decided to use raycasting to handle object selection, would be great
// idea to use Jolt to get AABB of every object.
void SceneEditor::PrepareObjectSelection()
{
    using namespace JPH;
    if(_collisionDependency.physSystem == nullptr)
    {
        std::cout << "Phys system instance is empty. Can't prepare object selection\n";
        return;
    }

    BodyIDVector bodies;
    _collisionDependency.physSystem->GetBodies(bodies);
    const BodyLockInterface& bli = _collisionDependency.physSystem->GetBodyLockInterface();
    for(BodyID bodyID : bodies)
    {
        BodyLockRead lock(bli, bodyID);

        if(lock.Succeeded())
        {   
            const Body& body = lock.GetBody();
            const Shape* shape = body.GetShape();
            if(shape == nullptr)
            {
                std::cout << "Can't get AABB of object with id " << bodyID.GetIndex() << " to prepare object selection\n";
                continue;
            }   
            
            JPH::AABox localBounds = shape->GetLocalBounds();
            localBounds.Translate(shape->GetCenterOfMass());
            if(_collidersAABBs.find(bodyID) == _collidersAABBs.end())
                _collidersAABBs.insert({bodyID, localBounds});
        }
    }
  
    Shader sceneEditorShader;
    sceneEditorShader.LoadShaders("sceneEditor.vert", "sceneEditor.frag");
    
    RenderManager::AddShaderByType(std::move(sceneEditorShader), RenderPassType::RENDER_SCENE_EDITOR);
}

void SceneEditor::HandleObjectSelection()
{
    // handle picking by itself
    SelectObject();

}


void SceneEditor::SelectObject()
{
    const Mouse&    mouseState  = _window->GetMouseState();
    const uint32_t  width       = _window->GetWindowWidth();
    const uint32_t  height      = _window->GetWindowHeight();
    const Matrices& matrices    = SamuraiCameras::g_activeCamera->GetMVP();
    if(mouseState.mouseLeft)
    {
        // making normalize device coords from mouse pos
        const float x = (2.0f * mouseState.xPos) / static_cast<float>(width) - 1.0f;
        const float y = 1.0f - (2.0f * mouseState.yPos) / static_cast<float>(height);
        const float z = 1.0f;
    
        std::cout << "Mouse pos: " << mouseState.xPos << " " << mouseState.yPos << '\n';
        std::cout << "NDC: " << x << " " << y << " " << z << '\n';


        // making z point forwards in homo clip space
        glm::vec4 rayClip = glm::vec4(x, y, -z, 1.0f);
        // going to view space
        glm::vec4 rayViewSpace = glm::inverse(matrices.projection) * rayClip;
        rayViewSpace = glm::vec4(rayViewSpace.x, rayViewSpace.y, -z, 0.0f);

        // maybe to do in world space, would see
        glm::vec3 rayWorld = glm::inverse(matrices.view) * rayViewSpace;
        
        const glm::vec3 rayWorldNormalized = glm::normalize(rayWorld);
        const glm::vec3 rayOrigin = SamuraiCameras::g_activeCamera->GetPosition();
        std::cout << "Ray: " << rayWorldNormalized.x << ' ' << rayWorldNormalized.y << ' ' << rayWorldNormalized.z << '\n';

        // Debug update
        LineDebug newRayDebug;
        newRayDebug.origin    = ConvertGlmVec3ToJolt(rayOrigin);
        newRayDebug.direction = ConvertGlmVec3ToJolt(rayWorldNormalized);
        newRayDebug.endType   = LineDebug::EndType::END_TYPE_DIRECTION;
        _editorDebug->RequestLineDebugUpdate(newRayDebug);
        


        for(auto body : _collidersAABBs)
        {
            JPH::AABox aaBoxLocal = body.second;
            
            const glm::vec3 aabbMin = ConvertJoltVec3ToGlm(aaBoxLocal.mMin);
            const glm::vec3 aabbMax = ConvertJoltVec3ToGlm(aaBoxLocal.mMax);

            
            // Means that point is inside
            // TO CHECK: Jolt's AABB generation, I think the issue lies over there.
            if(SamuraiMath::IntersectAABB(rayOrigin, rayWorldNormalized, aabbMin, aabbMax))
            {
                std::cout << "Intersects\n";
            }
        }
    }
}