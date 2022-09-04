#define SDL_MAIN_HANDLED
#define IM_VEC2_CLASS_EXTRA
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
#include "globals/globals.h"

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
#include <dwmapi.h>

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
        debugWindowConsoleText.append("\n");
    }
    static void pushf(std::string fmt, ...) {
        if (&debugWindowConsoleText == nullptr) { return; }
        va_list args;
        va_start(args, fmt);
        debugWindowConsoleText.appendfv(fmt.c_str(), args);
        va_end(args);
        debugWindowConsoleText.append("\n");
    }
    static void cwErrorHandler(const char* errs, uint32_t errid) {
        // errid not used
        pushf("%s\n%s", errs, ImRGB::resetstr());
    }

};
void MinimizeWindowEx(HWND hWnd) {
    ShowWindow(hWnd, SW_MINIMIZE);
}
void MaximizeWindowEx(HWND hWnd) {
    ShowWindow(hWnd, SW_MAXIMIZE);
}
void FullHideWindowEx(HWND hWnd) {

    long m_win_style = GetWindowLong(hWnd, GWL_STYLE);
    m_win_style &= ~(WS_VISIBLE);    // this works - window become invisible 

    m_win_style |= WS_EX_TOOLWINDOW;   // flags don't work - windows remains in taskbar
    m_win_style &= ~(WS_EX_APPWINDOW);
    ShowWindow(hWnd, SW_HIDE); // hide the window
    SetWindowLong(hWnd, GWL_STYLE, m_win_style); // set the style
    ShowWindow(hWnd, SW_SHOW); // show the window for the new style to come into effect
    ShowWindow(hWnd, SW_HIDE); // hide the window so we can't see it
}
ImVec2 getFullScreenSize() {
    return {
        (float)GetSystemMetrics(SM_CXFULLSCREEN),
        (float)GetSystemMetrics(SM_CYFULLSCREEN)
    };
}
ImVec2 getMaximizedWindowSize() {
    return {
        (float)GetSystemMetrics(SM_CXMAXIMIZED) - 12,
        (float)GetSystemMetrics(SM_CYMAXIMIZED) 
    };
}
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
    SDL_Window* window = SDL_CreateWindow("Windows Multitool Proxy Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
    HWND hWnd = GetActiveWindow();
    g_hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
    HWND chWnd = GetConsoleWindow();
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);


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
    io.IniFilename = 0;

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
    auto _ranges = new ImWchar[2]{ 0x0020, 0xFFEF };
    M_Globals::default_font = io.Fonts->AddFontDefault();
    M_Globals::symbol_font = io.Fonts->AddFontFromFileTTF("assets/symbols.ttf",18, 0, _ranges);
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

    int __windowcheck_id = 0;
    while (!m_beginExit)
    {
        __windowcheck_id++;
        if (__windowcheck_id == 10) {
            FullHideWindowEx(hWnd);
        }
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
#if (!NDEBUG)
        // close program when main window is closed if debug build
        else {
            break;
        }
#endif
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
ImWindowData m_main_d, m_help_d;
void m_showHelpWindow() {

    if (ImGui::Begin("Windows Multitool Help Window", &m_ShowHelpWindow, ImGuiWindowFlags_NoCollapse)) {
        ImGui::GetCurrentWindow()->UserData = &m_help_d;
        ImGui::End();
    }
}


float menu_pane_percent = 100.0f;
void m_showMainWindow() {
    ImGuiIO& io = ImGui::GetIO();
    if (m_main_d.need_resize) {
        if (m_main_d.maximized) {
            ImGui::SetNextWindowPos({ 0,0 });
            ImGui::SetNextWindowSize(getMaximizedWindowSize());
        }
        else {
            ImGui::SetNextWindowPos(m_main_d.m_FloatingMin);
            ImGui::SetNextWindowSize(m_main_d.m_FloatingSize);
            DebugConsole::pushuf("Setting restored down size..");
        }
        m_main_d.need_resize = false;
    }
    
    if (ImGui::Begin("\tWindows Multitool", &m_ShowDefaultWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar 
        | ( (m_main_d.maximized) ? (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : 0)
    )) {
        ImGui::GetCurrentWindow()->UserData = &m_main_d;
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::TextMulticolored(DebugConsole::debugWindowConsoleText.c_str());
        ImGuiStyle s_tmp = ImGui::GetStyle();
        ImGuiStyle& s = ImGui::GetStyle();
        ImVec2 menu_pane_left = ImGui::GetWindowPos();
        ImVec2 menu_pane_right = { alib_clamp(alib_percentf(ImGui::GetWindowWidth() * 0.4f, menu_pane_percent), 256.0f, 512.0f), ImGui::GetWindowHeight()};
        ImGui::BeginMenuBar();
        s.WindowPadding = { 0,0 };
        s.Colors[ImGuiCol_Button] = { 0,0,0,0 };
        ImVec2 pos_tmp = ImGui::GetCursorPos();

        ImGui::PushFont(M_Globals::symbol_font);
        ImVec2 pos = ImGui::CalcTextAlignedRight("Q P #\t ");
        ImGui::SetCursorScreenPos(pos);
        bool dock_window = ImGui::SmallButton("Q##__dock_window");
        ImGui::PushID("__maximize_window");
            bool maximize = ImGui::SmallButton((m_main_d.maximized)?"P" : ";"); ImGui::PopID();
        ImGui::PushID("__close_window");
            m_ShowDefaultWindow = !ImGui::SmallButton("#"); ImGui::PopID();
        ImGui::SetCursorPos({pos_tmp.x - 8, pos_tmp.y});
        bool menu_drawer = ImGui::Button(u8"¡");
        ImGui::PopFont();
        ImGui::TextMulticolored("\\db7991]Windows Multitool\\]");

        ImGui::EndMenuBar();
        if (dock_window) {
            MinimizeWindowEx(GetActiveWindow());
        }
        if (maximize) {
            if (!m_main_d.maximized) {
                // store the size of the window
                m_main_d.m_FloatingMin = ImGui::GetWindowPos();
                m_main_d.m_FloatingSize = ImGui::GetWindowSize();
            }
            m_main_d.need_resize = true;
            m_main_d.maximized = !m_main_d.maximized;
        }
        ImGui::End();
    }
}