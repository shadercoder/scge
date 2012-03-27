#include "scge\FileSystem.h"

#include "scge\ResourceManager\Resource.h"

#include "scge\Config.h"
#include <windows.h>

#include <thread>
#include <mutex>
#include <queue>
#include <array>
#include <filesystem>

class FileSystem::DirectoryWatcher
{
public:
	DirectoryWatcher(const std::string &resourceDirectory)
		: mThread()
		, mOverlapped()
		, mTerminate(false)
		, mDirectoryHandle(nullptr)
	{
		memset(&mOverlapped, 0, sizeof(OVERLAPPED));
		mOverlapped.hEvent = this;

		mThread = std::thread(std::bind(&DirectoryWatcher::watchFile, this, resourceDirectory));
	}

	~DirectoryWatcher()
	{
		if(mDirectoryHandle)
		{
			::CancelIo(mDirectoryHandle);
			::CloseHandle(mDirectoryHandle);
			mDirectoryHandle = nullptr;
		}

		mTerminate = true;
		mThread.join();
	}

	void watchFile(const std::string &resourceDirectory)
	{
		std::wstring wResourceDirectory;
		wResourceDirectory.assign(resourceDirectory.begin(), resourceDirectory.end());

		mDirectoryHandle = ::CreateFile(wResourceDirectory.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);

		if(mDirectoryHandle == INVALID_HANDLE_VALUE)
			return;

		if(!BeginRead())
		{
			while(!mTerminate)
				::WaitForMultipleObjectsEx(1, &mDirectoryHandle, true, INFINITE, true);
		}
	}

	bool BeginRead()
	{
		DWORD numBytes = 0;

		return ::ReadDirectoryChangesW(mDirectoryHandle, mBuffer.data(), mBuffer.size(), true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME, &numBytes, &mOverlapped, &notificationCompletion) == 0;
	}

	void readChanges()
	{
		char* data = mBuffer.data();
		while(data)
		{
			FILE_NOTIFY_INFORMATION& fni = *reinterpret_cast<FILE_NOTIFY_INFORMATION*>(data);

			std::wstring filenameW(fni.FileName, fni.FileNameLength / sizeof(wchar_t));

			std::string filename;
			filename.assign(filenameW.begin(), filenameW.end());

			{
				std::unique_lock<std::mutex> lock(mChangedFilesMutex);
				mChangedFiles.push(filename);
			}

			if(!fni.NextEntryOffset)
				return;

			data += fni.NextEntryOffset;
		}
	}

	void foreachFile(const std::function<void(const std::string &)> function)
	{
		std::unique_lock<std::mutex> lock(mChangedFilesMutex);

		while(!mChangedFiles.empty())
		{
			function(mChangedFiles.front());
			mChangedFiles.pop();
		}
	}

	static VOID CALLBACK notificationCompletion(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
	{
		if(dwErrorCode == ERROR_OPERATION_ABORTED)
			return;

		DirectoryWatcher* block = static_cast<DirectoryWatcher*>(lpOverlapped->hEvent);

		if(block->BeginRead())
			return;

		block->readChanges();
	}

private:
	bool mTerminate;
	std::thread mThread;

	std::mutex mChangedFilesMutex;
	std::queue<std::string> mChangedFiles;

	HANDLE mDirectoryHandle;

	std::array<char, 16384> mBuffer;
	OVERLAPPED mOverlapped;

	DirectoryWatcher(const DirectoryWatcher &other);
	DirectoryWatcher &operator=(const DirectoryWatcher &other);
};

FileSystem::FileSystem(std::string resourceDirectory)
	: mResourceDirectory(resourceDirectory)
	, mDirectoryWatcher(new DirectoryWatcher(mResourceDirectory))
{
}

FileSystem::~FileSystem()
{
}

void FileSystem::Update()
{
	mDirectoryWatcher->foreachFile([this](const std::string &changedFile)
	{
		auto it = mResourceFileAssociations.left.find(changedFile);
		if(it != mResourceFileAssociations.left.end())
		{
			Resource *resource = it->second;
			if(resource->getResourceStatus() != ResourceStatus::Loading)
			{
				resource->Release();
				resource->setResourceStatus(ResourceStatus::Unloaded);
			}
		}
	});
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
	if(!mResourceFileAssociations.insert(ResourceFileAssociations::value_type(file, &resource)).second)
		return true;

	return false;
}

void FileSystem::UnregisterForFileChanges(Resource &resource)
{
	mResourceFileAssociations.right.erase(&resource);
}