#include <scge/Console.h>
#include <scge/Window.h>
#include <scge/AudioDevice.h>
#include <scge/InputDevice.h>
#include <scge/FileSystem.h>
#include <scge/Timer.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowSmd)
{
	FileSystem fileSystem("../../Resources");
	Console console(fileSystem, false);
	
	ResourceManager resourceManager(console);

	AudioDevice audio(console, resourceManager, fileSystem);

	if(audio.Initialise())
		return 0;

	Window window("Void", console, resourceManager, fileSystem, 1280, 720, true, false);
	//Window window("Void", console, resourceManager, fileSystem);
	
	InputDevice &input = window.GetInputDevice();
	if(window.Initialise())
		return 0;

	ConsoleEvent shouldQuitConsole(console, input, "Game.Quit");
	input.addDefaultBinding(Key::Q, shouldQuitConsole);
	input.addDefaultBinding(Key::Escape, shouldQuitConsole);

	ConsoleEvent cameraForward(console, input, "Camera.Move.Forward");
	input.addDefaultBinding(Key::W, cameraForward);
	ConsoleEvent cameraBackward(console, input, "Camera.Move.Backward");
	input.addDefaultBinding(Key::S, cameraBackward);
	ConsoleEvent cameraLeft(console, input, "Camera.Move.Left");
	input.addDefaultBinding(Key::A, cameraLeft);
	ConsoleEvent cameraRight(console, input, "Camera.Move.Right");
	input.addDefaultBinding(Key::D, cameraRight);
	ConsoleEvent cameraUp(console, input, "Camera.Move.Up");
	input.addDefaultBinding(Key::Space, cameraUp);
	ConsoleEvent cameraDown(console, input, "Camera.Move.Down");
	input.addDefaultBinding(Key::LCtrl, cameraDown);
	ConsoleEvent cameraSprint(console, input, "Camera.Move.Sprint");
	input.addDefaultBinding(Key::LShift, cameraSprint);

	window.GetGraphicsDevice().getRenderer3D().setSkybox(resourceManager.getResourceReference<TextureResource>("[Texture] Clouds.dds"));

	Camera &camera = window.GetGraphicsDevice().getRenderer3D().getCamera();
	camera.rotateY(Math::DegreesToRadians(-90));
	camera.setPosition(-1100.0f, -170.0f, 0.0f);

	Timer gameTime;

	bool isMousePressed = false;
	int oldMouseX = 0;
	int oldMouseY = 0;

	for(;;)
	{
		uint64 frameTime = gameTime.getElapsedReset();
		float frameSeconds = frameTime / 1000.0f;

		fileSystem.Update();
		resourceManager.Update();
		console.update();

		if(window.Update()) break;
		if(audio.Update(camera)) break;
		input.Update();

		if(shouldQuitConsole.isPress()) break;

		if(isMousePressed != input.IsKeyDown(Key::LButton))
		{
			isMousePressed = !isMousePressed;
			if(isMousePressed)
			{
				window.setMouseClipping(true);
				oldMouseX = input.GetMouseXPositionAbsolute();
				oldMouseY = input.GetMouseYPositionAbsolute();
				window.setMouseVisibility(true);
			}
			else
			{
				window.setMouseClipping(false);
				window.SetMousePosition(oldMouseX, oldMouseY);
				window.setMouseVisibility(false);
			}
		}

		if(isMousePressed)
		{
			camera.rotateY(Math::DegreesToRadians(input.GetMouseXPositionRelative()));
			camera.rotateX(Math::DegreesToRadians(input.GetMouseYPositionRelative()));
		}

		float distance = 100.0f * frameSeconds;
		if(cameraSprint.isHeld()) distance *= 6;

		if(cameraForward.isHeld()) camera.moveX(distance);
		if(cameraBackward.isHeld()) camera.moveX(-distance);
		if(cameraRight.isHeld()) camera.moveZ(distance);
		if(cameraLeft.isHeld()) camera.moveZ(-distance);
		if(cameraUp.isHeld()) camera.moveY(distance);
		if(cameraDown.isHeld()) camera.moveY(-distance);
			
		if(window.Render()) break;
	}

	window.Release();
	audio.Release();

	return 0;
}