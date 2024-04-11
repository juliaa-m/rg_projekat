#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void renderCube();
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 750;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

};

struct DirLight{
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct ProgramState {
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 deadpoolPosition = glm::vec3(-3.5f, -0.25f, -1.0f);
    glm::vec3 wolverinePosition = glm::vec3(0.0f, -0.2f, -1.0f);
    glm::vec3 streetlightPosition = glm::vec3(-2.0f, -0.2f, -2.0f);
    glm::vec3 lightpPosition1 = glm::vec3(0.5f, 4.5f, 7.0f);
    glm::vec3 lightpPosition2 = glm::vec3(10.5f, 4.5f, 7.0f);
    glm::vec3 container1Position = glm::vec3(-7.0f, 1.9f, -8.0f);
    glm::vec3 container2Position = glm::vec3(-11.0f, 1.9f, -7.0f);
    glm::vec3 container3Position = glm::vec3(-11.0f, 6.0f, -8.0f);
    glm::vec3 container4Position = glm::vec3(-7.0f, 1.9f, 7.0f);
    glm::vec3 container5Position = glm::vec3(0.0f, 1.9f, -15.0f);
    glm::vec3 container6Position = glm::vec3(7.0f, 1.9f, -9.0f);
    glm::vec3 container7Position = glm::vec3(14.0f, 4.0f, -5.0f);
    glm::vec3 container8Position = glm::vec3(-26.0f, 1.9f, -2.0f);
    glm::vec3 buildingPosition1 = glm::vec3(10.0f, 5.5f, 10.0f);
    glm::vec3 buildingPosition2 = glm::vec3(20.0f, 5.5f, 10.0f);
    glm::vec3 buildingPosition3 = glm::vec3(30.0f, 5.5f, 10.0f);
    glm::vec3 buildingPosition4 = glm::vec3(29.9f, 5.5f, -1.5f);
    glm::vec3 buildingPosition5 = glm::vec3(29.9f, 5.5f, -11.5f);
    glm::vec3 buildingPosition6 = glm::vec3(29.9f, 5.5f, -21.5f);
    glm::vec3 buildingPosition7 = glm::vec3(16.0f, 5.5f, -20.0f);
    glm::vec3 groundPosition1 = glm::vec3(-25.0f, -1.71f, -2.5f);
    glm::vec3 groundPosition2 = glm::vec3(20.0f, -1.7f, -2.5f);
    glm::vec3 groundPosition3 = glm::vec3(-25.0f, -1.71f, -25.3f);
    glm::vec3 groundPosition4 = glm::vec3(20.0f, -1.7f, -25.3f);
    glm::vec3 groundPosition5 = glm::vec3(-25.0f, -1.71f, 20.4f);
    glm::vec3 groundPosition6 = glm::vec3(20.0f, -1.7f, 20.4f);
    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

bool blinn = false;
bool blinnKeyPressed = false;
bool bloom = true;
bool bloomKeyPressed = false;
bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 0.8f;

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);




    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }



    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");


    //blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    // shaders
    Shader modelShader("resources/shaders/model.vs", "resources/shaders/model.fs");
    Shader blendShader("resources/shaders/blend.vs", "resources/shaders/blend.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader blurShader("resources/shaders/blur.vs", "resources/shaders/blur.fs");
    Shader finalShader("resources/shaders/final.vs", "resources/shaders/final.fs");


    // models
    Model deadpool("resources/objects/deadpool/scene.gltf");
    deadpool.SetShaderTextureNamePrefix("material.");
    Model wolverine("resources/objects/wolverine/scene.gltf");
    wolverine.SetShaderTextureNamePrefix("material.");
    Model streetlight("resources/objects/streetlight/scene.gltf");
    streetlight.SetShaderTextureNamePrefix("material.");
    Model container1("resources/objects/container/scene.gltf");
    container1.SetShaderTextureNamePrefix("material.");
    Model container2("resources/objects/container/scene.gltf");
    container1.SetShaderTextureNamePrefix("material.");
    Model container3("resources/objects/container/scene.gltf");
    container1.SetShaderTextureNamePrefix("material.");
    Model building("resources/objects/building/scene.gltf");
    building.SetShaderTextureNamePrefix("material.");
    Model ground1("resources/objects/cobblestone/scene.gltf");
    ground1.SetShaderTextureNamePrefix("material.");
    Model lightp("resources/objects/lightpole/scene.gltf");
    lightp.SetShaderTextureNamePrefix("material.");

    unsigned glassTexture = loadTexture("resources/textures/glass.png");

    stbi_set_flip_vertically_on_load(true);


    //PointLight positions
    std::vector<glm::vec3> pointLightPositions;
    pointLightPositions.push_back(glm::vec3(-2.0f, 2.0, -1.0f));
    pointLightPositions.push_back(glm::vec3(0.5f, 4.5f, 5.9f));
    pointLightPositions.push_back(glm::vec3(10.5f, 4.5f, 5.9f));

    //Point Light
    PointLight pointLight;
    //pointLight.position = glm::vec3(-2.0f, 2.0, -1.0f);
    pointLight.ambient = glm::vec3(2.5f, 1.3f, 0.3f);
    pointLight.diffuse = glm::vec3(5.5f, 3.7f, 1.0f);
    pointLight.specular = glm::vec3(5.5f, 3.7f, 1.0f);


    //Direct Light
    DirLight dirlight;
    dirlight.direction = glm::vec3(-0.7f, -1.0f, -1.0);
    dirlight.ambient = glm::vec3(0.01f);
    dirlight.diffuse = glm::vec3(0.3f);
    dirlight.specular = glm::vec3(0.7f);



    //prison
    float prisonCube[] = {
            // positions          // texture Coords
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    // cube VAO
    unsigned int prisonVAO, prisonVBO;
    glGenVertexArrays(1, &prisonVAO);
    glGenBuffers(1, &prisonVBO);
    glBindVertexArray(prisonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, prisonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(prisonCube), &prisonCube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


    //HDR & BLOOM
    // configure (floating point) framebuffers
    // ---------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    //check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        //check if framebuffers are complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    //skybox
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    stbi_set_flip_vertically_on_load(false);
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/graycloud_ft.jpg"),
                    FileSystem::getPath("resources/textures/skybox/graycloud_bk.jpg"),
                    FileSystem::getPath("resources/textures/skybox/graycloud_up.jpg"),
                    FileSystem::getPath("resources/textures/skybox/graycloud_dn.jpg"),
                    FileSystem::getPath("resources/textures/skybox/graycloud_rt.jpg"),
                    FileSystem::getPath("resources/textures/skybox/graycloud_lf.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);
    stbi_set_flip_vertically_on_load(true);


    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    blendShader.use();
    blendShader.setInt("texture1", 0);
    finalShader.use();
    finalShader.setInt("hdrBuffer", 0);
    finalShader.setInt("bloomBlur", 1);
    blurShader.use();
    blurShader.setInt("image", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // render
        // ------
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(0.41961f, 0.36863f, 0.28235f, 0.1f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //render scene into floating point framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        modelShader.use();

        for(unsigned int i = 0; i < pointLightPositions.size(); i++){
            modelShader.setVec3("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            modelShader.setVec3("pointlight[" + std::to_string(i) + "].ambient", pointLight.ambient * 0.05f);
            modelShader.setVec3("pointlight[" + std::to_string(i) + "].diffuse", pointLight.diffuse * 0.7f);
            modelShader.setVec3("pointlight[" + std::to_string(i) + "].specular", pointLight.specular * 0.0f);
        }

        modelShader.setVec3("viewPos", programState->camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        //DirLight
        modelShader.setVec3("dirlight.direction", dirlight.direction);
        modelShader.setVec3("dirlight.ambient", dirlight.ambient);
        modelShader.setVec3("dirlight.diffuse", dirlight.diffuse);
        modelShader.setVec3("dirlight.specular", dirlight.specular);
        modelShader.setBool("blinn", blinn);


        glm::mat4 model = glm::mat4(1.0f);

        //SIDENOTE: these objects had issues rendering with Face Culling, hence that technique is not applied here
        // render model: building
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition1);
        model = glm::scale(model, glm::vec3(0.6f));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition2);
        model = glm::scale(model, glm::vec3(0.6f));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition3);
        model = glm::scale(model, glm::vec3(0.6f));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition4);
        model = glm::scale(model, glm::vec3(0.6f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition5);
        model = glm::scale(model, glm::vec3(0.6f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition6);
        model = glm::scale(model, glm::vec3(0.6f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->buildingPosition7);
        model = glm::scale(model, glm::vec3(0.6f));
        model = glm::rotate(model, (float)glm::radians(180.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        building.Draw(modelShader);


        //face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // render model: deadpool
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->deadpoolPosition);
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0,1, 0));
        model = glm::scale(model, glm::vec3(0.2f));
        modelShader.setMat4("model", model);
        deadpool.Draw(modelShader);

        // render model: wolverine
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->wolverinePosition);
        model = glm::rotate(model, (float)glm::radians(-90.0), glm::vec3(0,1, 0));
        model = glm::scale(model, glm::vec3(1.0f));
        modelShader.setMat4("model", model);
        wolverine.Draw(modelShader);

        // render model: streetlight
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->streetlightPosition);
        model = glm::scale(model, glm::vec3(0.012f));
        modelShader.setMat4("model", model);
        streetlight.Draw(modelShader);

        // render model: containers
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container1Position);
        model = glm::scale(model, glm::vec3(1.5f));
        modelShader.setMat4("model", model);
        container1.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container2Position);
        model = glm::scale(model, glm::vec3(1.5f));
        modelShader.setMat4("model", model);
        container2.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container3Position);
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, (float)glm::radians(18.0), glm::vec3(0,1, 0));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container4Position);
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, (float)glm::radians(45.0), glm::vec3(0,1, 0));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container5Position);
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, (float)glm::radians(45.0), glm::vec3(0,1, 0));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container6Position);
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, (float)glm::radians(-37.0), glm::vec3(0,1, 0));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container7Position);
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, (float)glm::radians(43.0), glm::vec3(0,1, 0));
        model = glm::rotate(model, (float)glm::radians(19.3), glm::vec3(1,0, 0));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->container8Position);
        model = glm::scale(model, glm::vec3(1.5f));
        modelShader.setMat4("model", model);
        container3.Draw(modelShader);


        // render model: ground
        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition1);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition2);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition3);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition4);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition5);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->groundPosition6);
        model = glm::scale(model, glm::vec3(5.0f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 0 , 1));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        ground1.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->lightpPosition1);
        model = glm::scale(model, glm::vec3(0.005f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(1, 0 , 0));
        model = glm::rotate(model, (float)glm::radians(-180.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        lightp.Draw(modelShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model,programState->lightpPosition2);
        model = glm::scale(model, glm::vec3(0.005f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(1, 0 , 0));
        model = glm::rotate(model, (float)glm::radians(-180.0), glm::vec3(0, 1 , 0));
        modelShader.setMat4("model", model);
        lightp.Draw(modelShader);


        glDisable(GL_CULL_FACE);


        //cube prison
        blendShader.use();
        blendShader.setMat4("projection", projection);
        blendShader.setMat4("view", view);
        glBindVertexArray(prisonVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glassTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 0.9, -0.7));
        model = glm::scale(model, glm::vec3(2.3f));
        blendShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // blur bright fragments with two-pass Gaussian Blur
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        finalShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        finalShader.setInt("bloom", bloom);
        finalShader.setInt("hdr", hdr);
        finalShader.setFloat("exposure", exposure);

        renderQuad();



        if (programState->ImGuiEnabled)
            DrawImGui(programState);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        programState->camera.Position.x = -4.0f;
        programState->camera.Position.y = 2.0f;
        programState->camera.Position.z = 4.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed){
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE){
        blinnKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hdrKeyPressed)
    {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
    {
        hdrKeyPressed = false;
    }

    //bloom
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::DragFloat3("Backpack position", (float*)&programState->deadpoolPosition);
        //ImGui::DragFloat("Backpack scale", &programState->backpackScale, 0.05, 0.1, 4.0);

        ImGui::End();
    }

    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube(){
    // initialize (if necessary)
    if (cubeVAO == 0){
        float vertices[] = {
                //position                      //normals                           //texture coords
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

//2D textures
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}