#pragma once
#include "../types/collisionTypes.h"

#ifndef JPH_DEBUG_RENDERER
#define JPH_DEBUG_RENDERER
#endif

// Purpose: To make debug system for physics(displaying colliders etc)
// Type: Own implementation of Jolt class
class CollisionDebug : public JPH::DebugRenderer
{
public:
    virtual void			                    DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void			                    DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override;
    virtual JPH::DebugRenderer::Batch			CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount) override;
	virtual JPH::DebugRenderer::Batch			CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount) override;
    virtual void			                    DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, 
                                                    const GeometryRef &inGeometry, ECullMode inCullMode = ECullMode::CullBackFace, 
                                                    ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid) override;
    virtual void			                    DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f) override;



    CollisionDebug();
};

// Purpose: Responsible for the references to the object
class RefHandle
{
private:
    int32_t _refCount = 0;

public:
    void IncreaseRefCount() { ++_refCount;}
    void RemoveRef() { --_refCount;}
};


class TriangleData final : public JPH::RefTargetVirtual
{
private:
    RefHandle _refHandle;
    int32_t _numOfTriangles;
    std::vector<float> _vertices;
    std::vector<JPH::uint32> _indices;
public:

    // Two constructors for two different types of batch method
    TriangleData(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount)
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


    // to do
    virtual void AddRef() override
    {
        _refHandle.IncreaseRefCount();
    }
    virtual void Release() override
    {
        _refHandle.RemoveRef();
    }
    
};