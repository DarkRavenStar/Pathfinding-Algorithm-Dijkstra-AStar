#pragma once

#include <ctime>
#include <string>
#include <iostream>
#include "bitmap.h"


const int TextureCount = 1;
GLuint GtextureID[TextureCount];
int guiTexW;
int guiTexH;

void loadTexture(const char* path, GLuint textureID)
{
    CBitmap bitmap(path);
    // Create Linear Filtered Texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Apply texture wrapping along horizontal part
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Apply texture wrapping along vertical part
                                                                  // bilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Near filtering. (For when texture needs to scale up)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Far filtering. (For when texture needs to scale down)


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.GetWidth(),
    bitmap.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.GetBits());

    guiTexW = bitmap.GetWidth();
    guiTexH = bitmap.GetHeight();

}

void InitTexture()
{
    glGenTextures(TextureCount, GtextureID);
    loadTexture("PathfindingGUI.bmp", GtextureID[0]);
}

void ButtonGUI(int x, int y, int scaleX, int scaleY)
{
    float xMin = (float)x;
    float xMax = (float)(xMin + guiTexW * scaleX);

    float yMin = (float)y;
    float yMax = (float)(yMin + guiTexH * scaleY);

    GLfloat vertices[] =
    {
        xMin, yMin, 0.0f,
        xMax, yMin, 0.0f,
        xMax, yMax, 0.0f,
        xMin, yMax, 0.0f,
    };

    GLfloat colors[] =
    {
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
    };

    GLfloat texCoords[] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GtextureID[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);

    glDrawArrays(GL_QUADS, 0, 12);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}
