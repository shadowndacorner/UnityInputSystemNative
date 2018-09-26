#include <iostream>
#include <unityinput/common.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <thread>
#include <chrono>
#ifndef _WIN32
#include <unistd.h>
#endif

int main(int argc, char** argv)
{
	#if __WIN32
	auto dll = SDL_LoadObject("uinput.dll");
	#elif defined(__APPLE__)
	auto dll = SDL_LoadObject("libuinput.dylib");
	#else
	auto dll = SDL_LoadObject("libuinput.so");
	#endif
	UInputPlugin plugin;
	plugin.Update = (UInputPlugin::UInputUpdateFunc)SDL_LoadFunction(dll, "UInputUpdate");
	plugin.GetState = (UInputPlugin::UInputGamePadGetStateFunc)SDL_LoadFunction(dll, "UInputGamePadGetState");
	plugin.SetState = (UInputPlugin::UInputGamePadSetStateFunc)SDL_LoadFunction(dll, "UInputGamePadSetState");
	plugin.GamepadCount = (UInputPlugin::UInputGamepadCountFunc)SDL_LoadFunction(dll, "UInputGamepadCount");

	std::cout << (plugin.Update) << std::endl;
	std::cout << (plugin.GetState) << std::endl;
	std::cout << (plugin.SetState) << std::endl;
	std::cout << (plugin.GamepadCount) << std::endl;

	// This is awful but meh
	using namespace std;
	while (true)
	{
		if (int(plugin.Update == NULL) + int(plugin.GetState == NULL) + int(plugin.SetState == NULL) + int(plugin.GamepadCount == NULL) != 0)
		{
			cout << (plugin.Update != 0) << ":" << (plugin.GetState != 0) << ":" << (plugin.SetState != 0) << ":" << (plugin.GamepadCount != 0) << std::endl;
			//cout << "Something is null" << endl;
		}
		else
		{
			plugin.Update();

			auto cnt = plugin.GamepadCount();
			cout << cnt << " gamepads loaded" << endl;
			
			UInputState state;
			for (uint32_t i = 0; i < cnt; ++i)
			{
				if (plugin.GetState(i, &state) == 0)
				{
					cout << "Gamepad " << i << ": Connected" << endl;
					cout << "\tA: " << (((state.Gamepad.wButtons & ButtonsConstants::A) != 0) ? "Pressed" : "Released") << endl;
					cout << "\tB: " << (((state.Gamepad.wButtons & ButtonsConstants::B) != 0) ? "Pressed" : "Released") << endl;
					cout << "\tX: " << (((state.Gamepad.wButtons & ButtonsConstants::X) != 0) ? "Pressed" : "Released") << endl;
					cout << "\tY: " << (((state.Gamepad.wButtons & ButtonsConstants::Y) != 0) ? "Pressed" : "Released") << endl;
					cout << "Left stick Y" << state.Gamepad.sThumbLY << std::endl;
				}
				else
				{
					cout << "Gamepad " << i << ": Disconnected" << endl;
				}
				cout << endl;
			}
		}

		{
#ifdef _WIN32
			using namespace chrono_literals;
			this_thread::sleep_for(100ms);
			system("cls");
#else
			usleep(100000);
#endif
		}
	}
}