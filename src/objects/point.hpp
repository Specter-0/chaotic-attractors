#pragma once
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <vector>

#include "../class/shaders.hpp"

class Point
{
    public:
        glm::vec3 Pos;

        int capacity = 10; // max lenght of trail
        std::vector<glm::vec3> trail;

        Point(glm::vec3 Position);

        float magnitude();
    
    private:
        
};