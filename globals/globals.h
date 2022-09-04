#pragma once
#ifndef alib__winmt_globals_h
#define alib__winmt_globals_h
#include <alib.hpp>
#include <imgui.h>
#include <imgui_texture.h>
#include <imgui_format.h>
namespace M_Globals {
	ImFont* symbol_font;
	ImFont* default_font;
};

enum WindowStateEnum : int32_t {
	WindowState_Minimized		= B32(00000000, 00000000, 00000000, 00000001),
	WindowState_Maximized		= B32(00000000, 00000000, 00000000, 00000010),
	WindowState_Docked			= B32(00000000, 00000000, 00000000, 00000100),
	WindowState_Fullscreen		= B32(00000000, 00000000, 00000000, 00001000),
	WindowState_NoTitlebar		= B32(00000000, 00000000, 00000000, 00010000),
	WindowState_UseTitleIcon	= B32(00000000, 00000000, 00000000, 00100000),
	WindowState_UseTrayIcon		= B32(00000000, 00000000, 00000000, 01000000),
};

typedef int32_t WindowState;
struct ImWindowData {
	WindowState m_State;
	ImVec2 MIN, MAX;
	bool minimized, maximized, exiting, need_resize;
	ImVec2 m_FloatingMin, m_FloatingSize;
};
#endif