#include "common.h"

extern "C"
{
	UINPUT_EXPORT void UInputUpdate();
	UINPUT_EXPORT uint32_t UInputGamepadCount();

	UINPUT_EXPORT uint32_t UInputGamePadGetState(uint32_t dwUserIndex, UInputState *pState);
	UINPUT_EXPORT void UInputGamePadSetState(uint32_t dwUserIndex, float leftMotor, float rightMotor);
}