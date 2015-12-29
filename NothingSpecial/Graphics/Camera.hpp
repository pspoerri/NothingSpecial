//
//  Camera.hpp
//  NothingSpecial
//
//  Created by Pascal Spörri on 29/12/15.
//  Copyright © 2015 Pascal Spörri. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include "base.hpp"

namespace Graphics {
    class Camera
    {
    public:
        Camera(float width, float height);
        Camera(float width, float height, float angle, float near, float far);
        
        void resize(float width, float height);
        
        const mat4& view() const;
        const mat4& proj() const;
        
        void move(const vec3& dir);
        
        static mat4 createPerspectiveMatrix(float aspect, float angle, float near, float far);
        static mat4 createLookAtMatrix(const vec3& eye, const vec3& center, const vec3& up);
        
    private:
        float m_angle;
        float m_aspect;
        float m_near;
        float m_far;
        
        mutable mat4 m_viewMatrix;
        mutable bool m_viewMatrixDirty;
        
        mutable mat4 m_projMatrix;
        mutable bool m_projMatrixDirty;
        
        vec3 m_position;
        vec3 m_up;
        vec3 m_lookAt;
    };
}

#endif /* Camera_hpp */
