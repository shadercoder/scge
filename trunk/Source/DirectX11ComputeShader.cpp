#include "scge\Graphics\DirectX11\DirectX11ComputeShader.h"

#include "scge\Utility.h"

DirectX11ComputeShaderData::DirectX11ComputeShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments)
	: FileResourceData(fileSystem)
	, mConsole(console)
	, mD3D11Device(pd3dDevice)
	, mShaderDefines(shaderDefines)
	, mMultiThreadLoad(multiThreadLoad)
{
	const std::vector<std::string> &tokens = StringUtility::getTokens(arguments);
	if(tokens.empty())
		return;

	mFileName = tokens[0];
	mFunctionName = (tokens.size() > 1 ? tokens[1] : "CSMain");
}

std::string DirectX11ComputeShaderData::getIdentifier() const
{
	return mFileName + " " + mFunctionName;
}

std::shared_ptr<Resource> DirectX11ComputeShaderData::createResource() const
{
	return std::shared_ptr<Resource>(new DirectX11ComputeShader(this));
}

bool DirectX11ComputeShader::Load()
{
	auto file = mResourceData->mFileSystem.getFilePath(mResourceData->mFileName);
	if(!file.second)
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to locate file %", mResourceData->mFileName));
		return true;
	}

	UINT shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if defined(SCGE_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
#endif

	std::wstring fileName;
	fileName.assign(file.first.begin(), file.first.end());

	ComPtr<ID3D10Blob> errorMessage;
	if(FAILED(D3DX11CompileFromFile(fileName.c_str(), mResourceData->mShaderDefines, nullptr, mResourceData->mFunctionName.c_str(), "cs_5_0", shaderFlags, 0, nullptr, &mShaderBuffer, &errorMessage, nullptr)))
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to compile Compute Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName));

		if(errorMessage)
			mResourceData->mConsole.threadSafePrintError(static_cast<const char *>(errorMessage->GetBufferPointer()));

		return true;
	}

	return mResourceData->mMultiThreadLoad && finaliseLoad();
}

bool DirectX11ComputeShader::Finalise()
{
	if(!mResourceData->mMultiThreadLoad && finaliseLoad())
		return true;

	registerForChanges();

	return false;
}

bool DirectX11ComputeShader::finaliseLoad()
{
	if(FAILED(mResourceData->mD3D11Device->CreateComputeShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), nullptr, &mComputeShader)))
	{
		const auto &error = StringUtility::format("Failed to Create Compute Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName);
		if(mResourceData->mMultiThreadLoad)
			mResourceData->mConsole.threadSafePrintError(error);
		else
			mResourceData->mConsole.printError(error);

		return true;
	}

	mShaderBuffer.Release();

	return false;
}

void DirectX11ComputeShader::Release()
{
	unregisterForChanges();
	mComputeShader.Release();
	mShaderBuffer.Release();
}