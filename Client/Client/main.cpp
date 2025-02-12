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
#include <thread>

#include "Chatroom.hpp"
#include "config.hpp"
#include "DataManager.hpp"
#include "User.hpp"
#include "ErrorHandler.hpp"
#include "GuiLanguage.hpp"
#include "Languages.hpp"

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

// GUI Logic

static bool wantsEditUser = false;
static bool* userUpdatedStatus = nullptr;


static ImVec4 white{ 1,1,1,1 };
static ImVec4 red{ 1.0f, 0.0f, 0.0f ,1.0f };
static ImVec4 yellow{ 1.0f, 1.0f, 0.0f , 1.0f };
static ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f };

// Popups
bool leaveChatroomPopup = false;

// Client

// Application

NetworkCallsCreator* creator;
NetworkCalls* networkCalls;

bool* receivingStatus;
bool* chatroomStatus;
bool* connectedStatus;


// Error Data

// we store the message and level of the Error here, so that we don't have to call the function every frame.
bool criticalError;
Error latestError;
int latestErrorLevel;
const char* latestErrorMessage = "";
bool finishedError = true;

// Chatroom Info
static User* clientUser = new User((char*)"Deguy", (char*)"77", white);

static inline Chatroom emptyChatroom;
static inline Chatroom* chatroom;
static inline MessageBuilder messageBuilder;



// We store all chatroom behaviour in the chatroom class, to organise and simplify our code.
static bool updateMessagesInChatroom;




// Error Logic
void ErrorChecker()
{
    // Seeing if a new error exists and assigning it it. Waits for old error to be finished.
    if (ErrorHandler::HasError() && finishedError)
    {
        latestError = ErrorHandler::GetError();
        latestErrorLevel = latestError.GetLevel();
        finishedError = false;
    }

    if (ErrorHandler::HasError() && !finishedError)
    {
        if (!latestErrorLevel)
        {
            ImGui::OpenPopup("Critical");
            
            if (ImGui::BeginPopupModal("Critical", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestError.GetMessageString().c_str());
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    criticalError = true;
                    ErrorHandler::DeleteError();
                    finishedError = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();

            }
        }
        else if (latestErrorLevel == 1)
        {
            ImGui::OpenPopup("Warning");
            
            if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestError.GetMessageString().c_str());
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ErrorHandler::DeleteError();
                    finishedError = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            
        }
        else if (latestErrorLevel == 2)
        {
            ImGui::OpenPopup("Information");
            
            if (ImGui::BeginPopupModal("Information", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestError.GetMessageString().c_str());
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ErrorHandler::DeleteError();
                    finishedError = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();

            }
        }
        else if (latestErrorLevel == 3)
        {
            ImGui::OpenPopup("Response Required");
            ErrorHandler::SetCurrentResponseError(latestError);
            if (ImGui::BeginPopupModal("Response Required", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text(latestError.GetMessageString().c_str());
                if (ImGui::Button("Yes", ImVec2(120, 0)))
                {
                    ErrorHandler::SetResponseBool(true);
                    ErrorHandler::SetHasResponded(true);
                    ErrorHandler::DeleteError();


                    finishedError = true;
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("No", ImVec2(120, 0)))
                {
                    ErrorHandler::DeleteError();
                    finishedError = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();

            }
        }


    }
}

// Checks if Ip is valid
bool IpChecker(std::string& _ip)
{
    if(!_ip.size())
    {
        return false;
    }
    int i = 0;
    int dotCount = 0;
    for (char character : _ip)
    {
        if(!isdigit(character) && (character == '.' && (i > 3 || i == 0)))
        {
            return false;
        }
        else if(character == '.')
        {
            i = -1;
            dotCount++;
        }
        else if(!isdigit(character))
        {
            return false;
        }
        i++;
    }
    if(dotCount == 3)
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

void JoinChatroom(std::string& _ip, std::string& _port, std::string& _password)
{
    const bool isIpValid = IpChecker(_ip);
    const bool isPortValid = PortChecker(_port);
    if(isIpValid && isPortValid)
    {
        networkCalls->Disconnect();
        networkCalls->CloseSocket();
        networkCalls->CreateSocket();
        networkCalls->Connect((char*)_ip.c_str(), std::stoi(_port), (char*)_password.c_str());
    }
    else
    {
	    if(!isIpValid)
	    {
            Error invalidIpError("Invalid Ip", 1);
            ErrorHandler::AddError(invalidIpError);
	    }
        else if(!isPortValid)
        {
            Error invalidPortError("Invalid Port", 1);
            ErrorHandler::AddError(invalidPortError);
        }
    }
}




// GUI Constructs

void JoinChatroomGui()
{
    ImGui::Text("Chatroom");
    static char guiIp[14] = "";
    static char guiPort[5] = "";
    static char guiPassword[32] = "";
    ImGui::InputText("IP", guiIp, IM_ARRAYSIZE(guiIp));
    ImGui::InputText("Port", guiPort, IM_ARRAYSIZE(guiPort));
    ImGui::InputText("Password", guiPassword, IM_ARRAYSIZE(guiPassword));
    if (ImGui::Button("Connect", ImVec2(120, 0)))
    {
        std::string ip = guiIp;
        std::string port = guiPort;
        std::string password = guiPassword;
        JoinChatroom(ip, port, password);
    }
}

void PopupChecker()
{
	if(leaveChatroomPopup)
	{
        ImGui::OpenPopup("Leave Chatroom");

        if (ImGui::BeginPopupModal("Leave Chatroom", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you would like to leave the chatroom?");
            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                leaveChatroomPopup = false;
                networkCalls->Disconnect();
                networkCalls->CloseSocket();
                networkCalls->CreateSocket();
                *chatroom = emptyChatroom;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("No", ImVec2(120, 0)))
            {
                leaveChatroomPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();

        }
	}
}


void PopulateUsers()
{
    for (int i = 0; i < 31; i++)
    {
        User user = chatroom->GetUser(i);

    }
}

// Menu Bar

std::string origialMessage = "Hello!";
std::vector<int> style;
static bool wantsDisplayName;
static bool wantsId;
static bool wantsDateShort;
static bool wantsDateLong;
static bool wantsTime;

void MenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::BeginMenu("Language"))
            {
                ImGui::MenuItem("English", "", &GuiLanguage::english);
                ImGui::MenuItem((const char*)u8"fran�ais", "", &GuiLanguage::french);
                ImGui::MenuItem("Polski", "", &GuiLanguage::polish);
                ImGui::MenuItem("Nederlands", "", &GuiLanguage::dutch);
                ImGui::MenuItem((const char*)u8"espa�ol", "", &GuiLanguage::spanish);
                ImGui::EndMenu();

            }
            if (ImGui::BeginMenu("Apperance"))
            {
                if (ImGui::BeginMenu("Message"))
                {
                    
                    Message exampleMessage(origialMessage, std::stoi(clientUser->GetId()));
                    messageBuilder.Reset();
                    messageBuilder.SetStyle(style);
                    messageBuilder.AddMessage(exampleMessage.GetUserPosition(), origialMessage, *clientUser);
                    messageBuilder.BuildFromStyle();
                    Message* finalMessage = messageBuilder.GetFinalMessage();
                    ImGui::Text(finalMessage->GetMessageComplete().c_str());
                    ImGui::SameLine();
                    if(ImGui::Checkbox("Display Name", &wantsDisplayName))
                    {
	                    if(wantsDisplayName)
	                    {
                            style.emplace_back(1);

	                    }
                        else
                        {
                            
	                        for (size_t i = 0, size = style.size(); i < size; i++)
	                        {
                                if (style[i] == 1)
                                {
                                    style.erase(style.begin()+i);
                                    break;
                                }
	                        }

                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Checkbox("Id", &wantsId))
                    {
                        if (wantsId)
                        {
                            style.emplace_back(2);
                        }
                        else
                        {
                            for (size_t i = 0, size = style.size(); i < size; i++)
                            {
                                if (style[i] == 2)
                                {
                                    style.erase(style.begin() + i);
                                    break;
                                }
                            }
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Checkbox("Short Date", &wantsDateShort))
                    {
                        if (wantsDateShort)
                        {
                            style.emplace_back(3);
                        }
                        else
                        {
                            for (size_t i = 0, size = style.size(); i < size; i++)
                            {
                                if (style[i] == 3)
                                {
                                    style.erase(style.begin() + i);
                                    break;
                                }
                            }
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Checkbox("Long Date", &wantsDateLong))
                    {
                        if (wantsDateLong)
                        {
                            style.emplace_back(4);
                        }
                        else
                        {
                            for (size_t i = 0, size = style.size(); i < size; i++)
                            {
                                if (style[i] == 4)
                                {
                                    style.erase(style.begin() + i);
                                    break;
                                }
                            }
                        }
                    }
                    ImGui::SameLine();
                    if(ImGui::Checkbox("Time", &wantsTime))
                    {
                        if (wantsTime)
                        {
                            style.emplace_back(5);
                        }
                        else
                        {
                            for (size_t i = 0, size = style.size(); i < size; i++)
                            {
                                if (style[i] == 5)
                                {
                                    style.erase(style.begin() + i);
                                    break;
                                }
                            }
                        }
                    }

                    if(ImGui::Button("SaveChanges"))
                    {
                        chatroom->SetStyle(style);
                        chatroom->UpdateMessages();
                        updateMessagesInChatroom = true;
                        
                    }
                    ImGui::EndMenu();

                }
                if (ImGui::MenuItem("Application"))
                {

                }

                ImGui::EndMenu();

            }
            

            if (ImGui::MenuItem("Credit"))
            {
                //ImGui::Text();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Chatroom"))
        {
            if (networkCalls->GetChatroomStatus())
            {
                if (ImGui::MenuItem("Leave Chatroom"))
                {
                    leaveChatroomPopup = true;
                }

            }
            else
            {
                if (ImGui::BeginMenu("Join Chatroom"))
                {
                    JoinChatroomGui();
                    ImGui::EndMenu();

                }

            }
            ImGui::EndMenu();

        }

        if (ImGui::BeginMenu("User"))
        {
            if (ImGui::MenuItem("Save User"))
            {
                DataManager::SaveUser(*clientUser, style);
            }
            if (ImGui::MenuItem("Load User"))
            {

            }

            if(wantsEditUser)
            {
                
                if(ImGui::MenuItem("Close Edit User Panel"))
                {
                    wantsEditUser = false;
                }
            }
            else
            {
                if (ImGui::MenuItem("Edit User"))
                {
                    wantsEditUser = true;
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
    
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    io.Fonts->AddFontDefault();
    ImFont* font = io.Fonts->AddFontFromFileTTF("d:\\Fonts\\arialuni.ttf", 18.0f);
    IM_ASSERT(font != nullptr);

    


    // Our state
    bool exit = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Setup Networking
    creator = new WindowsCallsCreator();
    networkCalls = creator->CreateNetworkCalls();
    networkCalls->CreateSocket();
    chatroom = &networkCalls->GetChatroom();

    // Indication of current processes
    receivingStatus = networkCalls->GetReceivingStatus();
    chatroomStatus = networkCalls->GetChatroomStatus();
    connectedStatus = networkCalls->GetConnectedStatus();
    userUpdatedStatus = networkCalls->GetUpdatedUserStatus();


    // All scripts point to the same client user. Which is declared here
    chatroom->SetClientUser(clientUser);
    networkCalls->UpdateUser(clientUser);

    bool resetFocus = false;

    // Loading Configs
    config::StartConfigs();

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

        
        
        

        // Receive data from chatroom
        networkCalls->Receive();

        // Check for critical error
        if (criticalError)
        {
            break;
        }
        // Gui Logic
        ErrorChecker();
        PopupChecker();

        if(*chatroomStatus)
        {
            ImGui::Begin("ChatRoom", &exit, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
            {
                if(updateMessagesInChatroom)
                {
                    chatroom->UpdateMessages();
                    updateMessagesInChatroom = false;
                }

                // Gui
                MenuBar();
                ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, (ImGui::GetWindowHeight() - 92)));
                if (ImGui::BeginChild("ConstrainedChild", ImVec2(0.0f, 0.0f), 0))
                {
                    std::vector<Message> messages = chatroom->GetMessages();
                    for(Message message : messages)
                    {
                        
                        ImVec4 userColour = chatroom->GetUser(message.GetUserPosition()).GetUserColour();
                        ImGui::TextColored(userColour,message.GetMessageComplete().c_str());
                    }
                    
                }
                ImGui::EndChild();
                static char textInput[messageSize] = "";
                if(resetFocus)
                {
                    ImGui::SetKeyboardFocusHere(1);
                    resetFocus = false;
                }
                if(ImGui::InputText("Message", textInput, IM_ARRAYSIZE(textInput), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                	resetFocus = true;
                    if(textInput != "")
                    {
                        networkCalls->SendText(textInput);
                        for (int i = 0; i < messageSize; i++)
                        {
                            textInput[i] = 0;
                        }
                    }
                    
                }
            }
            

            
            
            ImGui::End();
        }
        else
        {
            ImGui::Begin("Join Chatroom", &exit, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            {
                // Check for critical error
                if (criticalError)
                {
                    break;
                }
                // Gui
                MenuBar();
                ImGui::Text("Chatroom info:");
                static char ip[14] = "";
                ImGui::InputText("Ip", ip, IM_ARRAYSIZE(ip));
                static char port[5] = "";
                ImGui::InputText("Port", port, IM_ARRAYSIZE(port));
                static char password[32] = "";
                ImGui::InputText("Password", password, IM_ARRAYSIZE(password));
                if(ImGui::Button("Connect"))
                {
                    std::string sIp = ip;
                    std::string sPort = port;
                    std::string sPassword = password;
                    JoinChatroom(sIp, sPort, sPassword);
                }
            }




            ImGui::End();

        }

        ImGui::Begin("Users", &exit, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
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

            if (ImGui::BeginTable("Users", 3) && chatroomStatus)
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
        }

        ImGui::End();

        if(wantsEditUser)
        {
            ImGui::Begin("Edit User", &wantsEditUser, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);
            {
                std::string currentUserName;
                ImGui::SeparatorText("User Information");
                if(userUpdatedStatus)
                {
                    ImGui::TextColored(green, "Updated with chatroom.");
                }
                else if(!chatroomStatus && !connectedStatus)
                {
                    ImGui::TextColored(white, "Currently not in chatroom.");
                }
                else
                {
                    ImGui::TextColored(red, "Not updated with chatroom.");

                }
                std::string userNameText = "User name: " + (std::string)clientUser->GetDisplayName();
                ImGui::Text(userNameText.c_str());
                static char userName[14] = "";
                ImGui::InputText("Name", userName, IM_ARRAYSIZE(userName));

                ImGui::SeparatorText("User Aesthetics");
                static ImVec4 userColour;
                static bool ref_color = false;
                static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 1.0f);
                ImGui::SetNextItemWidth(200.0f);
                ImGui::ColorPicker4("Text Colour", (float*)&userColour, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_DisplayRGB, ref_color ? &ref_color_v.x : NULL);

                if(ImGui::Button("Save Changes"))
                {
                    std::string name = userName;
                    
                    clientUser->SetDisplayName((char*)name.c_str());
                    userColour.w = 1.0f;
                    clientUser->SetUserColour(userColour);
                    networkCalls->UpdateUser(clientUser);
                    chatroom->SetClientUser(clientUser);

                    
                }
            }
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

    creator->Disconnect();
    delete creator;
    delete networkCalls;
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
