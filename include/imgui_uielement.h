#pragma once
#ifndef __rosemary_project_imgui_uielement_h_
#define __rosemary_project_imgui_uielement_h_

#pragma warning( push )

#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )



#include "imgui.h"
#include "imgui_format.h"
#include "imgui_texture.h"
#include "imgui_internal.h"

#include <string>
#include <vector>
#include <array>
#include "cwerror.h"
#ifndef PI
#define PI 3.14159265358979323846
#endif
#include <SDL2/SDL_stdinc.h>


enum {
	ImGuiMenuButton_Hamburger,
	ImGuiMenuButton_Doner,
	ImGuiMenuButton_Bento,
	ImGuiMenuButton_Kebab,
	ImGuiMenuButton_Meatball,
	ImGuiMenuButton_Cookie
};
namespace ImGui {
	ImVector<ImVec2> m_uielement___lastwindowpos_stack;
	int m_uielement___RawAmt;
	static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y);
	}
	// Returns true if the rect has an area of 0
	static inline bool IsEmptyRect(ImRect r) {
		if (r.GetWidth() == 0 || r.GetHeight() == 0) { return true; }
		return false;
	}
	// Returns true if the rect has an area of 0
	static inline bool IsEmptyRect(ImVec2 r) {
		if (r.x == 0 || r.y == 0) { return true; }
		return false;
	}
	static inline ImVec2 ElementLocalPositionFromStatic(ImVec2 in) {
		ImVec2 out = {
			(in.x + ImGui::GetWindowPos().x) - ImGui::GetCurrentWindow()->Scroll.x,
			(in.x + ImGui::GetWindowPos().x) - ImGui::GetCurrentWindow()->Scroll.y
		};
		return out;
	}
	ImVector<float> __imFontSizeStack;
	void PushFontScale(float scale) {
		__imFontSizeStack.push_back(scale);
		ImGui::SetWindowFontScale(__imFontSizeStack.front());
	}
	void PopFontScale() {
		if (__imFontSizeStack.size() == 0) { return; }
		__imFontSizeStack.pop_back();
		// ensure we keep default font size if stack is empty after popping.
		if (__imFontSizeStack.size() == 0) { ImGui::SetWindowFontScale(1.0f); return; }
		ImGui::SetWindowFontScale(__imFontSizeStack.front());
	}
	static inline ImRect ScaleRect(ImRect m__in, float amount) {
		ImVec4 in = m__in.ToVec4();
		double mx = in.x + 0.5 * in.z;
		double my = in.y + 0.5 * in.w;
		in.z = amount * in.z;
		in.w = amount * in.w;
		// Mid point remains exactly at the same location.
		// Move the lower left corner so that it is offset from
		// the mid point by half the length and half the width.
		in.x = (mx - 0.5 * in.z);
		in.y = (my - 0.5 * in.w);
		return in;
	}
	struct UIElementData {
		ImRGB BackgroundColor = { 255,255,255,255 };
		float BackgroundBorderRadius = 0;
		float ElementBorderRadius = 0;
		ImRGB ForegroundColor = { 255,0,255,255 };
		ImRGB TextColor = { 0,0,0,255 };
		ImRGB HoverColor = { 245,245,245,255 };
		ImRGB ClickedColor = { 220,220,220,255 };
	};
	void ScrollEmptySpace()
	{
		ImVec2 delta = ImGui::GetIO().MouseDelta;


		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiWindow* window = g.CurrentWindow;
		if (!window) { return; }
		if ((ImGui::GetIO().MouseDownDuration[0] >= .04f)) {
			window->Scroll.x -= (delta.x);
			window->Scroll.y -= (delta.y);
		}
	}
	enum ElementsEnum : long long {
		ImGui_AllElements = B32(10000000, 00000000, 00000000, 00000000),
		ImGui_TextElement = B32(00000000, 00000000, 00000000, 00000010),
		ImGui_ButtonElement = B32(00000000, 00000000, 00000000, 00000100),
		ImGui_ImageElement = B32(00000000, 00000000, 00000000, 00001000),
		ImGui_CheckboxElement = B32(00000000, 00000000, 00000000, 00010000),
		ImGui_TextInputElement = B32(00000000, 00000000, 00000000, 00100000),
	};
	// Creates a fullscreen window via ImGui, that cannot be moved, resized, and has no background.
	void BeginFullscreen() {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ io.DisplaySize.x, io.DisplaySize.y });
		ImGuiWindowFlags __flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;
		ImGuiStyle _s_first = ImGui::GetStyle();
		ImGui::GetStyle().FramePadding = { 0,0 };
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().DisplayWindowPadding = { 0,0 };
		ImGui::Begin(alib_strfmt("##__RawBeginRoot_%d", m_uielement___RawAmt++), NULL, __flags);
		ImGui::GetStyle() = _s_first;
	}
	void EndFullscreen() {
		ScrollEmptySpace();
		m_uielement___lastwindowpos_stack.pop_back();
		ImGui::SetCursorScreenPos(m_uielement___lastwindowpos_stack.back());
		ImGui::End();
	}
	// Creates a fullscreen window via ImGui, that cannot be moved, resized, has no background, and does not take inputs.
	void BeginEmptyStatic() {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ io.DisplaySize.x, io.DisplaySize.y });
		ImGui::SetKeyboardFocusHere(-1);
		ImGuiWindowFlags __flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing;
		m_uielement___lastwindowpos_stack.push_back(ImGui::GetCursorScreenPos());
		ImGuiStyle _s_first = ImGui::GetStyle();
		ImGui::GetStyle().FramePadding = { 0,0 };
		ImGui::GetStyle().WindowPadding = { 0,0 };
		ImGui::GetStyle().DisplayWindowPadding = { 0,0 };
		ImGui::Begin(alib_strfmt("##__RawBeginRoot_%d", m_uielement___RawAmt++), NULL, __flags);
		ImGui::GetStyle() = _s_first;
	}
	void EndEmptyStatic() {
		ScrollEmptySpace();
		ImGui::SetCursorScreenPos(m_uielement___lastwindowpos_stack.back());
		ImGui::End();
	}
	// Allows a element to be scrolled via dragging.
	void BeginDragScrollable(const char* name, bool* _open = 0, ImGuiWindowFlags flags = 0) {
		ImGui::Begin(name, _open, flags);
	}
	void EndDragScrollable() {
		ScrollEmptySpace();
		ImGui::End();
	}
	// Allows a element to be scrolled via dragging.
	void BeginDragScrollableChild(const char* name, ImVec2 size = { 0,0 }, bool border = 0, ImGuiWindowFlags flags = 0) {
		ImGui::BeginChild(name, size, border, flags);
	}
	void EndDragScrollableChild() {
		ScrollEmptySpace();
		ImGui::EndChild();
	}
	static inline void RectFilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0) {
		ImDrawList* draw_list = GetWindowDrawList();
		draw_list->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), col, border_radii, 0);
	}
	static inline void RectUnfilled(ImVec2 pos, ImVec2 size, ImColor col, float border_radii = 0, float thickness = 4.0f) {
		ImDrawList* draw_list = GetWindowDrawList();
		draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), col, border_radii, 0, thickness);
	}
	static inline void DrawLine(ImVec2 start, ImVec2 end, ImColor col = IM_COL32(255, 0, 0, 255), float thickness = 3.0f) {
		ImDrawList* draw_list = GetWindowDrawList();
		draw_list->AddLine(start, end, col, thickness);
	}
	static inline void TextForeground(const char* text, ImVec2 pos) {
		//ImVec2 tsz = CalcTextSize(text);
		//ImVec4 tclp = { pos.x - tsz.x * 0.5f,pos.x- tsz.y * 0.5f, pos.x + tsz.x * 1.5f,pos.x + tsz.y * 1.5f };
		ImGui::GetForegroundDrawList()->AddText(pos, GetColorU32(ImGuiCol_Text), text);
		//ImGui::GetCurrentContext()->Font->RenderText(ImGui::GetForegroundDrawList(), ImGui::GetCurrentContext()->FontSize, pos, ImRGB(128, 128, 255), tclp, text, 0);
	}

	static inline void Circle(ImVec2 center, float radius, ImColor col = IM_COL32(255, 0, 0, 255), bool filled = true) {
		if (!filled) { ImGui::GetWindowDrawList()->AddCircle(center, radius, col, 0, 12); }
		if (filled) { ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, col, 0); }
	}

	// Create a loading bar, where pos = the top left most corner where bar will be placed
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the box, ImGuiCol_ButtonActive for the indicator)
	static inline void LoadingBar(float percentage, ImVec2 size = { 0,0 }, float border_thickness = 12.0f, float rounding = -1.0f) {
		ImVec2 pos = ImGui::GetCursorScreenPos();
		if (IsEmptyRect(size)) {
			size.x = { ImGui::GetIO().DisplaySize.x - GetStyle().ItemSpacing.x * 4 };
			size.y = { CalcTextSize("#").y };
		}
		if (rounding == -1.0f) {
			rounding = CalcTextSize("#").x * 0.25f;
		}
		alib_clampptr(&percentage, 0.0f, 100.0f);
		ImVec2 local = ElementLocalPositionFromStatic(pos);
		ImVec4 r = {
			pos.x + local.x + border_thickness,
			pos.y + local.y + border_thickness,
			alib_percentf(size.x, percentage),
			size.y
		};
		// shorthand for window_drawlist->RectFilled()
		RectFilled({ r.x,r.y }, { r.z, r.w }, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive), rounding);
		ImVec4 __col = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		ImRGB __out_col = {
			(unsigned char)(__col.x * 255),
			(unsigned char)(__col.y * 255),
			(unsigned char)(__col.z * 255),
			255
		};
		// We need to move it because as it is, we're just drawing directly to the clipping plane, so we need to offset by the window's position to keep the same position across movement.
		ImVec2 __pos_overlay = {
			pos.x + local.x + border_thickness,
			pos.y + local.y + border_thickness
		};
		RectUnfilled(__pos_overlay, size, __out_col, rounding);
		ImGui::SetCursorPosY(ImGui::GetCursorPosX() + size.y);
	}
	// Create a loading circle
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the background, ImGuiCol_ButtonActive for the indicator)
	static inline void LoadingCircle(float percentage, float radius = 0) {
		if (radius == 0) { radius = ImGui::GetStyle().ItemInnerSpacing.x * 5 + 1; }
		ImVec2 pos = GetCursorScreenPos();
		ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		col.w = 255;
		// The center's y position needs to  be 2*radius because it'll clip otherwise.
		ImVec2 c = { pos.x + radius, pos.y + (radius * 2) };
		alib_clampptr(&percentage, 0.0f, 100.0f);
		ImDrawList* dl = ImGui::GetWindowDrawList();
		float x, y = 0;
		std::vector<ImVec2> points;
		// Only put a point in the center if the shape wouldn't already be a circle.
		if (percentage != 100) {
			points.push_back(c);
		}
		// These need to be these numbers to be smooth. Don't worry, it doesn't add much overhead.
		float amt = (percentage * 0.01) * (M_PI * 2);
		float amt_step = 0.01f;
		double steps;
		for (float i = 0; i <= amt;) {
			x = radius * cos(i) + c.x;
			y = radius * sin(i) + c.y;
			ImVec2 _first(x, y);
			i = i + amt_step;
			x = radius * cos(i) + c.x;
			y = radius * sin(i) + c.y;
			ImVec2 _second(x, y);
			i = i + amt_step;
			points.push_back(_first);
			points.push_back(_second);
			steps++;
		}

		// background circle
		dl->AddCircleFilled(c, radius * 1.05f, ImGui::GetColorU32(ImGuiCol_Button));
		dl->AddConvexPolyFilled(points.data(), points.size(), ImGui::ColorConvertFloat4ToU32(col));
		ImGui::SetCursorPosY((ImGui::GetCursorPosY() + radius * 2) + ImGui::GetStyle().ItemSpacing.y);
	}
	// Create a loading circle  that's hollow (see: https://img.icons8.com/office/480/spinning-circle.png)
	// Percentage must be 0-100, otherwise it will just clamp to 100.
	// Uses colors from the Button colorset (ImGuiCol_Button for the background, ImGuiCol_ButtonActive for the indicator)
	static inline void LoadingCircleHollow(float percentage, float radius = 0) {
		if (radius == 0) { radius = ImGui::GetStyle().ItemInnerSpacing.x * 5 + 1; }
		ImVec2 pos = GetCursorScreenPos();
		ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		col.w = 255;
		ImVec2 c = { pos.x + radius, pos.y + (radius * 2) };
		alib_clampptr(&percentage, 0.0f, 100.0f);
		ImDrawList* dl = ImGui::GetWindowDrawList();
		float x, y = 0;
		std::vector<ImVec2> points;
		// These need to be these numbers to be smooth. Don't worry, it doesn't add much overhead.
		float amt = (percentage * 0.01) * (M_PI * 2);
		float amt_step = 0.01f;
		double steps;
		for (float i = 0; i <= amt;) {
			x = radius * cos(i) + c.x;
			y = radius * sin(i) + c.y;
			ImVec2 _first(x, y);
			i = i + amt_step;
			x = radius * cos(i) + c.x;
			y = radius * sin(i) + c.y;
			ImVec2 _second(x, y);
			i = i + amt_step;
			points.push_back(_first);
			points.push_back(_second);
			steps++;
		}
		// background circle
		dl->AddCircle(c, radius, ImGui::GetColorU32(ImGuiCol_Button), 0, radius * 0.185f);
		dl->AddPolyline(points.data(), points.size(), ImGui::ColorConvertFloat4ToU32(col), ImDrawFlags_RoundCornersAll, radius * 0.2f);
		ImGui::SetCursorPosY((ImGui::GetCursorPosY() + radius * 2) + ImGui::GetStyle().ItemSpacing.y);
	}
	static inline void InputTextPositioned(const char* label, char* buf, size_t buf_size, ImVec2 position, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = 0, void* user_data = 0) {
		ImVec2 l_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(position);
		ImGui::SetNextItemWidth(-1);
		ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
		ImGui::SetCursorPos(l_pos);
	}
	static inline void ImagePositioned(ImTextureID texid, ImVec2 begin, ImVec2 size, bool centered = true) {
		ImVec2 l_pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(begin.x - 1, begin.y - 1));
		ImGui::GetWindowDrawList()->AddImage(texid, { begin.x - (size.x * 0.5f), begin.y - (size.y * 0.5f) }, { begin.x + size.x, begin.y + size.y });
		ImGui::SetCursorPos(l_pos);
	}
	// An arrow drawn to look like a "back arrow" from android. Defaults to  pointing left.
	// See: https://i.stack.imgur.com/avYnD.png
	// Inherits colors from ImGuiCol_ButtonActive
	bool AndroidArrowButton(const char* str_id, ImGuiDir direction = ImGuiDir_Left, float thickness = 6.5f) {
		ImVec2 s_p = ImGui::GetCursorScreenPos();
		float height = ImGui::GetFrameHeight() * 0.75f;
		float width = height;
		bool ret = false;
		ImVec2 center = {
			s_p.x + (width * 0.5f),
			s_p.y + (height * 0.5f)
		};
		ImVec2 center_top = {
			center.x,
			center.y + (height * 0.5f)
		};
		ImVec2 center_bottom = {
			center.x,
			center.y - (height * 0.5f)
		};
		ImVec2 center_left = {
			center.x - (width * 0.5f),
			center.y
		};
		ImVec2 center_right = {
			center.x + (width * 0.5f),
			center.y
		};
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked()) { ret = true; }
		ImColor col_btn = GetStyle().Colors[ImGuiCol_ButtonActive];
		switch (direction) {
		case ImGuiDir_Up: {
			ImGui::DrawLine(center_top, center_bottom, col_btn, thickness);
			ImGui::DrawLine(center_left, center_bottom, col_btn, thickness);
			ImGui::DrawLine(center_right, center_bottom, col_btn, thickness);
		} break;
		case ImGuiDir_Down: {
			ImGui::DrawLine(center_bottom, center_top, col_btn, thickness);
			ImGui::DrawLine(center_left, center_top, col_btn, thickness);
			ImGui::DrawLine(center_right, center_top, col_btn, thickness);
		} break;
		case ImGuiDir_Left: {
			ImGui::DrawLine(center_left, center_right, col_btn, thickness);
			ImGui::DrawLine(center_left, center_top, col_btn, thickness);
			ImGui::DrawLine(center_left, center_bottom, col_btn, thickness);
		} break;
		case ImGuiDir_Right: {
			ImGui::DrawLine(center_right, center_left, col_btn, thickness);
			ImGui::DrawLine(center_right, center_top, col_btn, thickness);
			ImGui::DrawLine(center_right, center_bottom, col_btn, thickness);
		} break;
		}
		ImGui::SetCursorPosY(s_p.y + height + ImGui::GetStyle().ItemSpacing.y);

		return ret;
	}
	ImVec2 GetDefaultFABSize() {
		float height = ImGui::GetStyle().ItemSpacing.y * 1.5f;
		// FABs are always a square.
		return { height, height };
	}
	ImVec2 GetDefaultFABLocation() {
		ImVec2 displ_size = ImGui::GetIO().DisplaySize;
		ImVec2 ideal = GetDefaultFABSize();
		return { displ_size.x - ideal.x * 1.35f, displ_size.y - ideal.y * 1.35f };
	}
	// Styled as a FAB from android
	// See:  https://developer.android.com/training/material/images/fab.png
	// Inherits color from ImGuiCol_Button, ImGuiCol_ButtonActive (Unpressed, pressed)
	// Everything is shown in foreground, so don't worry about using this inside an imgui window.
	bool FloatingActionButton(const char* label, bool* v = NULL) {
		ImVec2 sp = ImGui::GetCursorScreenPos();
		ImVec2 p = ImGui::GetCursorPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 ideal_size = GetDefaultFABSize();
		ImVec2 ideal_pos = GetDefaultFABLocation();
		float height = ideal_size.y;
		float width = ideal_size.x;
		float radius = height * 0.5f;
		bool ret = false;
		ImGui::SetCursorScreenPos(ideal_pos);
		ImGui::Button(label, { width, height });
		if (ImGui::IsItemClicked()) {
			if (v != NULL) {
				*v = !*v;
			}
			ret = true;
		}
		ImVec4* colors = ImGui::GetStyle().Colors;
		ImColor col_btn_active = GetStyle().Colors[ImGuiCol_ButtonActive];
		ImColor col_btn = GetStyle().Colors[ImGuiCol_Button];
		ImGui::GetForegroundDrawList()->AddRectFilled(ideal_pos, ideal_size, ret ? col_btn_active : col_btn, radius);
		//ImGui::GetForegroundDrawList()->AddCircleFilled(ideal_pos, radius, ret ? col_btn_active : col_btn);
		ImVec2 __offset = { ideal_size.x * 0.3f, ideal_size.y * 0.3f };
		ImGui::TextForeground(std::string(label, 1).c_str(), { ideal_pos.x + __offset.x, ideal_pos.y + __offset.y });
		ImGui::SetCursorScreenPos(sp);
		ImGui::SetCursorPos(p);
		return ret;
	}
	typedef int ImGuiMenuButton;
	// If btn_type == ..Cookie:
	//		will draw a elipsis-like shape
	// See: https://i.stack.imgur.com/YIcbV.png
	// Inherits colors from ImGuiCol_ButtonActive
	bool MenuButton(const char* str_id, bool* v, ImVec2 pos, ImGuiMenuButton btn_type = ImGuiMenuButton_Hamburger) {
		ImVec2 s_p = ImGui::GetCursorScreenPos();
		ImVec2 ideal_pos = pos;
		float btn_height = ImGui::GetFrameHeight();
		float btn_width = btn_height * 1.25f;
		float elem_height = btn_height * 0.25f;
		float radius = elem_height * 0.5f;
		bool ret = false;
		// Prefrer to use a square button instead of a rectangle button
		ImGui::InvisibleButton(str_id, { btn_width, btn_height });
		if (ImGui::IsItemClicked()) { *v = !*v; }
		ImColor col_btn = *v ? GetStyle().Colors[ImGuiCol_ButtonActive] : GetStyle().Colors[ImGuiCol_Button];
		switch (btn_type) {
		case ImGuiMenuButton_Hamburger: {
			ImGui::RectFilled(ideal_pos, { btn_width, elem_height }, col_btn);
			ImGui::RectFilled({ ideal_pos.x, ideal_pos.y + elem_height * 1.5f }, { btn_width, elem_height }, col_btn);
			ImGui::RectFilled({ ideal_pos.x, ideal_pos.y + elem_height * 3.0f }, { btn_width, elem_height }, col_btn);
		}break;
		case ImGuiMenuButton_Doner: {
		} break;
		case ImGuiMenuButton_Bento: {} break;
		case ImGuiMenuButton_Kebab: {} break;
		case ImGuiMenuButton_Meatball: {} break;
		case ImGuiMenuButton_Cookie: {} break;

		}
		return ret;
	}
	struct MarkdownVars {
		struct MarkdownHeader {
			int y_off;
		};
		// header name (first N characters) : header data
		std::vector<std::pair<std::string, MarkdownHeader>> headdir;
		float element_border_width;

	};
	ImVec2 WrappedSize() {
		return ImGui::GetCurrentWindow()->DC.PrevLineSize;
	}

	ImVec2 CalcVerticalTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) {
		const ImVec2 rv = ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
		return ImVec2(rv.y, rv.x);
	}

	/// Draws vertical text. The position is the bottom left of the text rect.
	inline void AddTextVertical(ImDrawList* DrawList, const char* text, ImVec2 pos, ImU32 text_color) {
		pos.x = IM_ROUND(pos.x);
		pos.y = IM_ROUND(pos.y);
		ImFont* font = GImGui->Font;
		const ImFontGlyph* glyph;
		char c;
		ImGuiContext& g = *GImGui;
		ImVec2 text_size = CalcTextSize(text);
		while ((c = *text++)) {
			glyph = font->FindGlyph(c);
			if (!glyph) continue;

			DrawList->PrimReserve(6, 4);
			DrawList->PrimQuadUV(
				pos + ImVec2(glyph->Y0, -glyph->X0),
				pos + ImVec2(glyph->Y0, -glyph->X1),
				pos + ImVec2(glyph->Y1, -glyph->X1),
				pos + ImVec2(glyph->Y1, -glyph->X0),

				ImVec2(glyph->U0, glyph->V0),
				ImVec2(glyph->U1, glyph->V0),
				ImVec2(glyph->U1, glyph->V1),
				ImVec2(glyph->U0, glyph->V1),
				text_color);
			pos.y -= glyph->AdvanceX;
		}
	}
	int rotation_start_index;
	int scale_start_index;
	ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }
	ImVec2 ImElemCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
	}
	void ImRotateEnd(float a, ImVec2 center = ImElemCenter())
	{
		float angle = (a + 90) * M_PI / 180;
		// Make sure angle is oriented upright
		float s = sin(angle), c = cos(angle);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}
	void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	// Android styled thumb switch 
	// See: https://miro.medium.com/max/420/1*ccfEt-tGF1bDO6sV_yNc8w.gif
	void ThumbSwitch(const char* str_id, bool* v, const char* fmt = "\0", ...)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 p_t = { p.x, p.y };
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight() * 0.8f;
		float width = height * 2.0f;
		float radius = height * 0.5f;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;
		float t = *v ? 1.0f : 0.0f;
		ImVec4* colors = ImGui::GetStyle().Colors;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}
		draw_list->AddRectFilled(p_t, ImVec2(p_t.x + width, p_t.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]), height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p_t.x + radius + t * (width - radius * 2.0f), p_t.y + radius), radius - 1.5f, IM_COL32(245, 245, 245, 200));

		if (fmt[0]) {
			va_list args;
			va_start(args, fmt);
			ImGui::SameLine();
			TextV(fmt, args);
			va_end(args);
		}
		ImGui::SetCursorScreenPos({ p.x, p.y + (height * 1.25f) });
	}

	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
	{
		// Load from file
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create a OpenGL texture identifier
		GLuint image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);

		*out_texture = image_texture;
		*out_width = image_width;
		*out_height = image_height;

		return true;
	}
	bool LoadTextureFromMemory(unsigned char* data, size_t length, GLuint* out_texture, int* out_width, int* out_height)
	{
		// Load from file
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load_from_memory(data, length, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create a OpenGL texture identifier
		GLuint image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);

		*out_texture = image_texture;
		*out_width = image_width;
		*out_height = image_height;

		return true;
	}
};

#pragma warning(pop)
#endif