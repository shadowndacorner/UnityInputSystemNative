#include <unityinput/common.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

#include <unityinput/exports.h>
#include <unordered_map>

enum GamepadGetStateReturns : uint32_t
{
	Success = 0,
	NotConnected = 0,
	Failure = 1
};

class GameControllerWrapper
{
public:
	GameControllerWrapper() :
		controller(NULL),
		joystickID(-1) {}
	GameControllerWrapper(int id) :
		controller(SDL_GameControllerOpen(id)),
		joystickID(id) {}

	GameControllerWrapper(GameControllerWrapper&& rhs) :
		controller(rhs.controller), joystickID(rhs.joystickID)
	{
		rhs.controller = NULL;
		rhs.joystickID = ~0;
	}

	~GameControllerWrapper()
	{
		if (controller)
		{
			SDL_GameControllerClose(controller);
		}
	}

public:
	bool GetAttached()
	{
		return SDL_GameControllerGetAttached(controller) == SDL_TRUE;
	}

	bool GetButton(SDL_GameControllerButton button)
	{
		return SDL_GameControllerGetButton(controller, button) != SDL_FALSE;
	}

	uint16_t GetButtonAsMask(SDL_GameControllerButton button, uint16_t mask)
	{
		if (GetButton(button))
			return mask;

		return 0;
	}

	SDL_Joystick* GetJoystick()
	{
		return SDL_GameControllerGetJoystick(controller);
	}

	uint16_t GetUInputButtons()
	{
		uint16_t result =
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A, ButtonsConstants::A) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B, ButtonsConstants::B) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X, ButtonsConstants::X) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y, ButtonsConstants::Y) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK, ButtonsConstants::Back) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START, ButtonsConstants::Start) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP, ButtonsConstants::DPadUp) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN, ButtonsConstants::DPadDown) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT, ButtonsConstants::DPadLeft) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT, ButtonsConstants::DPadRight) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER, ButtonsConstants::LeftShoulder) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, ButtonsConstants::RightShoulder) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK, ButtonsConstants::RightThumb) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK, ButtonsConstants::LeftThumb) |
			GetButtonAsMask(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE, ButtonsConstants::Guide)
		;
		
		return result;
	}

	int16_t GetAxis(SDL_GameControllerAxis axis)
	{
		return SDL_GameControllerGetAxis(controller, axis);
	}

private:
	SDL_GameController * controller;
	int joystickID;
};

class InputSystemState
{
public:
	void JoystickAdded(SDL_ControllerDeviceEvent& ev)
	{
		int index = 0;
		while (controllers.count(index) > 0) { ++index; }
		if (index > maxIndex)
		{
			maxIndex = index;
		}

		auto wrapper = GameControllerWrapper(ev.which);
		auto joy = wrapper.GetJoystick();
		for (auto& it : controllers)
		{
			if (it.second.GetJoystick() == joy)
				return;
		}
		controllers.insert({ index, std::move(wrapper) });
	}

	void JoystickRemoved(SDL_ControllerDeviceEvent& ev)
	{

	}

	InputSystemState() : maxIndex(0)
	{
		if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
			SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
			return;
		}
	}

	~InputSystemState()
	{
		// destroy all gamepads before quitting
		controllers.clear();
		SDL_Quit();
	}

	void Update() {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type)
			{
			case SDL_CONTROLLERDEVICEADDED:
				JoystickAdded(ev.cdevice);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				JoystickRemoved(ev.cdevice);
				break;
			}
		}

		// Check if any controllers have been disconnected
		while (true)
		{
			bool passed = true;
			for (auto& it : controllers) {
				if (!it.second.GetAttached()) {
					controllers.erase(it.first);
					passed = false;
					break;
				}
			}

			if (passed)
				break;
		}
	}

	bool ApplyState(uint32_t _ind, UInputGamepad& pad)
	{
		int index = int(_ind);
		if (controllers.count(index) == 0)
		{
			return false;
		}

		auto& cnt = controllers[index];
		pad.wButtons = cnt.GetUInputButtons();
		pad.sThumbLX = cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
		pad.sThumbRX = cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
		pad.sThumbLY = -cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
		pad.sThumbRY = -cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);
		
		auto left = float(cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT)) / 32767.f * 255.f;
		auto right = float(cnt.GetAxis(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) / 32767.f * 255.f;

		// TODO: this is slow
		if (left < 0)
			left = 0;
		if (left > 255)
			left = 255;
		if (right < 0)
			right = 0;
		if (right > 255)
			right = 255;

		pad.bLeftTrigger = uint8_t(left);
		pad.bRightTrigger = uint8_t(right);
		return true;
	}

	int maxIndex;
	std::unordered_map<int, GameControllerWrapper> controllers;
};

static InputSystemState& State()
{
	static InputSystemState _init;
	return _init;
}

extern "C"
{
	UINPUT_EXPORT void UInputUpdate()
	{
		State().Update();
	}

	UINPUT_EXPORT uint32_t UInputGamepadCount()
	{
		return State().maxIndex + 1;
	}

	UINPUT_EXPORT uint32_t UInputGamePadGetState(uint32_t dwUserIndex, UInputState* _state)
	{
		if (!_state || dwUserIndex >= UInputGamepadCount())
		{
			return Failure;
		}

		auto& input = State();
		auto& state = _state->Gamepad;
		if (State().ApplyState(dwUserIndex, state))
		{
			return Success;
		}
		return Failure;
	}

	UINPUT_EXPORT void UInputGamePadSetState(uint32_t userIndex, float leftMotor, float rightMotor)
	{
		State();
		// TODO: Vibration w/ SDL

	}
}