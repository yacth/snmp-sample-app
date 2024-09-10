#include "snmp_pp/snmp_pp.h"
#include "snmp_pp/libsnmp.h"
#include "sensuron_agent.h"

#ifdef SNMP_PP_NAMESPACE
using namespace Snmp_pp;
#endif

#include <iostream>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>

void displayAsciiChar(const char *inputBuffer, char *outputBuffer, int bufferSize, int charsPerLine)
{
  int count = 0;
  std::stringstream ss;
  for (int i = 0; i < bufferSize; ++i) // Display printable ASCII characters
  {
    char c = static_cast<char>(*(inputBuffer + i));
    ss << std::hex << std::setw(2) << std::setfill('0') << (0xFF & c) << " ";
    count++;

    if (count >= charsPerLine)
    {
      ss << "\n";
      count = 0;
    }
  }

  std::string stringBuffer = ss.str();
  std::strcpy(outputBuffer, stringBuffer.c_str());
}

void main_loop(GLFWwindow *window, ImGuiIO &io)
{

  DefaultLog::log()->set_filter(ERROR_LOG, 0);
  DefaultLog::log()->set_filter(DEBUG_LOG, 0); // Set logging level of DEBUG to 0 to suppress debug logging

  sensuron::SensuronAgent *sensuronAgent = new sensuron::SensuronAgent();
  sensuronAgent->init();

  static sensuron::sensuronPayload_t sensuronPayload{0};

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  // Main loop

  ImFont *regularLatoFont = io.Fonts->AddFontFromFileTTF("fonts/Lato-Regular.ttf", 16.0f);
  ImFont *boldLatoFont = io.Fonts->AddFontFromFileTTF("fonts/Lato-Bold.ttf", 18.0f);

  io.Fonts->Build();

  char commandBuffer[PAYLOAD_SIZE] = "Command Data (edit)";

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow(&show_demo_window);
    //  2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

    // // Set the window position to the top-left corner of the application window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // Set the window size to the size of the application window
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    /*##################################################*/
    ImGui::PushFont(boldLatoFont);
    ImGui::Begin("SensuronControl", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    // ImGui::Begin("SensuronControl");

    ImGui::SeparatorText("Parameters");
    ImGui::PopFont();

    static char ipString[128] = SENSURON_IP_ADDR;
    ImGui::InputTextWithHint("Sensuron IP", "Sensuron IP", ipString, IM_ARRAYSIZE(ipString));
    static char portString[128] = SENSURON_PORT;
    ImGui::InputTextWithHint("Sensuron IP Port", "Sensuron IP Port", portString, IM_ARRAYSIZE(portString));
    static char inputOid[128] = SENSURON_RECEIVE_DATA_OID;
    ImGui::InputTextWithHint("Input OID", "Input OID", inputOid, IM_ARRAYSIZE(inputOid));
    static char outputOid[128] = SENSURON_SEND_DATA_OID;
    ImGui::InputTextWithHint("Output OID", "Output OID", outputOid, IM_ARRAYSIZE(outputOid));

    ImGui::PushFont(boldLatoFont);
    bool isStarted = ImGui::Button("Start");
    ImGui::PopFont();

    if (isStarted && strlen(ipString) != 0 && strlen(portString) != 0 && strlen(inputOid) != 0 && strlen(outputOid) != 0)
    {
      sensuronAgent->stop();
      sensuronAgent->setSensuronIpAddress(ipString);
      sensuronAgent->setSensuronPort(portString);
      sensuronAgent->setReceiveDataOid(inputOid);
      sensuronAgent->setSendDataOid(outputOid);
      sensuronAgent->init();
    }
    /*##################################################*/
    ImGui::PushFont(boldLatoFont);
    ImGui::SeparatorText("Input data");
    ImGui::PopFont();
    ImGui::InputTextMultiline("##InputData", &commandBuffer[0], sizeof(commandBuffer), ImVec2(-100, ImGui::GetTextLineHeight() * 16));

    ImGui::PushFont(boldLatoFont);
    bool sendCommandBuffer = ImGui::Button("Send command");
    ImGui::PopFont();
    if (sendCommandBuffer)
    {
      sensuronAgent->setCommand(commandBuffer);
      sensuronAgent->sendCommand();
      sensuronAgent->receivePayload();
      sensuronPayload = sensuronAgent->getPayload();
    }
    /*##################################################*/

    ImGui::PushFont(boldLatoFont);
    ImGui::SeparatorText("Output data");
    ImGui::PopFont();

    char payload1Buffer[SINGLE_UINT_PAYLOAD] = {1, 2, 3, 4};
    static char payload1HexData[SINGLE_UINT_PAYLOAD] = {};
    memcpy(payload1Buffer, &sensuronPayload.payload1, sizeof(sensuronPayload.payload1));
    displayAsciiChar(payload1Buffer, payload1HexData, sizeof(payload1HexData), 16);

    ImGui::InputTextWithHint("UINT32", "Payload1", payload1HexData, IM_ARRAYSIZE(payload1HexData), ImGuiInputTextFlags_ReadOnly);

    char payload2Buffer[SINGLE_FLOAT_PAYLOAD] = {};
    static char payload2HexData[SINGLE_FLOAT_PAYLOAD] = {};
    memcpy(payload2Buffer, &sensuronPayload.payload2, sizeof(sensuronPayload.payload2));
    displayAsciiChar(payload2Buffer, payload2HexData, sizeof(payload2HexData), 16);

    ImGui::InputTextWithHint("FLOAT32", "Payload2", payload2HexData, IM_ARRAYSIZE(payload2HexData), ImGuiInputTextFlags_ReadOnly);

    std::stringstream payload3StringStream;

    int charsPerLine = 16; // Number of characters per line
    int count = 0;
    char payload3Buffer[ARRAY_FLOATS_PAYLOAD] = {};
    memcpy(payload3Buffer, &sensuronPayload.payload3, sizeof(sensuronPayload.payload3));

    for (int i = 0; i < sizeof(payload3Buffer); ++i) // Display printable ASCII characters
    {
      char c = static_cast<char>(*(payload3Buffer + i));
      payload3StringStream << std::hex << std::setw(2) << std::setfill('0') << (0xFF & c) << " ";
      count++;

      if (count >= charsPerLine)
      {
        payload3StringStream << "\n";
        count = 0;
      }
    }

    std::string payload3HexData = payload3StringStream.str();
    ImGui::InputTextMultiline("FLOAT[2048]", &payload3HexData[0], payload3HexData.size(), ImVec2(-100, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);

    /*##################################################*/

    ImGui::End();

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
}

int main(int, char **)
{
  // Initialize GLFW
  if (!glfwInit())
    return -1;

  // Create a windowed mode window and its OpenGL context
  GLFWwindow *window = glfwCreateWindow(480, 740, "SNMP++ Sample App", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader (GLAD in this case)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
    return -1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Load default font
  io.Fonts->AddFontDefault();

  // Setup Dear ImGui style
  ImGui::StyleColorsLight();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  main_loop(window, io);

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
