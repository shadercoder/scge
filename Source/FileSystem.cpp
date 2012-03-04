#include "scge\FileSystem.h"

#include "scge\ResourceManager\Resource.h"

#include <filesystem>

FileSystem::FileSystem(const std::string & resourceDirectory)
	: mResourceDirectory(resourceDirectory)
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::Update()
{
}

std::pair<std::string, bool> FileSystem::getFilePath(const std::string &fileName, bool canCreate) const
{
	std::tr2::sys::path filePath(mResourceDirectory);
	filePath /= fileName;

	if(!filePath.parent_path().empty() && !std::tr2::sys::exists(filePath.parent_path()))
	{
		if(!canCreate || !std::tr2::sys::create_directory(filePath.parent_path()))
			return std::make_pair(filePath.relative_path().string(), false);
	}
	
	if(!filePath.relative_path().empty() && !std::tr2::sys::exists(filePath.relative_path()))
		return std::make_pair(filePath.relative_path().string(), false);

	return std::make_pair(filePath.relative_path().string(), true);
}

std::ifstream FileSystem::OpenFileRead(const std::string &fileName, bool binary) const
{
	auto filePath = getFilePath(fileName, false);
	if(!filePath.second)
		return std::ifstream();

	std::ios_base::openmode flags = std::ios_base::in;
	if(binary) flags |= std::ios_base::binary;

	std::ifstream file(filePath.first, flags);
	if(!file)
		return std::ifstream();

	return std::move(file);
}

std::ofstream FileSystem::OpenFileWrite(const std::string &fileName, bool canCreate, bool append, bool binary) const
{
	auto filePath = getFilePath(fileName, canCreate);
	if(!filePath.second)
		return std::ofstream();

	std::ios_base::openmode flags = std::ios_base::out;
	flags |= (append ? std::ios_base::app : std::ios_base::trunc);
	if(binary) flags |= std::ios_base::binary;

	std::ofstream file(filePath.first, flags);
	if(!file)
		return std::ofstream();

	return std::move(file);
}

bool FileSystem::RegisterForFileChanges(Resource &resource, const std::string &file)
{
	return true;
}

void FileSystem::UnregisterForFileChanges(Resource &resource)
{
}