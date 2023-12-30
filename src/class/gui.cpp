#include "gui.hpp"

Gui::Gui(GLFWwindow* window, Camera* camera, std::string GLSL_version)
{
    Gui::window = window;
    Gui::camera = camera;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(Gui::window, true);
    ImGui_ImplOpenGL3_Init(GLSL_version.c_str());

    ImGui::StyleColorsDark();
}

void Gui::newframe()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Gui::render(float* scalar, float* speed, int* lossCount, float* pointSize)
{
    if (open)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        ImGui::Begin(" Config ", nullptr, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar()) 
        {
            if (ImGui::MenuItem("Close")) open = false; 

            if (ImGui::BeginMenu("Menu")) 
            {
                if (!constantsOpen) {
                    if (ImGui::MenuItem("View Constants")) constantsOpen = true; 
                } else if (ImGui::MenuItem("close Constants")) constantsOpen = false; 

                if (!attractorSelect) {
                    if (ImGui::MenuItem("View Attractors")) attractorSelect = true; 
                } else if (ImGui::MenuItem("close Attractors")) attractorSelect = false; 
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) 
            {
                if (doCull) {
                    if (ImGui::MenuItem("Disable particle Culling")) doCull = false; 
                } else if (ImGui::MenuItem("Enable particle Culling")) doCull = true; 
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Camera")) 
            {
                if (ImGui::MenuItem("Nothing here")) std::cout << "UwU";
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::ColorEdit4("Color", (float*)&clearColor);

        ImGui::SliderFloat("Scalar", scalar, 0.01, 100.0);
        ImGui::SliderFloat("speed", speed, 0.0, 10.0);
        ImGui::SliderFloat("Point Size", pointSize, 0.1, 100.0);

        if (doCull) {
            ImGui::Text("Loss Count");
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.2f, 1.0f), "Loss %s / %s Total", std::to_string(*lossCount).c_str(), std::to_string(PointsInital.size()).c_str());
        } else ImGui::Text("%s Total, Culling is disabled", std::to_string(PointsInital.size()).c_str());

        if (ImGui::Button("Reset")) {
            *Points = PointsInital;
            *lossCount = 0;
        }

        ImGui::End();

        if (constantsOpen) 
            renderConstants();
        
        if (attractorSelect) 
            renderAttractorSelect();
    } 
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::ProcessInput()
{
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) open = true;
}

void Gui::setPointsArray(std::vector<glm::vec3>* PointsRef)
{
    Points = PointsRef;
    PointsInital = *PointsRef;
}


void Gui::setEquation(Equations newEquation)
{
    equation = newEquation;
    
    constants.clear();

    std::string letters = "abcdef";
    for (unsigned int i = 0; i < equationConstants[newEquation].size(); i++)
    {
        datapoint data = equationConstants[newEquation][i];
        constants[letters.substr(i, 1)] = Constant{data.value, data.min, data.max, data.value, false, 0.1f};
    }
}

void Gui::renderConstants()
{
    ImGui::Begin(" Constants ", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::MenuItem("Close")) 
            constantsOpen = false; 
        ImGui::EndMenuBar();
    }


    if (constants.size() > 0) {
        for (const auto& pair : constants) 
        {
            ImGui::SliderFloat(pair.first.c_str(), &constants[pair.first].value, constants[pair.first].min, constants[pair.first].max);
            
            if (ImGui::Button((std::string("Reset ") + pair.first).c_str())) 
                constants[pair.first].value = constants[pair.first].inital;
            
            ImGui::SameLine(); 
            
            ImGui::Checkbox((std::string("Scale ") + pair.first).c_str(), &constants[pair.first].scaling);
            ImGui::SameLine(); 
            ImGui::SetNextItemWidth(60);
            ImGui::InputFloat((std::string("Speed ") + pair.first).c_str(), &constants[pair.first].scalingSpeed);
        }
    } else ImGui::Text("No constants created");

    ImGui::End();
}

void Gui::updateScalingConstants()
{
    for (const auto& pair : constants) 
    {
        if (constants[pair.first].scaling) {
            constants[pair.first].value += constants[pair.first].scalingSpeed;
            
            if (constants[pair.first].value > constants[pair.first].max) {
                constants[pair.first].value = constants[pair.first].max;
            }
        }
    }
}

void Gui::renderAttractorSelect()
{
    ImGui::Begin(" Attractors ", nullptr, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::MenuItem("Close")) 
            attractorSelect = false; 
        
        ImGui::EndMenuBar();
    }

    if (ImGui::BeginCombo("Options", equationNames[equation])) {
        for (int i = 0; i < Equations::CUBE + 1; ++i) {
            bool isSelected = (equation == Equations(i));

            if (ImGui::Selectable(equationNames[i], isSelected)) setEquation(Equations(i));

            if (isSelected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::End();
}