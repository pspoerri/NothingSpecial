//
//  Mesh.cpp
//  NothingSpecial
//
//  Created by Pascal Spörri on 18/09/15.
//  Copyright © 2015 Pascal Spörri. All rights reserved.
//

#include "Mesh.hpp"
#include "GLWrapper.hpp"
#include "Geometry.hpp"
using namespace::Graphics;

#define SHADER_PATH "Graphics/shaders/"

Mesh::Mesh(const std::vector<vec3>& vertices,
           const std::vector<vec4>& colors,
           const std::vector<face>& faces):
    RenderableObject(),
    m_vertices(vertices),
    m_colors(colors),
    m_faces(faces),
    m_normals(Math::Geometry::computeVertexNormals(m_vertices, m_faces)),
    m_shader(BASE_PATH SHADER_PATH "diffuse.vert",
             BASE_PATH SHADER_PATH "diffuse.frag")
{
    GLWrapper::GLErrorThrow();
    m_shader.bind();
    m_verticesVBO.set(m_vertices);
    m_verticesVBO.map(m_shader, m_shader.attributeLocation("inPosition"), false);
    
    m_colorsVBO.set(m_colors);
    m_colorsVBO.map(m_shader, m_shader.attributeLocation("inColor"), false);
    GLWrapper::GLErrorThrow();

    m_facesVBO.set(m_faces);
    GLWrapper::GLErrorThrow();
    
    m_normalsVBO.set(m_normals);
    m_normalsVBO.map(m_shader, m_shader.attributeLocation("inNormal"), true);
    GLWrapper::GLErrorThrow();
    
    m_shader.unbind();
    GLWrapper::GLErrorThrow();
}

Mesh::~Mesh()
{
    
}

void
Mesh::render(const Camera& camera) const
{
    glEnable(GL_CULL_FACE);
    
    m_shader.bind();
    m_shader.setUniform("uViewMatrix", camera.view());
    m_shader.setUniform("uProjMatrix", camera.proj());
    m_shader.setUniform("uModelMatrix", modelMatrix());
    GLWrapper::GLErrorThrow();
    
    m_facesVBO.bind();
    glDrawElements(GL_TRIANGLES, m_facesVBO.length(), GL_UNSIGNED_INT, (void*) NULL);
    GLWrapper::GLErrorThrow();
    
    glBindVertexArray(0);
    GLWrapper::GLErrorThrow();
    
    glUseProgram(0);
    GLWrapper::GLErrorThrow();

    m_shader.unbind();
    glDisable(GL_CULL_FACE);
}