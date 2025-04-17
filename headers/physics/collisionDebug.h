#pragma once
#include "../types/collisionTypes.h"

// #ifndef JPH_DEBUG_RENDERER
// #define JPH_DEBUG_RENDERER
// #endif

#include <Jolt/Renderer/DebugRenderer.h>


class Camera;

// Purpose: To make debug system for physics(displaying colliders etc)
// Type: Own implementation of Jolt class
class CollisionDebug : public JPH::DebugRenderer
{
private:
    EBOSetupBuffers _geometryBuffers;
    VBOSetupUnskinned _linesBuffers;
public:
    virtual void			                    DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void			                    DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow = JPH::DebugRenderer::ECastShadow::Off) override;
    virtual JPH::DebugRenderer::Batch			CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount) override;
	virtual JPH::DebugRenderer::Batch			CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount) override;
    virtual void			                    DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, 
                                                            const JPH::DebugRenderer::GeometryRef &inGeometry, JPH::DebugRenderer::ECullMode inCullMode = JPH::DebugRenderer::ECullMode::CullBackFace, 
                                                            JPH::DebugRenderer::ECastShadow inCastShadow = JPH::DebugRenderer::ECastShadow::On, JPH::DebugRenderer::EDrawMode = JPH::DebugRenderer::EDrawMode::Solid) override;
    virtual void			                    DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f) override;


    CollisionDebug();
};


// Purpose: class to allocate handle of triangle data to display it later.
// created for allocation as would need a lot of memory(probably) so better to place on the heap.
// Type: own implementation with the usage of reference counter from Jolt

class TriangleData : public JPH::RefTargetVirtual
{
private:
    JPH::atomic<JPH::uint32> _refCount = 0;

    std::vector<float> _vertices;
    std::vector<JPH::uint32> _indices;



    virtual void                        AddRef()  override { ++_refCount;}
    virtual void                        Release() override { if(--_refCount == 0) delete this;}

public:
    int32_t _numOfTriangles;
    bool _usesIndices = false;

    // would make getters/setters to make it more prominent
    const std::vector<float>& GetVertices() const { return _vertices;}
    const std::vector<JPH::uint32> GetIndices() const { return _indices;}



    TriangleData() = delete;
    // Two constructors for two different types of batch method
    explicit TriangleData(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount)
    {
        _numOfTriangles = inTriangleCount;
        _vertices.reserve(_numOfTriangles * 9);
        for(int32_t i = 0; i < _numOfTriangles; ++i)
        {
            JPH::DebugRenderer::Triangle triangle = inTriangles[i];


            JPH::DebugRenderer::Vertex vert1 = triangle.mV[0];
            JPH::DebugRenderer::Vertex vert2 = triangle.mV[1];
            JPH::DebugRenderer::Vertex vert3 = triangle.mV[2];

            _vertices.push_back(vert1.mPosition.x);
            _vertices.push_back(vert1.mPosition.y);
            _vertices.push_back(vert1.mPosition.z);

            _vertices.push_back(vert2.mPosition.x);
            _vertices.push_back(vert2.mPosition.y);
            _vertices.push_back(vert2.mPosition.z);

            _vertices.push_back(vert3.mPosition.x);
            _vertices.push_back(vert3.mPosition.y);
            _vertices.push_back(vert3.mPosition.z);
        }
    }

    TriangleData(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount)
    {
        _usesIndices = true;
        _vertices.reserve(inVertexCount * 3);
        _indices.reserve(inIndexCount);

        for(int i = 0; i < inVertexCount; ++i)
        {
            _vertices.push_back(inVertices[i].mPosition.x);
            _vertices.push_back(inVertices[i].mPosition.y);
            _vertices.push_back(inVertices[i].mPosition.z);
        }

        for(int i = 0; i < inIndexCount; ++i)
        {
            _indices.push_back(inIndices[i]);
        }
    }
};


// Purpose: to inject everything needed from collision class at once
// forward declaration of collision debug;
struct CollisionDependency
{
	JPH::PhysicsSystem* physSystem     = nullptr;
	CollisionDebug*     collisionDebug = nullptr;
};