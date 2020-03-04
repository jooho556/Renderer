#pragma once

/*
All of OpenGL objects (Shader, Texture, ...) must inherit this base class 
to prevent data invalidation from copy constructors

Derived classes must:
 - have copy & move ctors explicitly calling their base copy / move ctors
 - check 'm_is_copied' value before deleting object
 */

class GLObject
{
public:
    GLObject() {};
    GLObject(const GLObject & object) { Copy(object); }
    GLObject(GLObject && object) { Copy(object); }
    virtual ~GLObject() {}

protected:
    void Copy(const GLObject & object) {
        m_id = object.m_id;
        object.m_is_copied = true;
    }

    unsigned int m_id = -1;
    mutable bool m_is_copied = false;
};