#ifndef __FileSystem_h__
#define __FileSystem_h__

#include <map>
#include <fstream>
#include <memory>

#include <boost\bimap.hpp>
#include <boost\bimap\unordered_set_of.hpp>

class Resource;
class FileSystem
{
public:
	FileSystem(std::string resourceDirectory = "Resources");
	~FileSystem();

	void Update();

	std::pair<std::string, bool> getFilePath(const std::string &fileName, bool canCreate = false) const;

	std::ifstream OpenFileRead(const std::string &fileName, bool binary = false) const;
	std::ofstream OpenFileWrite(const std::string &fileName, bool canCreate = false, bool append = false, bool binary = false) const;

	bool RegisterForFileChanges(Resource &resource, const std::string &file);
	void UnregisterForFileChanges(Resource &resource);

private:
	std::string mResourceDirectory;

	typedef boost::bimap<boost::bimaps::unordered_set_of<std::string>, boost::bimaps::set_of<Resource*>> ResourceFileAssociations;
	ResourceFileAssociations mResourceFileAssociations;

	class DirectoryWatcher;
	std::unique_ptr<DirectoryWatcher> mDirectoryWatcher;

	FileSystem(const FileSystem &other);
	FileSystem &operator=(const FileSystem &other);
};

#endif // __FileSystem_h__