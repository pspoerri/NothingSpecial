//
//  Volume.cpp
//  NothingSpecial
//
//  Created by Pascal Spörri on 30/12/15.
//  Copyright © 2015 Pascal Spörri. All rights reserved.
//

#include "Volume.hpp"
#include "Geometry.hpp"

using namespace Graphics;

#define SHADER_PATH "Graphics/shaders/"

Volume::Volume(std::shared_ptr<Math::Grid3D<float>> data):
    m_data(data),
    m_shaderVolume(BASE_PATH SHADER_PATH "volume.vert",
                   BASE_PATH SHADER_PATH "volume.frag"),
    m_shaderBackface(BASE_PATH SHADER_PATH "backfaceMap.vert",
                     BASE_PATH SHADER_PATH "backfaceMap.frag"),
    m_textureBackface({{GL_TEXTURE_BASE_LEVEL, 0},
                    {GL_TEXTURE_MAX_LEVEL, 0},
                    {GL_TEXTURE_WRAP_S, GL_REPEAT},
                      {GL_TEXTURE_WRAP_T, GL_REPEAT},
                      {GL_TEXTURE_MIN_FILTER, GL_NEAREST},
                      {GL_TEXTURE_MAG_FILTER, GL_NEAREST}}),
    m_textureBuffer({{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
                     {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                     {GL_TEXTURE_WRAP_S, GL_REPEAT},
                     {GL_TEXTURE_WRAP_T, GL_REPEAT},
                     {GL_TEXTURE_WRAP_R, GL_REPEAT}})
{
    m_textureBuffer.set((*m_data).raw(), (*m_data).dimX(), (*m_data).dimY(), (*m_data).dimZ());
    float max = std::max((*m_data).dimX(), std::max((*m_data).dimY(), (*m_data).dimZ()));
    GLWrapper::GLErrorThrow();
    
    glGenFramebuffers(1, &m_frameBufferIdx);
    GLWrapper::GLErrorThrow();

    initCube();
    
    // Center the volume
    m_ModelMatrix(0,3) = -0.5;
    m_ModelMatrix(1,3) = -0.5;
    m_ModelMatrix(2,3) = -0.5;
    
    // Scale the volume
    m_ModelMatrix(0,0) = (*m_data).dimX()/max;
    m_ModelMatrix(1,1) = (*m_data).dimY()/max;
    m_ModelMatrix(2,2) = (*m_data).dimZ()/max;
    m_ModelMatrix *= mat4::Identity()*8.0;
    m_ModelMatrix(3,3) = 1.0;
}

void
Volume::render(const Camera &camera) const
{
    renderBackFace(camera);
    renderVolume(camera);
}

void
Volume::renderBackFace(const Graphics::Camera &camera) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferIdx);

    m_textureBackface.SetData(nullptr, camera.width(), camera.height());
    GLWrapper::GLErrorThrow();
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_textureBackface.textureBuffer(), 0);
    GLWrapper::GLErrorThrow();
    
    // Render the back face to a texture
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    GLWrapper::GLErrorThrow();

    m_shaderBackface.bind();
    m_shaderBackface.setUniform("uViewMatrix", camera.view());
    m_shaderBackface.setUniform("uProjMatrix", camera.proj());
    m_shaderBackface.setUniform("uModelMatrix", modelMatrix());
    GLWrapper::GLErrorThrow();

    m_cubeFacesVBO.bind();
    glDrawElements(GL_TRIANGLES, m_cubeFacesVBO.length(), GL_UNSIGNED_INT, (void*) NULL);
    GLWrapper::GLErrorThrow();
    m_cubeFacesVBO.unbind();
    
    m_shaderBackface.unbind();
    
    glDisable(GL_CULL_FACE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
Volume::renderVolume(const Graphics::Camera &camera) const
{
    // Render the volume with the back face as a reference
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    m_shaderVolume.bind();
    m_shaderVolume.setUniform("uViewMatrix", camera.view());
    m_shaderVolume.setUniform("uProjMatrix", camera.proj());
    m_shaderVolume.setUniform("uModelMatrix", modelMatrix());
    m_shaderVolume.setUniform("uWindowSize", vec2(camera.width(), camera.height()));
    m_shaderVolume.setUniform("uVolumeTex", &m_textureBuffer);
    m_shaderVolume.setUniform("uBackFace",  &m_textureBackface);
    GLWrapper::GLErrorThrow();
    
    m_cubeFacesVBO.bind();
    glDrawElements(GL_TRIANGLES, m_cubeFacesVBO.length(), GL_UNSIGNED_INT, (void*) NULL);
    GLWrapper::GLErrorThrow();

    m_cubeFacesVBO.unbind();
    
    m_shaderVolume.unbind();
    
    glDisable(GL_CULL_FACE);
}

void
Volume::initCube()
{
    std::vector<vec3> vertices;
    std::vector<face> faces;
    Math::Geometry::generateCube(vertices, faces);
    
    // Rescale the vertices to the range 0,1
    for (auto& v:vertices)
    {
        v = 0.5*(v+vec3(1,1,1));
    }
    
    m_cubeVerticesVBO.set(vertices);
    m_cubeVerticesVBO.map(m_shaderBackface, m_shaderBackface.attributeLocation("inPosition"), false);
    m_cubeVerticesVBO.map(m_shaderVolume, m_shaderVolume.attributeLocation("inPosition"), false);
    
    m_cubeFacesVBO.set(faces);
}