#include "GL/glew.h"
#include <glm/gtc/type_ptr.hpp>
#include "EmptyCanvas.h"
#include "Constants.h"

float EmptyCanvas::rect_vertices[] = {
    -.5f, .5f, 0.f,
    -.5f, -.5f, 0.f,
    .5f, -.5f, 0.f,
    -.5f, .5f, 0.f,
    .5f, -.5f, 0.f,
    .5f, .5f, 0.f
};

EmptyCanvas::EmptyCanvas()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), &rect_vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glBindVertexArray(0);
}

EmptyCanvas::~EmptyCanvas()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void EmptyCanvas::Bind() const
{
    glBindVertexArray(VAO);
}

void EmptyCanvas::Unbind() const
{
    glBindVertexArray(0);
}

void EmptyCanvas::Draw(Shader& shader)
{
    shader.Use();
    glm::mat4 ndc_to_device = glm::mat4(1.f);
    shader.SetMat4("ndc_to_device", 
        glm::scale(glm::translate(ndc_to_device, glm::vec3(.5, .5, 0.)), glm::vec3(SCREEN_WIDTH, SCREEN_WIDTH, 0)));
    Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    Unbind();
}
