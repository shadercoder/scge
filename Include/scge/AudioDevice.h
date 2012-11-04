#ifndef AudioDevice_h__
#define AudioDevice_h__

/*#include "scge\Console.h"
#include "scge\ResourceManager\ResourceFactory.h"
#include "scge\AudioDevice\SoundResource.h"
#include "scge\Math\Camera.h"

#include <fmod.hpp>

#pragma comment(lib, "fmodex_vc.lib")

class AudioDevice
{
public:
	AudioDevice(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem);
	~AudioDevice();

	bool Initialise();
	void Release();

	bool Update(Camera &camera);

	void setMasterVolume(float newValue) { mMasterVolume.setValue(newValue); }
	float getMasterVolume() const { return mMasterVolume.getValue(); }

private:
	Console& mConsole;
	ResourceManager &mResourceManager;
	FileSystem &mFileSystem;

	bool mInitialised;

	ConsoleVariable<float, true> mMasterVolume;

	bool CheckError(FMOD_RESULT result);

	FMOD::System* mSystem;
	FMOD::SoundGroup* mMasterSoundGroup;

	ResourceFactory mSoundFactory;

	AudioDevice(const AudioDevice &other);
	AudioDevice &operator=(const AudioDevice &other);
};*/

#endif // AudioDevice_h__