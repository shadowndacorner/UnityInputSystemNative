# Unity Input System - Native API

This is the native code backing my input system.  It's more or less a dropin replacement for XInputDotNet's XInputInterface.dll, except that it uses SDL_GameController rather than XInput behind the scenes.  It also replaced XInput with UInput as a naming convention, mostly so that there is a clear distinction for me.  Ideally, I want to eventually get rid of the XInput-like usage altogether, but for now, this sort of hybrid works.

## How It Works
Pretty much all this library does is gamepad tracking (with a super simple id allocation "algorithm" that sort of follows how XInput does it, but may not match Xbox 360 controllers lights) along with converting SDL gamepad state into an XInput-like state structure.  It's pretty simple, but works well enough for me.