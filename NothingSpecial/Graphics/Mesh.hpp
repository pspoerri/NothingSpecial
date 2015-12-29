//
//  Mesh.hpp
//  NothingSpecial
//
//  Created by Pascal Spörri on 18/09/15.
//  Copyright © 2015 Pascal Spörri. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp
#include "base.hpp"
#include "Shader.hpp"
#include "VertexBuffer.hpp"
#include "RenderableObject.hpp"

namespace Graphics {
    class Mesh: public RenderableObject
    {
    public:
        Mesh(std::vector<vec3>&& vertices,
             std::vector<vec4>&& colors,
             std::vector<face>&& faces);
        virtual ~Mesh();
        virtual void render(const Camera& camera) const;
        
    private:
        std::vector<vec3> m_vertices;
        std::vector<vec4> m_colors;
        std::vector<face> m_faces;
        std::vector<vec3> m_normals;

        Shader m_shader;
        VertexBuffer<vec3> m_verticesVBO;
        VertexBuffer<vec4> m_colorsVBO;
        VertexBuffer<face, true> m_facesVBO;
        VertexBuffer<vec3> m_normalsVBO;
        
        mat4 m_ModelMatrix;
    };
}

#endif /* Mesh_hpp */
