#include <time.h>
#include "Window.h"
#include "Render.h"
#include "ShaderTest.h"
#include "Model.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

vec3 cameraPos   = vec3(0.0f, 0.0f,  3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp    = vec3(0.0f, 1.0f,  0.0f);

vec3 lightPos(1.2f, 1.0f, 2.0f);

bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;
float lastX =  SCR_WIDTH  / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool leftButton = false;

void button_callback(bool down)
{
    leftButton = down;
    if(down)
    {
        firstMouse = true;
    }
}

void mouse_callback(double xpos, double ypos)
{
    if (!leftButton)
    {
        return;
    }
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    cameraFront = normalize(front);
}

void processInput(std::shared_ptr<Window> window)
{
    if(window->Keys[VK_ESCAPE] == 1)
        window->SetShouldClose(true);
    
    float cameraSpeed = 2.5 * deltaTime; 
    if (window->Keys[VK_UP] == 1)
        cameraPos += cameraSpeed * cameraFront;
    if (window->Keys[VK_DOWN] == 1)
        cameraPos -= cameraSpeed * cameraFront;
    if (window->Keys[VK_LEFT] == 1)
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
    if (window->Keys[VK_RIGHT] == 1)
        cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
}

int main()
{
    std::shared_ptr<Window> window = std::make_shared<Window>();
    window->Create(SCR_WIDTH, SCR_HEIGHT, "W05");

    window->MakeContextCurrent(Renderer->Context);
    Renderer->Viewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    window->SetCursorPosCallback(mouse_callback);
    //window->SetScrollCallback(scroll_callback);
    window->SetLeftButtonCallback(button_callback);

    // ------------------------------------
    // vertex shader
    std::shared_ptr<VertexShaderTest> vertexShaderSource = std::make_shared<VertexShaderTest>();
    int vertexShader = Renderer->CreateShader(std::dynamic_pointer_cast<Shader>(vertexShaderSource));
    // fragment shader
    std::shared_ptr<FragmentShaderTest> fragmentShaderSource = std::make_shared<FragmentShaderTest>();
    int fragmentShader = Renderer->CreateShader(std::dynamic_pointer_cast<Shader>(fragmentShaderSource));
    // link shaders
    int ourShader = Renderer->CreateProgram();
    Renderer->AttachShader(ourShader, vertexShader);
    Renderer->AttachShader(ourShader, fragmentShader);
    Renderer->LinkProgram(ourShader);

    Renderer->DeleteShader(vertexShader);
    Renderer->DeleteShader(fragmentShader);

    Model ourModel("D:/CPP/SoftRenderer/Resource/nanosuit/nanosuit.obj");

    while (!window->ShouldClose())
    {
        // time
        float currentFrame = clock()/CLOCKS_PER_SEC;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        Renderer->ClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        Renderer->Clear();

        Renderer->UseProgram(ourShader);

        Renderer->Uniform3f(Renderer->GetUniformLocation(ourShader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);

        mat4 projection = perspective(radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        Renderer->UniformMatrix4fv(Renderer->GetUniformLocation(ourShader, "projection"), &projection);
        mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unsigned int viewLoc  = Renderer->GetUniformLocation(ourShader, "view");
        Renderer->UniformMatrix4fv(viewLoc, &view);

        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0f, -2.0f, 0.0f));
        model = scale(model, vec3(0.2f, 0.2f, 0.2f));
        Renderer->UniformMatrix4fv(Renderer->GetUniformLocation(ourShader, "model"), &model);
        ourModel.Draw(ourShader);

        // window: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window->SwapBuffer();
        window->PollEvents();
    }

    Renderer->DeleteProgram(ourShader);

    window->Terminate();

    return 0;
}