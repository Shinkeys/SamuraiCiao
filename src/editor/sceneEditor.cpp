#include "../../headers/editor/sceneEditor.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/editor/editor.h"
#include "../../headers/types/collisionTypes.h"


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

void SceneEditor::PassAssetManager(AssetManager* manager)
{
    if(manager == nullptr)
    {
        std::cout << "Asset manager instance is empty. Can't use it in the editor\n";
        return;
    }

    _manager = manager;
}


void SceneEditor::PassCollisionDependency(CollisionDependency* dependencies)
{
    if(dependencies == nullptr)
    {
        std::cout << "Cannot inject collision. Collision dependencies is null\n";
        return;
    }
    
    _collisionDependency = dependencies;

}
// Purpose: as I decided to use raycasting to handle object selection, would be great
// idea to use Jolt to get AABB of every object.
void SceneEditor::PrepareObjectSelection()
{
    Shader sceneEditorShader;
    sceneEditorShader.LoadShaders("sceneEditor.vert", "sceneEditor.frag");
    
    RenderManager::AddShaderByType(std::move(sceneEditorShader), RenderPassType::RENDER_SCENE_EDITOR);


    _gizmo.PassCollisionDependency(_collisionDependency);
    _gizmo.Initialize();
}

void SceneEditor::HandleObjectSelection()
{
    // handle picking by itself
    SelectObject();
    
    ChangeSelectionState();



    if(_someObjectSelected)
        _gizmo.RenderLoop();
}


// Purpose: method to change state of the variable which shows that some 
// object is currently selected or not
void SceneEditor::ChangeSelectionState()
{
    if(_someObjectSelected && _window->GetKeysState().cancelSelection)
    {
        _someObjectSelected = false;
        return;
    }
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
        

        auto rayResult = _collisionDependency->CheckForRayIntersection(rayOrigin, rayWorldNormalized);
        if(rayResult != std::nullopt)
        {
            std::cout << "Selected object is: " << rayResult.value() << '\n';
            

            const std::string& meshName = rayResult.value();

            const auto objectCenter = _manager->GetMeshCenterPoint(meshName);
            const glm::mat4* modelMatrix = _manager->GetTransformMatrixByName(meshName);
            
                if(objectCenter == std::nullopt)
                    return;
                if(modelMatrix == nullptr)
                    return;
                
            const glm::vec3 centerWorldSpace = (*modelMatrix) * glm::vec4(objectCenter.value(), 1.0f);
            _gizmo.TranslateGizmoObject(centerWorldSpace);


            if(meshName == "gizmo_axis_x" || meshName == "gizmo_axis_y" || meshName == "gizmo_axis_z")
            {

            }
            else
            {
                
                
                _someObjectSelected = true;
            }
        }
    }
}