#include "scge\Graphics\Resources\MaterialResource.h"

#include "scge\Config.h"

#include <algorithm>
#include <fstream>
#include <string>

MaterialResourceData::MaterialResourceData(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, const std::string &arguments)
	: FileResourceData(fileSystem)
	, mConsole(console)
	, mResourceManager(resourceManager)
{
	mFileName = arguments;
}

std::string MaterialResourceData::getIdentifier() const
{
	return mFileName;
}

template <typename T>
inline bool read(std::ifstream &file, T &result)
{
	file.read((char *)&result, sizeof(T));
	return file.fail();
}

inline bool read(std::ifstream &file, std::string &result)
{
	uint32 stringLength = 0;
	if(read(file, stringLength)) return true;

	result.resize(stringLength);

	file.read(&result[0], sizeof(char)*stringLength);
	return file.fail();
}

bool MaterialResource::LoadMaterial(const MaterialResourceData &resourceData)
{
	std::ifstream file = resourceData.mFileSystem.OpenFileRead(resourceData.mFileName);
	if(!file || !file.is_open())
		return true;

	uint8 fileVersion = 0;
	if(read(file, fileVersion) || fileVersion != 1)
		return true;

	if(read(file, mDiffuseTextureName))
		return true;

	if(read(file, mNormalTextureName))
		return true;

	if(read(file, mSpecularTextureName))
		return true;

	uint8 materialType;
	if(read(file, materialType))
		return true;

	mMaterialType = static_cast<MaterialType>(materialType);

	if(read(file, mIsAlphaTestMaterial))
		return true;

	return false;
}