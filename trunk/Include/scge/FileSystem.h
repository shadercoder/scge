#ifndef __FileSystem_h__
#define __FileSystem_h__

#include <map>
#include <fstream>

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
	std::map<Resource*, std::pair<std::string, std::string>> mResourceToFile;

	std::string mResourceDirectory;

	FileSystem(const FileSystem &other);
	FileSystem &operator=(const FileSystem &other);
};

#endif // __FileSystem_h__