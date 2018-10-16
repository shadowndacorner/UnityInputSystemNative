#pragma once
#include <inttypes.h>

#if defined(_WIN32)
#define UINPUT_EXPORT __declspec(dllexport)
#elif defined(UINPUT_LINUX) || defined(UINPUT_OSX) || defined(EMSCRIPTEN)
#define UINPUT_EXPORT
#else
#error Export not defined
#endif

extern "C"
{
	enum ButtonsConstants
	{
		DPadUp = 0x00000001,
		DPadDown = 0x00000002,
		DPadLeft = 0x00000004,
		DPadRight = 0x00000008,
		Start = 0x00000010,
		Back = 0x00000020,
		LeftThumb = 0x00000040,
		RightThumb = 0x00000080,
		LeftShoulder = 0x0100,
		RightShoulder = 0x0200,
		Guide = 0x0400,
		A = 0x1000,
		B = 0x2000,
		X = 0x4000,
		Y = 0x8000
	};

	// Compatibility with XInputDotNet
    struct UInputGamepad
    {
        uint16_t wButtons;
        uint8_t bLeftTrigger;
        uint8_t bRightTrigger;
        int16_t sThumbLX;
        int16_t sThumbLY;
        int16_t sThumbRX;
        int16_t sThumbRY;
    };

    struct UInputState
    {
        uint32_t packetNumber;
        UInputGamepad Gamepad;
    };

	struct UInputPlugin
	{
		typedef void (*UInputUpdateFunc)();
		typedef uint32_t (*UInputGamepadCountFunc)();
		typedef uint32_t (*UInputGamePadGetStateFunc)(uint32_t dwUserIndex, UInputState *pState);
		typedef void (*UInputGamePadSetStateFunc)(uint32_t dwUserIndex, float leftMotor, float rightMotor);

		UInputUpdateFunc Update;
		UInputGamepadCountFunc GamepadCount;
		UInputGamePadGetStateFunc GetState;
		UInputGamePadSetStateFunc SetState;
	};
}