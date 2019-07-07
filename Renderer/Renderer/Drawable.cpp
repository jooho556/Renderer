#include <GL/glew.h>
#include <iostream>
#include "Drawable.h"

Drawable::Drawable(float vertices[], int num_v, unsigned int indices[], int num_i, Shader * shader)
    : VAO(0), vertex_num(num_v), element_num(num_i), m_shader(shader)
{
    //VAO stores:
    // 1. 'glEnable(Disable)VertexAttribArray' is called or not?
    // 2. Vertex attribute configuration via 'glVertexAttribPointer'
    // 3. VBO associated with vertex attribute by calls to 'glVertexAttribPointer'
    // 4. EBO (and its bind / unbind calls) --------> DO NOT UNBIND EBO BEFORE UNBINDING VAO!!
    //                                                                              -> if not, EBO configuration would not be stored in VAO.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //After creating this & bounding VBO & vertex attribute configuration,
    //we can simply bound VAO from now on, not re-bounding VBO or resetting configuration.

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * num_i, indices, GL_STATIC_DRAW);

    //Buffer object for openGL
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //BOUNDED!!  <----------------------------------------------------//
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_v, vertices, GL_STATIC_DRAW);                              //  
                                                                                                                                                                              //
    //Vertex attributes takes its data from VBO which are bounded to GL_ARRAY_BUFFER recently. //
    glVertexAttribPointer( //Position
        0, // layout (location = 0) in vec3 Pos
        3, // vec'3'
        GL_FLOAT, // type of data 
        GL_FALSE, // make data normalized or not
        sizeof(float) * 6, // Stride - Offset from current attribute(position) to the next set of attribute(position) 
        (void*)0 // where the current attribute begins in the buffer.
    );
    glEnableVertexAttribArray(0);

    //Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Drawable::~Drawable()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Drawable::Bind() const { glBindVertexArray(VAO); }
void Drawable::Unbind() const { glBindVertexArray(0); }
