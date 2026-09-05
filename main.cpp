#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

class SerialPortManager {
public:
    HANDLE hSerial = INVALID_HANDLE_VALUE;
    bool isConnected = false;

    static std::vector<std::string> getAvailablePorts() {
        std::vector<std::string> ports;
        char portName[32];
        for (int i = 1; i <= 32; ++i) {
            std::string testName = "COM" + std::to_string(i);
            std::string fullPath = "\\\\.\\" + testName;
            HANDLE hTest = CreateFileA(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hTest != INVALID_HANDLE_VALUE) {
                ports.push_back(testName);
                CloseHandle(hTest);
            }
        }
        if (ports.empty()) {
            ports.push_back("COM3");
        }
        return ports;
    }

    bool connect(const std::string& portName, DWORD baudRate = CBR_115200) {
        std::string fullPath = "\\\\.\\" + portName;
        hSerial = CreateFileA(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hSerial == INVALID_HANDLE_VALUE) return false;

        DCB dcbParams = {0};
        dcbParams.DCBlength = sizeof(dcbParams);
        if (!GetCommState(hSerial, &dcbParams)) {
            disconnect();
            return false;
        }

        dcbParams.BaudRate = baudRate;
        dcbParams.ByteSize = 8;
        dcbParams.StopBits = ONESTOPBIT;
        dcbParams.Parity   = NOPARITY;
        if (!SetCommState(hSerial, &dcbParams)) {
            disconnect();
            return false;
        }

        COMMTIMEOUTS timeouts = {0};
        timeouts.ReadIntervalTimeout         = 10;
        timeouts.ReadTotalTimeoutConstant     = 10;
        timeouts.ReadTotalTimeoutMultiplier   = 2;
        timeouts.WriteTotalTimeoutConstant    = 50;
        timeouts.WriteTotalTimeoutMultiplier  = 5;
        SetCommTimeouts(hSerial, &timeouts);

        isConnected = true;
        return true;
    }

    void disconnect() {
        if (isConnected && hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            hSerial = INVALID_HANDLE_VALUE;
        }
        isConnected = false;
    }

    bool send(const std::string& data) {
        if (!isConnected) return false;
        DWORD bytesWritten = 0;
        return WriteFile(hSerial, data.c_str(), static_cast<DWORD>(data.length()), &bytesWritten, NULL);
    }

    std::string read() {
        if (!isConnected) return "";
        char buf[256];
        DWORD bytesRead = 0;
        if (ReadFile(hSerial, buf, sizeof(buf) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buf[bytesRead] = '\0';
            return std::string(buf);
        }
        return "";
    }
};

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(720, 520, "ESP32 Custom Mod Control Center", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.0f);
    style.Colors[ImGuiCol_Button]   = ImVec4(0.18f, 0.22f, 0.28f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    SerialPortManager serial;
    std::vector<std::string> portList = SerialPortManager::getAvailablePorts();
    int selectedPortIdx = 0;

    int powerDuty = 180;
    int cutoffMs = 5000;
    std::string terminalLogs = "[*] Engine ready. Select serial COM-port to initiate connection.\n";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (serial.isConnected) {
            std::string incoming = serial.read();
            if (!incoming.empty()) {
                terminalLogs += incoming;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("ESP32 Control Dashboard", NULL, 
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.6f, 1.0f), "SYSTEM STATUS: %s", 
                           serial.isConnected ? "CONNECTED (ONLINE)" : "DISCONNECTED");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Port Configuration:");
        if (!portList.empty()) {
            std::vector<const char*> portItems;
            for (const auto& p : portList) portItems.push_back(p.c_str());

            ImGui::SetNextItemWidth(140);
            ImGui::Combo("##PortsCombo", &selectedPortIdx, portItems.data(), static_cast<int>(portItems.size()));
        }

        ImGui::SameLine();
        if (ImGui::Button("Refresh Ports")) {
            portList = SerialPortManager::getAvailablePorts();
            selectedPortIdx = 0;
            terminalLogs += "[*] COM-port list refreshed.\n";
        }

        ImGui::SameLine();
        if (!serial.isConnected) {
            if (ImGui::Button("Connect", ImVec2(100, 0))) {
                if (!portList.empty() && serial.connect(portList[selectedPortIdx])) {
                    terminalLogs += "[+] Handshake OK: Linked to " + portList[selectedPortIdx] + "\n";
                } else {
                    terminalLogs += "[-] Connection error on " + portList[selectedPortIdx] + "\n";
                }
            }
        } else {
            if (ImGui::Button("Disconnect", ImVec2(100, 0))) {
                serial.disconnect();
                terminalLogs += "[*] Connection closed.\n";
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Power & Protection Configuration:");
        
        int percent = (powerDuty * 100) / 255;
        float approxVolts = 3.7f * ((float)powerDuty / 255.0f);
        float approxWatts = (approxVolts * approxVolts) / 1.0f;

        ImGui::SliderInt("PWM Duty (0-255)", &powerDuty, 0, 255);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Calculated Output: %d%% (~%.2f V | ~%.1f W @ 1.0 Ohm)", 
                           percent, approxVolts, approxWatts);

        ImGui::SliderInt("Cutoff Timer (ms)", &cutoffMs, 1000, 10000);

        ImGui::Spacing();
        if (ImGui::Button("Apply Parameters", ImVec2(160, 32))) {
            if (serial.isConnected) {
                std::string cmd = "SET:" + std::to_string(powerDuty) + ":" + std::to_string(cutoffMs) + "\n";
                serial.send(cmd);
                terminalLogs += "[>] Send: " + cmd;
            } else {
                terminalLogs += "[!] Hardware not connected.\n";
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Software Fire (1.5s)", ImVec2(160, 32))) {
            if (serial.isConnected) {
                serial.send("FIRE:1500\n");
                terminalLogs += "[>] Triggering manual test pulse: 1500ms\n";
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Live UART Monitor:");
        ImGui::SameLine(ImGui::GetWindowWidth() - 95);
        if (ImGui::Button("Clear Log")) {
            terminalLogs.clear();
        }

        ImGui::BeginChild("ConsoleScrollArea", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(terminalLogs.c_str());
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.05f, 0.05f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    serial.disconnect();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
