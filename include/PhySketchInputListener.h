#ifndef PhySketchInputListener_h__
#define PhySketchInputListener_h__
#pragma once

#include "PhySketchDefinitions.h"

namespace PhySketch
{	
	enum MouseButton;
	enum Key;


	class InputListener
	{
	public:
		InputListener();
		virtual ~InputListener() {};

		virtual void keyPressed(Key key) = 0;
		virtual void keyReleased(Key key) = 0;

		virtual void mouseDown(MouseButton button, int x, int y) = 0;
		virtual void mouseUp(MouseButton button, int x, int y) = 0;
		virtual void mouseMoved(int x, int y) = 0;

	};

	
	enum MouseButton
	{
		MB_Left		= 1,
		MB_Middle	= 2,
		MB_Right	= 3
	};

	enum Key
	{
		KEY_Back		= 1,
		KEY_Tab			= 2,
		KEY_Clear		= 3,
		KEY_Return		= 4,
		KEY_Shift		= 5,
		KEY_Control		= 6,
		KEY_Menu		= 7,
		KEY_Pause		= 8,
		KEY_Capital		= 9,
		KEY_Kana		= 10,
		KEY_Hangul		= 11,
		KEY_Junja		= 12,
		KEY_Final		= 13,
		KEY_Hanja		= 14,
		KEY_Kanji		= 15,
		KEY_Escape		= 16,
		KEY_Convert		= 17,
		KEY_Nonconvert	= 18,
		KEY_Accept		= 19,
		KEY_Modechange	= 20,
		KEY_Space		= 21,
		KEY_Prior		= 22,
		KEY_Next		= 23,
		KEY_End			= 24,
		KEY_Home		= 25,
		KEY_Left		= 26,
		KEY_Up			= 27,
		KEY_Right		= 28,
		KEY_Down		= 29,
		KEY_Select		= 30,
		KEY_Print		= 31,
		KEY_Execute		= 32,
		KEY_Snapshot	= 33,
		KEY_Insert		= 34,
		KEY_Delete		= 35,
		KEY_Help		= 36,
		KEY_Lwin		= 37,
		KEY_Rwin		= 38,
		KEY_Apps		= 39,
		KEY_Sleep		= 40,
		KEY_Numpad0		= 41,
		KEY_Numpad1		= 42,
		KEY_Numpad2		= 43,
		KEY_Numpad3		= 44,
		KEY_Numpad4		= 45,
		KEY_Numpad5		= 46,
		KEY_Numpad6		= 47,
		KEY_Numpad7		= 48,
		KEY_Numpad8		= 49,
		KEY_Numpad9		= 50,
		KEY_Multiply	= 51,
		KEY_Add			= 52,
		KEY_Separator	= 53,
		KEY_Subtract	= 54,
		KEY_Decimal		= 55,
		KEY_Divide		= 56,
		KEY_F1			= 57,
		KEY_F2			= 58,
		KEY_F3			= 59,
		KEY_F4			= 60,
		KEY_F5			= 61,
		KEY_F6			= 62,
		KEY_F7			= 63,
		KEY_F8			= 64,
		KEY_F9			= 65,
		KEY_F10			= 66,
		KEY_F11			= 67,
		KEY_F12			= 68,
		KEY_F13			= 69,
		KEY_F14			= 70,
		KEY_F15			= 71,
		KEY_F16			= 72,
		KEY_F17			= 73,
		KEY_F18			= 74,
		KEY_F19			= 75,
		KEY_F20			= 76,
		KEY_F21			= 77,
		KEY_F22			= 78,
		KEY_F23			= 79,
		KEY_F24			= 80,
		KEY_Numlock		= 81,
		KEY_Scrolllock	= 82,
		KEY_OEM_Plus	= 83,
		KEY_OEM_Comma	= 84,
		KEY_OEM_Minus	= 85,
		KEY_OEM_Period	= 86,
		KEY_Q			= 87,
		KEY_W			= 88,
		KEY_E			= 89,
		KEY_R			= 90,
		KEY_T			= 91,
		KEY_Y			= 92,
		KEY_U			= 93,
		KEY_I			= 94,
		KEY_O			= 95,
		KEY_P			= 96,
		KEY_A			= 97,
		KEY_S			= 98,
		KEY_D			= 99,
		KEY_F			= 100,
		KEY_G			= 101,
		KEY_H			= 102,
		KEY_J			= 103,
		KEY_K			= 104,
		KEY_L			= 105,
		KEY_Z			= 106,
		KEY_X			= 107,
		KEY_C			= 108,
		KEY_V			= 109,
		KEY_B			= 110,
		KEY_N			= 111,
		KEY_M			= 112

	};
}
#endif // PhySketchInputListener_h__