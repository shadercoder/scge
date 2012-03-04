#ifndef SoundResource_h__
#define SoundResource_h__

#include "scge\FileSystem\FileResource.h"

#include "scge\Math\Vector3.h"

#include <fmod.hpp>

class SoundResourceData : public FileResourceData
{
public:
	SoundResourceData(FileSystem &fileSystem, FMOD::System &system, FMOD::SoundGroup &soundGroup, const std::string &arguments);

	virtual std::string getIdentifier() const;
	virtual std::string getFactory() const { return "Sound"; }

	virtual std::shared_ptr<Resource> createResource() const;

	FMOD::System &mSystem;
	FMOD::SoundGroup &mSoundGroup;
};

class SoundResource : public ResourceBase<SoundResourceData, FileResource>
{
public:
	SoundResource(const SoundResourceData *data);

	virtual bool Load();
	virtual bool Finalise();
	virtual void Release();

	bool playSound(FMOD::Channel** channel) const
	{
		return mResourceData->mSystem.playSound(channel ? FMOD_CHANNEL_REUSE : FMOD_CHANNEL_FREE, mSound, true, channel) != FMOD_OK;
	}

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *soundResource)
			: FileResource::Interface<I>(soundResource)
			, mChannel(nullptr)
			, mPosition()
			, mVelocity()
			, mMinDistance(400.0f)
			, mMaxDistance(1600.0f)
		{}

		void playSound()
		{
			if(getResource().playSound(&mChannel))
				return;

			mChannel->set3DAttributes(&mPosition, &mVelocity);
			mChannel->set3DMinMaxDistance(mMinDistance, mMaxDistance);
			mChannel->setPaused(false);
		}

		void stopSound()
		{
			if(mChannel)
			{
				bool isPlaying = false;
				if(mChannel->isPlaying(&isPlaying) != FMOD_OK)
					return;

				if(isPlaying) mChannel->stop();
			}
		}

		Vector3 setPosition() const { return Vector3(mPosition.x, mPosition.y, mPosition.z); }
		void setPosition(Vector3 position)
		{
			mPosition.x = position.x;
			mPosition.y = position.y;
			mPosition.z = position.z;

			if(mChannel) mChannel->set3DAttributes(&mPosition, &mVelocity);
		}

		Vector3 getVelocity() const { return Vector3(mVelocity.x, mVelocity.y, mVelocity.z); }
		void setVelocity(Vector3 velocity)
		{
			mVelocity.x = velocity.x;
			mVelocity.y = velocity.y;
			mVelocity.z = velocity.z;

			if(mChannel) mChannel->set3DAttributes(&mPosition, &mVelocity);
		}

		float getMinDistance() const { return mMinDistance; }
		float getMaxDistance() const { return mMaxDistance; }
		void setMinMaxDistance(float min, float max)
		{
			mMinDistance = min;
			mMaxDistance = max;

			if(mChannel) mChannel->set3DMinMaxDistance(mMinDistance, mMaxDistance);
		}

	private:
		FMOD::Channel* mChannel;

		FMOD_VECTOR mPosition;
		FMOD_VECTOR mVelocity;

		float mMinDistance;
		float mMaxDistance;
	};

private:
	FMOD::Sound* mSound;
};

#endif // SoundResource_h__