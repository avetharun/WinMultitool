#ifndef __htgnative_imgui_format_h_
#define __htgnative_imgui_format_h_
#pragma warning( push )

#pragma warning( disable : 4244 )

#include "imgui.h"
#include "imgui_internal.h"
#include "alib.hpp"

namespace ImGui {
	const char ColorMarkerStart = '\\';
	const char ColorMarkerEnd = ']';
	/// <summary>
	/// HSV : Stub for a HSV (hue saturation value) triplet. Don't use, instead use ImRGB, as it contains functions to interperet this
	/// </summary>
	struct ImHSV {
		float hue, saturation, value;
	};
	/// <summary>
	/// HSL : Stub for a HSL (hue saturation luma) triplet. Don't use, instead use ImRGB, as it contains functions to interperet this
	/// Identical to ImHSV, except can convert to HSV, which is used instead of this.
	/// </summary>
	struct ImHSL {
		float hue, saturation, luma;
		operator ImHSV() { return { hue, saturation, luma }; }
		ImHSL(ImHSV _hsv) {
			hue = _hsv.hue;
			saturation = _hsv.saturation;
			luma = _hsv.value;
		}
	};
	struct ImRGB {
		unsigned char colour[4] = { 255,255,255,255 };
		unsigned char getr() {
			return (colour)[0];
		}
		unsigned char getg() {
			return (colour)[1];
		}
		unsigned char getb() {
			return (colour)[2];
		}
		unsigned char geta() {
			return (colour)[3];
		}
		ImHSV convertHSV() {
			ImHSV _hsv;
			ImGui::ColorConvertRGBtoHSV(getr(), getg(), getb(), _hsv.hue, _hsv.saturation, _hsv.value);
			return _hsv;
		}
		void setr(unsigned char _r) {
			colour[0] = _r;
		}
		void setg(unsigned char _g) {
			colour[1] = _g;
		}
		void setb(unsigned char _b) {
			colour[2] = _b;
		}
		void seta(unsigned char _a) {
			colour[3] = _a;
		}
		void hue(float _h) {
			ImHSV _hsv = convertHSV();
			_hsv.hue += _h;
			float _r = getr() / 255.0f;
			float _g = getg() / 255.0f;
			float _b = getb() / 255.0f;
			ImGui::ColorConvertHSVtoRGB(_hsv.hue, _hsv.saturation, _hsv.value, _r, _g, _b);
			setr(_r);
			setg(_g);
			setb(_b);
		}
		void saturation(float _s) {
			ImHSV _hsv = convertHSV();
			_hsv.saturation *= _s;
			float _r = getr() / 255.0f;
			float _g = getg() / 255.0f;
			float _b = getb() / 255.0f;
			ImGui::ColorConvertHSVtoRGB(_hsv.hue, _hsv.saturation, _hsv.value, _r, _g, _b);
			setr(_r);
			setg(_g);
			setb(_b);
		}
		void value(float _v) {
			ImHSV _hsv = convertHSV();
			_hsv.value *= _v;
			float _r = getr() / 255.0f;
			float _g = getg() / 255.0f;
			float _b = getb() / 255.0f;
			ImGui::ColorConvertHSVtoRGB(_hsv.hue, _hsv.saturation, _hsv.value, _r, _g, _b);
			setr(_r);
			setg(_g);
			setb(_b);
		}
		void parse_hsv(float h, float s, float v) {
			hue(h);
			saturation(s);
			value(v);
		}
		ImRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
			this->setr(r);
			this->setg(g);
			this->setb(b);
			this->seta(a);
		}
		ImRGB(ImVec4 c) {
			this->setr(c.x * 255.0f);
			this->setg(c.y * 255.0f);
			this->setb(c.z * 255.0f);
			this->seta(c.w * 255.0f);
		}
		ImRGB(ImHSV hsv) {
			float _r;
			float _g;
			float _b;
			ImGui::ColorConvertHSVtoRGB(hsv.hue, hsv.saturation, hsv.value, _r, _g, _b);
		}
		ImRGB() {}
		operator ImColor() { return { getr(), getg(), getb(), geta() }; }
		//operator ImVec4() { return { (float)getr(), (float)getg(), (float)getb(), (float)geta() }; }
		operator ImU32() { return ImGui::ColorConvertFloat4ToU32({ (float)getr(), (float)getg(), (float)getb(), (float)geta() }); }
		// Convert the color to a format understood by TextMulticolored()
		std::string tostring() {
			return alib_strfmts("%c%02x%02x%02x%c", ColorMarkerStart, getr(), getg(), getb(), ColorMarkerEnd);
		}
		// String to reset color
		static std::string resetstr() {
			return alib_strfmts("%c%c", ColorMarkerStart, ColorMarkerEnd);
		}
	};
	ImRGB ColorConvertHSVtoRGB(ImHSV _hsv) { return _hsv; }
	ImHSV ColorConvertRGBtoHSV(ImRGB _rgb) { return _rgb.convertHSV(); }

	bool __ProcessInlineHexColorImpl(const char* start, const char* end, ImVec4& color)
	{
		const int hexCount = (int)(end - start);
		if (hexCount == 6 || hexCount == 8)
		{
			char hex[9];
			strncpy(hex, start, hexCount);
			hex[hexCount] = 0;

			unsigned int hexColor = 0;
			if (sscanf(hex, "%x", &hexColor) > 0)
			{
				color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
				color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
				color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
				color.w = 1.0f;
				if (hexCount == 8)
				{
					color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
				}

				return true;
			}
		}

		return false;
	}

	void TextWrappedUnformatted(const char* text_begin, const char* text_end)
	{
		bool need_backup = (GetCurrentWindow()->DC.TextWrapPos < 0.0f);  // Keep existing wrap position if one is already set
		if (need_backup) {
			PushTextWrapPos(0.0f);
		}
		TextEx(text_begin, text_end, ImGuiTextFlags_NoWidthForLargeClippedText); // Skip formatting
		if (need_backup) {
			PopTextWrapPos();
		}
	}

	void __TextColouredFormattedImplV(const char* fmt, va_list args) {
		char tempStr[4096];
		vsnprintf(tempStr, sizeof(tempStr), fmt, args);

		tempStr[sizeof(tempStr) - 1] = '\0';

		bool pushedColorStyle = false;
		const char* textStart = tempStr;
		const char* textCur = tempStr;
		while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
		{
			if (*textCur == ColorMarkerStart)
			{
				// Print accumulated text
				if (textCur != textStart)
				{
					ImGui::TextUnformatted(textStart, textCur);
					ImGui::SameLine(0.0f, 0.0f);
				}

				// Process color code
				const char* colorStart = textCur + 1;
				do
				{
					++textCur;
				} while (*textCur != '\0' && *textCur != ColorMarkerEnd);

				// Change color
				if (pushedColorStyle)
				{
					ImGui::PopStyleColor();
					pushedColorStyle = false;
				}

				ImVec4 textColor;
				if (__ProcessInlineHexColorImpl(colorStart, textCur, textColor))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
					pushedColorStyle = true;
				}

				textStart = textCur + 1;
			}
			else if (*textCur == '\n')
			{
				// Print accumulated text an go to next line
				ImGui::TextUnformatted(textStart, textCur);
				textStart = textCur + 1;
			}

			++textCur;
		}

		if (textCur != textStart)
		{
			ImGui::TextUnformatted(textStart, textCur);
		}
		else
		{
			ImGui::NewLine();
		}

		if (pushedColorStyle)
		{
			ImGui::PopStyleColor();
		}
	}
	void __TextColouredFormattedImpl(const char* fmt, ...)
	{
		va_list argPtr;
		va_start(argPtr, fmt);
		__TextColouredFormattedImplV(fmt, argPtr);
		va_end(argPtr);
	}
	// Note: multicolored starts are formatted as follows BBGGRRAA. AA is optional.
	void TextMulticolored(const char* fmt, ...) {
		va_list argPtr;
		va_start(argPtr, fmt);
		__TextColouredFormattedImplV(fmt, argPtr);
		va_end(argPtr);
	}
	// Note: multicolored starts are formatted as follows BBGGRRAA. AA is optional.
	void TextMulticoloured(const char* fmt, ...) {
		va_list argPtr;
		va_start(argPtr, fmt);
		__TextColouredFormattedImplV(fmt, argPtr);
		va_end(argPtr);
	}
	// Aligns text to the right of screen, regardless of if there's text there already. Returns the position of the first element.
	ImVec2 TextAlignedRight(const char* fmt, ...) {
		va_list argPtr;
		va_start(argPtr, fmt);
		std::string tmp = alib_strfmtsv(fmt, argPtr);
		ImVec2 sz = ImGui::CalcTextSize(tmp.c_str());
		ImVec2 cont_region = ImGui::GetWindowContentRegionMax();
		ImVec2 cur = ImGui::GetCursorScreenPos();
		ImVec2 pos = { cur.x + (cont_region.x - sz.x) , cur.y };
		ImGui::GetForegroundDrawList()->AddText(pos, GetColorU32(ImGuiCol_Text), tmp.c_str());
		va_end(argPtr);
		return pos;
	}
	ImVec2 CalcTextAlignedRight(const char* fmt, ...) {
		va_list argPtr;
		va_start(argPtr, fmt);
		std::string tmp = alib_strfmtsv(fmt, argPtr);
		ImVec2 sz = ImGui::CalcTextSize(tmp.c_str());
		ImVec2 cont_region = ImGui::GetWindowContentRegionMax();
		ImVec2 cur = ImGui::GetCursorScreenPos();
		ImVec2 pos = { cur.x + (cont_region.x - sz.x) , cur.y };
		va_end(argPtr);
		return pos;
	}
};
typedef ImGui::ImRGB ImRGB;
typedef ImGui::ImHSL ImHSL;
typedef ImGui::ImHSV ImHSV;

#pragma warning( pop )

#endif