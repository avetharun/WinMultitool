#define SDL_MAIN_HANDLED

#include <iostream>

#include <alib.hpp>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL_opengl.h>

#include <imgui_uielement.h>
#include <imgui_texture.h>
#include <imgui_format.h>
#include <WinUser.h>

#include <cwerror.h>

#define NIM_ADD         0x00000000
#define NIM_MODIFY      0x00000001
#define NIM_DELETE      0x00000002
#define NIM_SETFOCUS    0x00000003
#define NIM_SETVERSION  0x00000004


#define NIF_MESSAGE     0x00000001
#define NIF_ICON        0x00000002
#define NIF_TIP         0x00000004
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010


#define NIS_HIDDEN              0x00000001
#define NIS_SHAREDICON          0x00000002



#define NIN_SELECT          (WM_USER + 0)
#define NINF_KEY            0x1
#define NIN_KEYSELECT       (NIN_SELECT | NINF_KEY)

#define NIN_BALLOONSHOW         (WM_USER + 2)
#define NIN_BALLOONHIDE         (WM_USER + 3)
#define NIN_BALLOONTIMEOUT      (WM_USER + 4)
#define NIN_BALLOONUSERCLICK    (WM_USER + 5)
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define NIN_POPUPOPEN           (WM_USER + 6)
#define NIN_POPUPCLOSE          (WM_USER + 7)
#endif // (NTDDI_VERSION >= NTDDI_VISTA)
#if (NTDDI_VERSION >= NTDDI_WIN7)
#endif // (NTDDI_VERSION >= NTDDI_WIN7)


#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)


#if (NTDDI_VERSION >= NTDDI_VISTA)
#define NIF_REALTIME    0x00000040
#define NIF_SHOWTIP     0x00000080
#endif // (NTDDI_VERSION >= NTDDI_VISTA)


#include <windows.h>
#include <shellapi.h>
#include <conio.h>


#define IDM_CONTEXT_EXIT 1001
#define IDM_CONTEXT_HELP 1002

#define WMAPP_NOTIFYCALLBACK (WM_APP + 1) // arbitrary value between WP_APP and 0xBFFF
bool m_im_Initialized;
bool m_ShowDefaultWindow = true;
bool m_ShowHelpWindow = false;
bool m_ShowTrayWindow = false;

// Main loop
bool m_beginExit = false;

namespace GlobalState {
    ImGui::ImageTexture AppIcon;
};

wchar_t const szTrayWindowClass[] = L"m_WinMultitool_SystemTray";
WNDPROC g_prevWndProc;


HINSTANCE g_hInst = NULL;

// show a help window popup
void m_showHelpWindow();
// show the main window (has settings)
void m_showMainWindow();

static inline HMENU hMenuPopup;
void RegisterWindowClass(PCWSTR pszClassName, PCWSTR pszMenuName, WNDPROC lpfnWndProc)
{
    WNDCLASSEX wcex = { sizeof(wcex) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = lpfnWndProc;
    wcex.hInstance = g_hInst;
    wcex.hIcon = LoadIcon(g_hInst, L"assets/icon.ico");
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = pszMenuName;
    wcex.lpszClassName = pszClassName;
    RegisterClassEx(&wcex);
}
void showTrayContextMenu(bool state, HWND hWnd) {

    DestroyMenu(hMenuPopup);
    POINT curPoint;
    GetCursorPos(&curPoint);
    if (!state) { return; }
    hMenuPopup = CreatePopupMenu();
    AppendMenu(hMenuPopup, MF_STRING, IDM_CONTEXT_HELP, L"Help");
    AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenuPopup, MF_STRING, IDM_CONTEXT_EXIT, L"Exit");
    TrackPopupMenu(hMenuPopup,
        TPM_LEFTALIGN | TPM_RIGHTBUTTON,
        curPoint.x, curPoint.y, 0, hWnd, NULL);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
    if (umsg == WMAPP_NOTIFYCALLBACK) {
        switch (lParam) {
            case WM_RBUTTONUP: {
                showTrayContextMenu(true, hWnd);
                break;
            }
            case WM_LBUTTONDBLCLK: {
                m_ShowDefaultWindow = !m_ShowDefaultWindow;
                break;
            }
        }
    }
    else if (umsg == WM_COMMAND) { // In context menu

        switch (LOWORD(wParam))
        {
            case IDM_CONTEXT_EXIT: {
                m_beginExit = true;
                break;
            }
            case IDM_CONTEXT_HELP: {
                m_ShowHelpWindow = !m_ShowHelpWindow;
            }

        }
    }
    else {
        showTrayContextMenu(false, hWnd);
    }
    return DefWindowProcW(hWnd, umsg, wParam, lParam);

}


struct DebugConsole {
    static inline ImGuiTextBuffer debugWindowConsoleText = {};
    // Deletes buffer in sizes of 512 if the size of the buffer exceeds 4012
    static void pushuf(std::string cstr) {
        if (&debugWindowConsoleText == nullptr) { return; }
        debugWindowConsoleText.append(cstr.c_str());
    }
    static void pushf(std::string fmt, ...) {
        if (&debugWindowConsoleText == nullptr) { return; }
        va_list args;
        va_start(args, fmt);
        debugWindowConsoleText.appendfv(fmt.c_str(), args);
        va_end(args);
    }
    static void cwErrorHandler(const char* errs, uint32_t errid) {
        // errid not used
        pushf("%s\n%s", errs, ImRGB::resetstr());
    }

};


int main() { 
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Windows Multitool", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
    HWND hWnd = GetActiveWindow();
    g_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
    HWND chWnd = GetConsoleWindow();
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    long m_win_style = GetWindowLong(hWnd, GWL_STYLE);
    m_win_style &= ~(WS_VISIBLE);    // this works - window become invisible 
    
    m_win_style|= WS_EX_TOOLWINDOW;   // flags don't work - windows remains in taskbar
    m_win_style&= ~(WS_EX_APPWINDOW);
    
    long m_con_style = GetWindowLong(chWnd, GWL_STYLE);
    m_con_style &= ~(WS_VISIBLE);    // this works - window become invisible 
    
    m_con_style |= WS_EX_TOOLWINDOW;   // flags don't work - windows remains in taskbar
    m_con_style &= ~(WS_EX_APPWINDOW);
    ShowWindow(hWnd, SW_HIDE); // hide the window
    SetWindowLong(hWnd, GWL_STYLE, m_win_style); // set the style
    ShowWindow(hWnd, SW_SHOW); // show the window for the new style to come into effect
    ShowWindow(hWnd, SW_HIDE); // hide the window so we can't see it

    ShowWindow(chWnd, SW_HIDE); // hide the window
    SetWindowLong(chWnd, GWL_STYLE, m_con_style); // set the style
    ShowWindow(chWnd, SW_SHOW); // show the window for the new style to come into effect
    ShowWindow(chWnd, SW_HIDE); // hide the window so we can't see it

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    cwError::onError = DebugConsole::cwErrorHandler;

    const wchar_t CLASS_NAME[] = L"WinMultitool Status Tray Manager";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND tray_hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"WinMultitool tray icon",      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        hWnd,       // Parent window    
        NULL,       // Menu
        g_hInst,  // Instance handle
        NULL        // Additional application data
    );
    NOTIFYICONDATAA nid;
    nid.cbSize = sizeof(nid);
    nid.uID = (UINT)std::hash<const char*>()("winmultitooltray");
    nid.hIcon = (HICON)LoadImage(NULL, L"assets/icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_SHARED);
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.hWnd = tray_hwnd;
    strcpy(nid.szTip, "Windows Multitool Tray Icon");

    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

    Shell_NotifyIconA(NIM_ADD, &nid);
    m_im_Initialized = true;

    ImGui::GetStyle().WindowMinSize = { 512, 256 };


    while (!m_beginExit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            // This should never be called, but just in case, we'll call it anyways.
            if (event.type == SDL_QUIT)
                m_beginExit = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                m_beginExit = true;
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        if (m_ShowDefaultWindow) {
            m_showMainWindow();
        }
        if (m_ShowHelpWindow) {
            m_showHelpWindow();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    Shell_NotifyIconA(NIM_DELETE, &nid);
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void m_showHelpWindow() {

    if (ImGui::Begin("Windows Multitool Help Window", &m_ShowHelpWindow, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
    }
}
void m_showMainWindow() {
    if (ImGui::Begin("Windows Multitool", &m_ShowDefaultWindow, ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextMulticolored(DebugConsole::debugWindowConsoleText.c_str());
        ImGui::End();
    }
}