#include "scge\AudioDevice\SoundResource.h"

/*#include "scge\Utility.h"

SoundResourceData::SoundResourceData(FileSystem &fileSystem, FMOD::System &system, FMOD::SoundGroup &soundGroup, const std::string &arguments)
	: FileResourceData(fileSystem)
	, mSystem(system)
	, mSoundGroup(soundGroup)
{
	mFileName = arguments;
}

std::string SoundResourceData::getIdentifier() const
{
	return mFileName;
}

std::unique_ptr<Resource> SoundResourceData::createResource() const
{
	return std::unique_ptr<Resource>(new SoundResource(this));
}

SoundResource::SoundResource(const SoundResourceData *data)
	: ResourceBase(data)
	, mSound(nullptr)
{
}

bool SoundResource::Load()
{
	return false;
}

bool SoundResource::Finalise()
{
	auto file = mResourceData->mFileSystem.getFilePath(mResourceData->mFileName);
	if(!file.second)
		return true;

	if(mResourceData->mSystem.createSound(file.first.c_str(), FMOD_3D | FMOD_LOOP_OFF, 0, &mSound) != FMOD_OK)
		return true;

	if(mSound->setSoundGroup(&mResourceData->mSoundGroup) != FMOD_OK)
		return true;

	return false;
}

void SoundResource::Release()
{
	if(mSound)
	{
		mSound->release();
		mSound = nullptr;
	}
}*/