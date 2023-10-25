#include "sphere.h"
#include <iostream>
#define PI 3.14159265359

Sphere::Sphere(glm::vec3 center, int radius, Shader shader)
	:Mesh(shader), center(center), radius(radius)
{
    // taken from 
    // http://www.songho.ca/opengl/gl_sphere.html

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    int sectorCount = 36;
    int stackCount = 18;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;
    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            glm::vec3 pos(x, y, z);

            // normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            glm::vec3 normal(nx, ny, nz);

            // vertex tex coord between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            glm::vec2 texCoord(s, t);

            vertices.push_back(Vertex(pos + center, normal, texCoord));
        }
    }

    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2); 
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }

            // vertical lines for all stacks
            //lineIndices.push_back(k1);
            //lineIndices.push_back(k2);
            //if (i != 0)  // horizontal lines except 1st stack
            //{
            //    lineIndices.push_back(k1);
            //    lineIndices.push_back(k1 + 1);
            //}
        }
    }
}

void Sphere::init()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(shader.getAttribLocation("pos"));
    glEnableVertexAttribArray(shader.getAttribLocation("normal"));
    glEnableVertexAttribArray(shader.getAttribLocation("uv"));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos) + sizeof(vertices[0].normal)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
