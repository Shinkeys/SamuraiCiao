#include "../../headers/utilities/geometryUtil.h"

#include "../../headers/math/math.h"

namespace SamuraiUtils
{
    std::vector<Vertex> GenerateCylinderVertices(float radius, float height, int32_t sectorCount)
    {
        const float angleStep = SamuraiDefines::g_TwoPi / sectorCount;
        std::vector<Vertex> vertices;
        // Top part of cylinder. Normals basically point up;
        Vertex vertex;
        vertex.position = glm::vec3(0.0f, height, 0.0f);
        vertex.normal   = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.push_back(vertex);
        for(int32_t i = 0; i <= sectorCount; ++i)
        {
            float angle = i * angleStep;
            vertex.position.x = radius * std::cos(angle);
            vertex.position.y = height;
            vertex.position.z = radius * std::sin(angle);
            
            vertex.normal.x = 0.0f;
            vertex.normal.y = 1.0f;
            vertex.normal.z = 0.0f;
            
            vertices.push_back(vertex);
        }
        
        // Base(bottom) part of cylinder. Normals basically point down;
        vertex.position = glm::vec3(0.0f, 0.0, 0.0f);
        vertex.normal   = glm::vec3(0.0f, -1.0f, 0.0f);
        vertices.push_back(vertex);
        for(int32_t i = 0; i <= sectorCount; ++i)
        {
            float angle = i * angleStep;
            vertex.position.x = radius * std::cos(angle);
            vertex.position.y = 0.0f;
            vertex.position.z = radius * std::sin(angle);
            
            vertex.normal.x = 0.0f;
            vertex.normal.y = -1.0f;
            vertex.normal.z = 0.0f;
            
            vertices.push_back(vertex);
        }
        
        // Vertices for the side
        for(int32_t i = 0; i <= sectorCount; ++i)
        {
            float angle = i * angleStep;
            vertex.position.x = radius * std::cos(angle);
            vertex.position.y = height;
            vertex.position.z = radius * std::sin(angle);
            
            vertex.normal = glm::normalize(glm::vec3(std::cos(angle), 0.0f, std::sin(angle)));
            
            // Top vertex
            vertices.push_back(vertex);
            
            // Bottom vertex
            vertex.position.y = 0.0f;
            vertices.push_back(vertex);
        }
        
        
        return vertices;
    }

    std::vector<uint32_t> GenerateCylinderIndices(int32_t sectorCount)
    {
        std::vector<uint32_t> indices;
        
        // Indices for the top
        for(int32_t i = 1; i <= sectorCount; ++i)
        {
            // 2 triangles per sector
            indices.push_back(0); // Center
            indices.push_back(i); // Current vertex
            indices.push_back(i % sectorCount + 1);  // Next vertex
        }

        // Indices for the bottom
        const int32_t bottomCenterIndex = sectorCount + 2;
        for(int32_t i = 1; i <= sectorCount; ++i)
        {
            indices.push_back(bottomCenterIndex); // Center
            indices.push_back(bottomCenterIndex + i); // Current vertex
            indices.push_back(bottomCenterIndex + (i % sectorCount) + 1);
        }

        // Indices for the side
        const int32_t sideStartIndex = (sectorCount + 2) * 2;
        for(int32_t i = 0; i < sectorCount; ++i)
        {
            int32_t topIndex = sideStartIndex + i * 2;
            int32_t bottomIndex = topIndex + 1;


            indices.push_back(topIndex);
            indices.push_back(bottomIndex);
            indices.push_back(topIndex + 2);

            indices.push_back(bottomIndex);
            indices.push_back(bottomIndex + 2);
            indices.push_back(topIndex + 2);
        }

        return indices;
    }
    
    std::vector<Vertex> GenerateConeVertices(float radius, float height, int32_t segments)
    {
        std::vector<Vertex> vertices;
        
        // Segments should be at least 3(Tetrahedron)
        segments = std::max(segments, 3);
        
        // Top part(apex) of cone
        Vertex vertex;
        vertex.position = glm::vec3(0.0f, height, 0.0f);
        vertex.normal   = glm::normalize(glm::vec3(0.0f, height, 0.0f));
        vertex.tangents = glm::vec3(1.0f, 0.0f, 0.0f);
        vertices.push_back(vertex);

        const float angleStep = SamuraiDefines::g_TwoPi / static_cast<double>(segments);
        for(int32_t i = 0; i < segments; ++i)
        {
            float angle = i * angleStep;
            
            vertex.position = glm::vec3(radius * std::cos(angle), 0.0f, radius * std::sin(angle));
            vertex.normal = glm::normalize(glm::vec3(vertex.position.x, height / 2.0f, vertex.position.z));
            vertex.tangents = glm::normalize(glm::vec3(-std::sin(angle), 0.0f, std::cos(angle)));
            
            vertices.push_back(vertex); 
        }
        
        // Base part
        vertex.position = glm::vec3(0.0f);
        vertex.normal   = glm::vec3(0.0f, -1.0f, 0.0f);
        vertex.tangents = glm::vec3(1.0f,  0.0f, 0.0f);
        vertices.push_back(vertex);
        
        return vertices;
    }

    std::vector<uint32_t> GenerateConeIndices(int32_t segments)
    {
        std::vector<uint32_t> indices;
        
        // Side
        for(int32_t i = 0; i < segments; ++i)
        {
            uint32_t apexIndex = 0; // Apex is stored first
            uint32_t baseIndex = i + 1;
            uint32_t baseIndex2 = (i + 1) % segments + 1;

            indices.push_back(baseIndex2);
            indices.push_back(baseIndex);
            indices.push_back(apexIndex);
        }

        const uint32_t centerIndex = segments + 1;
        for(int32_t i = 0; i < segments; ++i)
        {
            uint32_t baseIndex = i + 1;
            uint32_t baseIndex2 = (i + 1) % segments + 1;

            // Center -> base -> base2
            indices.push_back(baseIndex);
            indices.push_back(baseIndex2);
            indices.push_back(centerIndex);
        }
        return indices;
    }
    
    std::vector<Vertex> GenerateCubeVertices()
    {
        std::vector<Vertex> vertices;

        glm::vec3 positions[] = 
        {
            {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},     // Front face
            {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},     // Back face
            { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f},     // Right face
            {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f},     // Left face
            {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},     // Top face
            {-0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f},     // Bottom face
        };


        glm::vec3 normals[] = 
        {
            {  1.0f,  0.0f,  0.0f},
            { -1.0f,  0.0f,  0.0f},
            {  0.0f,  1.0f,  0.0f},
            {  0.0f, -1.0f,  0.0f},
            {  0.0f,  0.0f,  1.0f},
            {  0.0f,  0.0f, -1.0f},
        };

        // Filling all the vertices
        Vertex vertex;
        for(int32_t i = 0; i < 24; ++i)
        {
            vertex.position = positions[i];
            vertex.normal   = normals[i % 6];
            vertices.push_back(vertex);
        }

        return vertices;
    }

    std::vector<uint32_t> GenerateCubeIndices()
    {
        std::vector<uint32_t> indices = 
        {
            0, 1, 2, 2, 3, 0,       // Front face
            4, 5, 6, 6, 7, 4,       // Back  face
            8, 9, 10, 10, 11, 8,    // Right face
            12, 13, 14, 14, 15, 12, // Left face
            16, 17, 18, 18, 19, 16, // Top face
            20, 21, 22, 22, 23, 20  // Bottom face
        };

        return indices;
    }
}