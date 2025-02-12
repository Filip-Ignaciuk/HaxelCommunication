#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <glfw3.h> // Will drag system OpenGL headers

// Application Headers
#include "Chatroom.hpp"
#include "Config.hpp"
#include "User.hpp"
#include "ErrorHandler.hpp"
#include "GuiLanguage.hpp"
#include "Languages.hpp"
#include "Storage.hpp"

#include "WindowsNetworking.hpp"
#include "WindowsNetworkCallsCreator.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#define IMGUI_ENABLE_FREETYPE

// Server

// Application
NetworkCallsCreator* creator;
NetworkCalls* networkCalls;
std::vector<std::string> allConsoleTexts;

// Status
static std::string currentStatus;
static ImVec4 currentColour;
static ImVec4 white{ 1,1,1,1 };
static ImVec4 red{ 1.0f, 0.0f, 0.0f ,1.0f};
static ImVec4 yellow{ 1.0f, 1.0f, 0.0f , 1.0f};
static ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f};

// Error Data

// we store the message and level of the Error here, so that we don't have to call the function every frame.
bool criticalError;
Error latestError;
int latestErrorLevel;
const char* latestErrorMessage = "";
bool finishedError = true;

// Chatroom Info

// We store all chatroom behaviour in the chatroom class, to organise and simplify our code.


// Popups
static bool closeChatroomPopup = false;
static bool createChatroomPopup = false;
static bool closeSocketPopup = false;
static bool creditPopup = false;

// GUI Logic

// Error Logic
void ErrorChecker()
{
    // Seeing if a new error exists and assigning it it. Waits for old error to be finished.
    if (ErrorHandler::HasError() && finishedError)
    {
        latestError = ErrorHandler::GetError();
        latestErrorLevel = latestError.GetLevel();
        latestErrorMessage = latestError.GetMessageString().c_str();
        finishedError = false;
    }

    if (ErrorHandler::HasError() && !finishedError)
    {
        if (!latestErrorLevel)
        {
            ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[11]);

            if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[11], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestErrorMessage);
                if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[12], ImVec2(120, 0)))
                {
                    finishedError = true;
                    criticalError = true;
                    ErrorHandler::DeleteError();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else if (latestErrorLevel == 1)
        {
            ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[13]);

            if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[13], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestErrorMessage);
                if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[14], ImVec2(120, 0)))
                {
                    finishedError = true;
                    ErrorHandler::DeleteError();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else if (latestErrorLevel == 2)
        {
            ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[15]);

            if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[15], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestErrorMessage);
                if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[14], ImVec2(120, 0)))
                {
                    finishedError = true;
                    ErrorHandler::DeleteError();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        else if(latestErrorLevel == 3)
        {
	        // Add to console chatroom
            allConsoleTexts.emplace_back(latestErrorMessage);
            finishedError = true;
            ErrorHandler::DeleteError();
        }
        else if (latestErrorLevel == 4)
        {
            // Change status of chatroom
            currentStatus = latestErrorMessage;
            finishedError = true;
            ErrorHandler::DeleteError();

        }


    }
}

// Checks if Ip is valid
bool IpChecker(std::string& _ip)
{
    if (!_ip.size())
    {
        return false;
    }
    int i = 0;
    int dotCount = 0;
    for (char character : _ip)
    {
        if (!isdigit(character) && (character == '.' && (i > 3 || i == 0)))
        {
            return false;
        }
        else if (character == '.')
        {
            i = -1;
            dotCount++;
        }
        else if (!isdigit(character))
        {
            return false;
        }
        i++;
    }
    if (dotCount == 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Checks if Port is valid
bool PortChecker(std::string& _port)
{
    if (!_port.size())
    {
        return false;
    }
    for (char character : _port)
    {
        if (!isdigit(character))
        {
            return false;
        }
    }
    return true;
}

bool CreateBind(std::string& _ip, std::string& _port)
{
    const bool isIpValid = IpChecker(_ip);
    const bool isPortValid = PortChecker(_port);
    if(isIpValid && isPortValid)
    {

        networkCalls->Bind((char*)_ip.c_str(), std::stoi(_port));
        return true;
    }
    else
    {
	    if(!isIpValid)
	    {
            Error invalidIpError(LanguageFileInitialiser::charAllTextsInApplication[16], 1);
            ErrorHandler::AddError(invalidIpError);
	    }
        else if(!isPortValid)
        {
            Error invalidPortError(LanguageFileInitialiser::charAllTextsInApplication[17], 1);
            ErrorHandler::AddError(invalidPortError);
        }
        return false;
    }
}

void PopupChecker()
{
    // Checks for popups
	if (closeChatroomPopup)
	{
        ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[20]);

        if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[20], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[22], ImVec2(120, 0)))
            {
                closeChatroomPopup = false;
                networkCalls->Disconnect();
                networkCalls->CloseChatroom();
                currentStatus = LanguageFileInitialiser::allTextsInApplication[34];
                currentColour = yellow;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[23], ImVec2(120, 0)))
            {
                closeChatroomPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
	}
    else if(createChatroomPopup)
    {
	    
    }
    else if(closeSocketPopup)
    {
        ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[20]);

        if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[20], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[22], ImVec2(120, 0)))
            {
                closeSocketPopup = false;
                if(networkCalls->GetChatroomStatus())
                {
                    networkCalls->Disconnect();
                    networkCalls->CloseChatroom();
                }
                networkCalls->CloseSocket();
                networkCalls->CreateSocket();
                currentStatus = LanguageFileInitialiser::allTextsInApplication[35];
                currentColour = red;
                std::vector<std::string> emptyVector;
                allConsoleTexts = emptyVector;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[23], ImVec2(120, 0)))
            {
                closeSocketPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    else if(creditPopup)
    {
        ImGui::OpenPopup(LanguageFileInitialiser::charAllTextsInApplication[20]);

        if (ImGui::BeginPopupModal(LanguageFileInitialiser::charAllTextsInApplication[20], NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[22], ImVec2(120, 0)))
            {
                creditPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}


// GUI Constructs

void PopulateUsers()
{
    for (int i = 0; i < 31; i++)
    {
        User user = chatroom->GetUser(i);

    }
}

// Menu Bar

void MenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::TextColored(currentColour, currentStatus.c_str());
        if (ImGui::BeginMenu(LanguageFileInitialiser::charAllTextsInApplication[3]))
        {
            if (ImGui::BeginMenu(LanguageFileInitialiser::charAllTextsInApplication[4]))
            {
                ImGui::MenuItem("English", "", &GuiLanguage::english);
                ImGui::MenuItem((const char*)u8"fran�ais", "", &GuiLanguage::french);
                ImGui::MenuItem("Polski", "", &GuiLanguage::polish);
                ImGui::MenuItem("Nederlands", "", &GuiLanguage::dutch);
                ImGui::MenuItem((const char*)u8"espa�ol", "", &GuiLanguage::spanish);
                ImGui::EndMenu();

            }
            if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[5]))
            {

            }

            if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[6]))
            {
                creditPopup = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(LanguageFileInitialiser::charAllTextsInApplication[1]))
        {
            if (networkCalls->GetChatroomStatus())
            {
                if (ImGui::BeginMenu(LanguageFileInitialiser::charAllTextsInApplication[1]))
                {
                    ImGui::SeparatorText("Socket Information");
                    std::string ip = networkCalls->GetCurrentIp();
                    std::string ipText = "Ip: " + ip;
                    ImGui::Text(ipText.c_str());
                    std::string port = std::to_string(networkCalls->GetCurrentPort());
                    std::string portText = "Port: " + port;
                    ImGui::Text(portText.c_str());
                    ImGui::SeparatorText("Chatroom Information");
                    std::string chatroomNameText = "Chatroom name: " + chatroom->GetChatroomName();
                    ImGui::Text(chatroomNameText.c_str());
                    std::string chatroomPasswordText = "Chatroom password: " + chatroom->GetPassword();
                    ImGui::Text(chatroomPasswordText.c_str());
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[18]))
                {
                    closeChatroomPopup = true;
                }
                if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[19]))
                {
                    closeSocketPopup = true;
                }
            }
            else if (networkCalls->GetBindStatus())
            {
                if (ImGui::BeginMenu(LanguageFileInitialiser::charAllTextsInApplication[1]))
                {
                    ImGui::SeparatorText("Socket Information");
                    std::string ip = networkCalls->GetCurrentIp();
                    std::string ipText = "Ip: " + ip;
                    ImGui::Text(ipText.c_str());
                    std::string port = std::to_string(networkCalls->GetCurrentPort());
                    std::string portText = "Port: " + port;
                    ImGui::Text(portText.c_str());
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[19]))
                {
                    closeSocketPopup = true;
                }
            }
            else
            {
                if (ImGui::MenuItem(LanguageFileInitialiser::charAllTextsInApplication[10]))
                {
                    createChatroomPopup = true;
                }
            }
            ImGui::EndMenu();
        }

    }
    ImGui::EndMenuBar();
}


// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Haxel Communication", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);

    ImFont* font = io.Fonts->AddFontFromFileTTF("d:\\Fonts\\arialuni.ttf", 18.0f);
    IM_ASSERT(font != nullptr);
    


    // Our state
    bool exit = true;
    bool resetFocus = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setup Networking
    creator = new WindowsCallsCreator();
    networkCalls = creator->CreateNetworkCalls();
    networkCalls->CreateSocket();
    chatroom = creator->GetChatroom();


    // Loading Configs
    config::StartConfigs();

    // Gui
    std::string currentIpText = "";
    std::string currentPortText = "";
    std::string currentChatroomNameText = "";

    currentColour = red;

    int temp = 0;

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::PushFont(font);
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

        
        
        bool listeningStatus = networkCalls->GetListeningStatus();
        bool chatroomStatus = networkCalls->GetChatroomStatus();
        bool getBindedStatus = networkCalls->GetBindStatus();

        // Networking
        networkCalls->Receive();
        networkCalls->Listen();

        // Gui Logic
        ErrorChecker();
        PopupChecker();

        ImGui::Begin(LanguageFileInitialiser::charAllTextsInApplication[1], &exit, ImGuiWindowFlags_MenuBar);
        {
            
            
            // Check for critical error
            if (criticalError)
            {
                break;
            }
            // Gui
            MenuBar();
            

            // Server Info
            if(!getBindedStatus)
            {
                currentStatus = LanguageFileInitialiser::allTextsInApplication[35];
                ImGui::Text(LanguageFileInitialiser::charAllTextsInApplication[7]);
                static char ip[15];
                static char port[5];
                ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[8], ip, IM_ARRAYSIZE(ip));
                ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[9], port, IM_ARRAYSIZE(port));
                if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[10]))
                {
                    std::string sIp = ip;
                    std::string sPort = port;
                    if(CreateBind(sIp, sPort))
                    {
                        for (char i = 0; i < 15; i++)
                        {
                            ip[i] = NULL;
                        }
                        for (char i = 0; i < 5; i++)
                        {
                            port[i] = NULL;
                        }
                    }
                    currentIpText = LanguageFileInitialiser::allTextsInApplication[8] + ": " + sIp;
                    currentPortText = LanguageFileInitialiser::allTextsInApplication[9] + ": " + sPort;
                    currentStatus = LanguageFileInitialiser::allTextsInApplication[34];
                    currentColour = yellow;
                }
            }
            else if(!chatroomStatus)
            {
                ImGui::SeparatorText("Information");
            	ImGui::Text(currentIpText.c_str());
                ImGui::Text(currentPortText.c_str());
                static char chatroomName[wordSize];
                static char chatroomPassword[wordSize];
                ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[36], chatroomName, IM_ARRAYSIZE(chatroomName));
                ImGui::InputText("Password", chatroomPassword, IM_ARRAYSIZE(chatroomPassword));
            	if(ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[37]))
                {
                    std::string sChatroomName = chatroomName;
                    networkCalls->OpenChatroom(chatroomName, chatroomPassword);
                    currentChatroomNameText = LanguageFileInitialiser::allTextsInApplication[36] + ": " + sChatroomName;
                    currentStatus = LanguageFileInitialiser::allTextsInApplication[34];
                    currentColour = green;
                }

            }
            else
            {
                ImGui::SeparatorText("Information");
                ImGui::Text(currentIpText.c_str());
                ImGui::Text(currentPortText.c_str());
                ImGui::Text(currentChatroomNameText.c_str());
                ImGui::SeparatorText("Console");
                ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, (ImGui::GetWindowHeight() - 175)));
                if (ImGui::BeginChild("ConstrainedChild", ImVec2(0.0f, 0.0f), 0))
                {
                    for (std::string text : allConsoleTexts)
                    {
                        ImGui::Text(text.c_str());
                    }

                }
                ImGui::EndChild();
                static char command[128] = "";
                if (resetFocus)
                {
                    ImGui::SetKeyboardFocusHere(1);
                    resetFocus = false;
                }
                if(ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[41], command, IM_ARRAYSIZE(command), ImGuiInputTextFlags_CallbackAlways))
                {
                    resetFocus = true;

                }
            }
            
            

            ImGui::End();

        }




        
        ImGui::Begin(LanguageFileInitialiser::charAllTextsInApplication[2], &exit, ImGuiWindowFlags_MenuBar);
        {
            if (ImGui::BeginTable(LanguageFileInitialiser::charAllTextsInApplication[2], 3))
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 2; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    if (!column)
                    {
                        ImGui::Text("User");

                        continue;
                    }
                    ImGui::Text("Id");

                }

            	ImGui::EndTable();

            }

            
            if (ImGui::BeginTable(LanguageFileInitialiser::charAllTextsInApplication[2], 3) && chatroomStatus)
            {

                for (int row = 0; row < 32; row++)
                {
                    User user = chatroom->GetUser(row);
                    ImGui::TableNextRow();
                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        if (!column)
                        {
                            
                            ImGui::Text(user.GetDisplayName());
                            continue;
                        }
                        
                        ImGui::Text(user.GetId());
                    }
                }
                
            }
            ImGui::EndTable();
        	ImGui::End();
        }

        ImGui::PopFont();
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    networkCalls->CloseChatroom();
	networkCalls->Disconnect();
    networkCalls->CloseSocket();
    delete networkCalls;
    delete creator;

    return 0;
}
