#include "GVertice.hpp"
#include "GObject.hpp"
#include <glm/glm.hpp>
#include "tower.hpp"

GObject* createPyramid() {

    // Vertices coordinates
    vector<GLfloat> verticesP({ 
        //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
        0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f, 1.0f,      0.0f, -1.0f, 0.0f, // Bottom side
        0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 1.0f,      0.0f, -1.0f, 0.0f, // Bottom side

        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 0.5f, 1.0f,     -0.8f, 0.5f,  0.0f, // Left Side

        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
        0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 1.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 0.5f, 1.0f,      0.0f, 0.5f, -0.8f, // Non-facing side

        0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
        0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 1.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 0.5f, 1.0f,      0.8f, 0.5f,  0.0f, // Right side

        0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 1.0f,      0.0f, 0.5f,  0.8f, // Facing side
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 1.0f, 1.0f,      0.0f, 0.5f,  0.8f, // Facing side
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 0.5f, 0.0f,      0.0f, 0.5f,  0.8f  // Facing side
    });

    // Indices for vertices order
    vector<GLuint> indicesP({ 
        0, 1, 2, // Bottom side
        0, 2, 3, // Bottom side
        4, 6, 5, // Left side
        7, 9, 8, // Non-facing side
        10, 12, 11, // Right side
        13, 15, 14 // Facing side
    });

    GObject *pyramid = new GObject();
    
    for(int i = 0; i < verticesP.size(); i += 11) {
        v3 coords = v3(verticesP[i], verticesP[i+1], verticesP[i+2]);
        v3 colors = v3(verticesP[i+3], verticesP[i+4], verticesP[i+5]);
        v2 texture = v2(verticesP[i+6], verticesP[i+7]);
        v3 normal = v3(verticesP[i+8], verticesP[i+9], verticesP[i+10]);
        GVertice v(coords, colors, texture, normal);
        pyramid->addVertice(v);
    }
    

    // Create indices
    for(int i = 0; i < indicesP.size(); i += 3) {
        pyramid->addTriangle(indicesP[i], indicesP[i+1], indicesP[i+2]);
    }
    return pyramid;
}

Tower* createTower(GLfloat height, GLfloat radius) {

    GLfloat circunference = 2.0f * M_PI * radius;
    int segments = 200;
    GLfloat segmentWidth = circunference / segments;

    v3 towerColor = v3(0.219f, 0.219f, 0.219f);

    Tower* tower = new Tower();
    tower->height = height;
    tower->radius = radius;
    GVertice  verCenter = GVertice(v3(0.0f, 0.0f, 0.0f), towerColor, v2(0.5f, 0.5f));
    GVertice  verTop = GVertice(v3(0.0f, height, 0.0f), towerColor + v3(0.5f, 0.5f, 0.5f), v2(4.0f, 4.0f));
    int vci = tower->addVertice(verCenter);
    int vti = tower->addVertice(verTop);

    float x = radius;
    float y = 0.0f;
    GVertice  ver1 = GVertice(v3(x, 0.0f, y), towerColor - v3(0.1f, 0.1f, 0.1f), v2(0.0f, 0.0f));
    int leftMi = tower->addVertice(ver1);

    float angle = 1.0f;
    while(true) {
        towerColor +=  angle > 180 ? -v3(0.002f, 0.002f, 0.002f) : v3(0.002f, 0.002f, 0.002f);
        if(angle > 360.0f) angle = 360.0f;

        float angleRadians = glm::radians(angle + 0.0f);
        x = radius * cos(angleRadians);
        y = radius * sin(angleRadians);
        GVertice  ver2 = GVertice(v3(x, 0.0f, y), towerColor - v3(0.1f, 0.1f, 0.1f), ver1.texture + v2(0.0f, angle > 180 ? -0.01f : 0.01f));
        int v2i = tower->addVertice(ver2);

        tower->addTriangle(leftMi, vci, v2i);
        tower->addTriangle(leftMi, vti, v2i);

        leftMi = v2i;
        ver1 = ver2;

        if(angle >= 360.0f) break;

        angle += (360/segments);
    }


    tower->generateNormals = true;
    return tower;
}

GObject* createWall() {
    GObject* wall = new GObject();

    v3 wallColor = v3(0.419f, 0.419f, 0.419f);

    GVertice ver1 = GVertice(v3(0.0f, 0.0f, 0.0f), wallColor, v2(0.0f, 0.0f));
    GVertice ver2 = GVertice(v3(0.0f, 1.0f, 0.0f), wallColor, v2(0.0f, 1.0f));

    int leftMi = wall->addVertice(ver1);
    int v2i = wall->addVertice(ver2);
    int v3i, v4i;

    for(int i = 1; i < 25; i++) {
        float x = i * 1.0f;
        
        // Calculate UV coordinates based on world coordinates
        // This ensures proper tiling
        GVertice ver3 = GVertice(v3(x, 0.0f, 0.0f), wallColor, v2(x, 0.0f));
        GVertice ver4 = GVertice(v3(x, 1.0f, 0.0f), wallColor, v2(x, 1.0f));

        v3i = wall->addVertice(ver3);
        v4i = wall->addVertice(ver4);

        wall->addTriangle(leftMi, v2i, v3i);
        wall->addTriangle(v2i, v3i, v4i);

        leftMi = v3i;
        v2i = v4i;
        ver1 = ver3;
        ver2 = ver4;
    }

    wall->scale(v3(16.0f, 300.0f, 1.0f));
    return wall;
}


GObject* createPlane(v3 color, float textureScale) {
    color /= 255.0f;
    GObject* wall = new GObject();

    GVertice ver1 = GVertice(v3(0.0f, 0.0f, 0.0f), color, v2(0.0f, 0.0f));
    GVertice ver2 = GVertice(v3(0.0f, 0.0f, 384.0f), color, v2(0.0f, textureScale * 384.0f));

    int leftMi = wall->addVertice(ver1);
    int v2i = wall->addVertice(ver2);
    int v3i, v4i;

    for(int i = 1; i < 25; i++) {
        float x = i * 1.0f;
        float z_max = 384.0f;
        
        // Calculate UV coordinates based on actual world coordinates
        // This ensures proper tiling regardless of geometry scale
        GVertice ver3 = GVertice(v3(x, 0.0f, 0.0f), color, v2(textureScale * x, 0.0f));
        GVertice ver4 = GVertice(v3(x, 0.0f, z_max), color, v2(textureScale * x, textureScale * z_max));

        v3i = wall->addVertice(ver3);
        v4i = wall->addVertice(ver4);

        wall->addTriangle(leftMi, v2i, v3i);
        wall->addTriangle(v2i, v3i, v4i);

        leftMi = v3i;
        v2i = v4i;
        ver1 = ver3;
        ver2 = ver4;
    }

    wall->scale(v3(16.0f, 100.0f, 1.0f));
    return wall;
}



GObject* createPlane() {
  return createPlane(v3(157, 163, 165), 1);
}

