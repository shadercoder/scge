#include "scge\AudioDevice.h"

/*#include "scge\Utility.h"

#include <fmod_errors.h>

AudioDevice::AudioDevice(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem)
	: mConsole(console)
	, mResourceManager(resourceManager)
	, mFileSystem(fileSystem)
	, mInitialised(false)
	, mSystem(nullptr)
	, mSoundFactory(resourceManager, "Sound")
	, mMasterVolume(console, "Audio.MasterVolume", "Master Volume for the game.", 0.0f, 1.0f, 1.0f)
{
}

AudioDevice::~AudioDevice()
{
	if(mInitialised)
		Release();
}

bool AudioDevice::Initialise()
{
	if(CheckError(FMOD::System_Create(&mSystem)))
		return true;

	unsigned int fmodVersion;
	if(CheckError(mSystem->getVersion(&fmodVersion)))
		return true;

	if(fmodVersion < FMOD_VERSION)
	{
		mConsole.printError("Using wrong version of FMOD, requires " + StringUtility::toString(FMOD_VERSION) + " but is using " + StringUtility::toString(fmodVersion));
		return true;
	}

	int numDrivers;
	if(CheckError(mSystem->getNumDrivers(&numDrivers)))
		return true;

	if(numDrivers == 0)
	{
		if(CheckError(mSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND)))
			return true;
	}
	else
	{
		FMOD_SPEAKERMODE speakerMode;
		FMOD_CAPS caps;

		if(CheckError(mSystem->getDriverCaps(0, &caps, 0, &speakerMode)))
			return true;

		if(CheckError(mSystem->setSpeakerMode(speakerMode)))
			return true;

		if((caps & FMOD_CAPS_HARDWARE_EMULATED) && CheckError(mSystem->setDSPBufferSize(1024, 10)))
			return true;

		char name[256];
		if(CheckError(mSystem->getDriverInfo(0, name, 256, 0)))
			return true;

		if(strstr(name, "SigmaTel"))
		{
			if(CheckError(mSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR)))
				return true;
		}
	}

	auto result = mSystem->init(100, FMOD_INIT_NORMAL, 0);
	if(result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		if(CheckError(mSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO)))
			return true;

		if(CheckError(mSystem->init(100, FMOD_INIT_NORMAL, 0)))
			return true;
	}
	else if(CheckError(result))
		return true;

	if(CheckError(mSystem->set3DSettings(1.0f, 1.0f, 1.0f)))
		return true;

	FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_BATHROOM;
	if(CheckError(mSystem->getReverbProperties(&prop)))
		return true;

	if(CheckError(mSystem->createSoundGroup("MasterGroup", &mMasterSoundGroup)))
		return true;

	mMasterVolume.updateValue();
	if(CheckError(mMasterSoundGroup->setVolume(mMasterVolume)))
		return true;

	if(mSoundFactory.Initialise([this](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new SoundResourceData(mFileSystem, *mSystem, *mMasterSoundGroup, arguments));
	}))
		return true;

	mInitialised = true;

	return false;
}

void AudioDevice::Release()
{
	mSoundFactory.Release();

	if(mMasterSoundGroup)
	{
		CheckError(mMasterSoundGroup->release());
		mMasterSoundGroup = nullptr;
	}

	if(mSystem)
	{
		CheckError(mSystem->close());
		CheckError(mSystem->release());
		mSystem = nullptr;
	}

	mInitialised = false;
}

bool AudioDevice::Update(Camera &camera)
{
	if(!mInitialised)
		return true;

	if(mMasterVolume.hasValueChanged())
	{
		mMasterVolume.updateValue();
		if(CheckError(mMasterSoundGroup->setVolume(mMasterVolume)))
			return true;
	}
	
	const Vector3 &cameraPosition = camera.getPosition();
	FMOD_VECTOR position = {0};
	position.x = cameraPosition.x; position.y = cameraPosition.y; position.z = cameraPosition.z;

	const Vector3 &cameraForward = camera.getLook();
	FMOD_VECTOR forward = {0};
	forward.x = cameraForward.x; forward.y = cameraForward.y; forward.z = cameraForward.z;

	const Vector3 &cameraUp = camera.getUp();
	FMOD_VECTOR up = {0};
	up.x = -cameraUp.x; up.y = -cameraUp.y; up.z = -cameraUp.z;

	FMOD_VECTOR velocity = {0};

	if(CheckError(mSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up)))
		return true;

	if(CheckError(mSystem->update()))
		return true;

	return false;
}

bool AudioDevice::CheckError(FMOD_RESULT result)
{
	if(result != FMOD_OK)
	{
		mConsole.printError(FMOD_ErrorString(result));
		return true;
	}

	return false;
}
*/