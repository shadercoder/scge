#include <scge\Console.h>
#include <scge\Window.h>
#include <scge\InputDevice.h>
#include <scge\FileSystem.h>
#include <scge\Timer.h>
#include <scge\Warning.h>
#include <scge\Graphics\PointLight.h>
#include <scge\Graphics\Base\ModelResource.h>

void fillLightsGrid(std::vector<PointLight> &lights, Window &window)
{
	lights.clear();
	for(int x = 0; x < 8; ++x)
		for(int y = 0; y < 8; ++y)
			for(int z = 0; z < 8; ++z)
				lights.push_back(PointLight(window.GetGraphicsDevice().getRenderer3D(), Vector3(-1280.0f + (340.0f * x), 40.0f + (340.0f * y), -1280.0f + (340.0f * z)), Vector3(0.5f, 0.5f, 0.5f), 10, 510));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowSmd)
{
	FileSystem fileSystem("../../Resources");
	Console console(fileSystem, false);
	
	ResourceManager resourceManager(console);

	InputDevice input(console);

	Window window("Void", console, input, resourceManager, fileSystem, 1280, 720, true, false);
	//Window window("Void", console, input, resourceManager, fileSystem);
	if(window.Initialise()) return 0;

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

	std::vector<PointLight> lights;

	std::vector<ResourceReference<ModelResource>> models;
#define add_model(x) models.push_back(resourceManager.getResourceReference<ModelResource>("[Model] " x ".smdf"))
	add_model("Lion");
	
	/*add_model("Lionbackground"); add_model("VaseRound"); add_model("Ceiling");
	add_model("ColumnA"); add_model("ColumnB"); add_model("ColumnC"); add_model("Floor"); add_model("FabricGreen");
	add_model("FabricBlue"); add_model("FabricRed"); add_model("CurtainBlue"); add_model("CurtainRed");
	add_model("CurtainGreen"); add_model("VaseHanging"); add_model("Vase"); add_model("Bricks");
	add_model("Arch"); add_model("Details"); add_model("Roof");	add_model("VasePlant");
	add_model("Chain"); add_model("Thorn");add_model("Vase");*/

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

		for(auto & model : models)
			model->Render();

		if(input.IsKeyPress(Key::Num1))
			fillLightsGrid(lights, window);

		Vector3 lightChange;
		if(input.IsKeyDown(Key::Up)) lightChange.y += distance;
		if(input.IsKeyDown(Key::Down)) lightChange.y -= distance;
		if(input.IsKeyDown(Key::Left)) lightChange.x += distance;
		if(input.IsKeyDown(Key::Right)) lightChange.x -= distance;

		if(lightChange != Vector3::Zero)
		{
			for(PointLight &light : lights)
				light.setPosition(light.getPosition() + lightChange);
		}
			
		if(window.Render()) break;
	}

	return 0;
}