#include "scge\Graphics\DirectX11\DirectX11PixelShader.h"

#include "scge\Utility.h"

DirectX11PixelShaderData::DirectX11PixelShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments)
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
	mFunctionName = (tokens.size() > 1 ? tokens[1] : "PSMain");
}

std::string DirectX11PixelShaderData::getIdentifier() const
{
	return mFileName + " " + mFunctionName;
}

std::unique_ptr<Resource> DirectX11PixelShaderData::createResource() const
{
	return std::unique_ptr<Resource>(new DirectX11PixelShader(this));
}

bool DirectX11PixelShader::Load()
{
	auto file = mResourceData->mFileSystem.getFilePath(mResourceData->mFileName);
	if(!file.second)
	{
		mResourceData->mConsole.threadSafePrintError("Failed to locate file "+ mResourceData->mFileName);
		return true;
	}

	UINT shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#if defined(SCGE_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
#endif

	std::wstring fileName;
	fileName.assign(file.first.begin(), file.first.end());

	ComPtr<ID3D10Blob> errorMessage;
	if(FAILED(D3DX11CompileFromFile(fileName.c_str(), mResourceData->mShaderDefines, nullptr, mResourceData->mFunctionName.c_str(), "ps_5_0", shaderFlags, 0, nullptr, mShaderBuffer.getModifieablePointer(), errorMessage.getModifieablePointer(), nullptr)))
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to compile Pixel Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName));

		if(errorMessage)
			mResourceData->mConsole.threadSafePrintError(static_cast<const char *>(errorMessage->GetBufferPointer()));

		return true;
	}
	
	return mResourceData->mMultiThreadLoad && finaliseLoad();
}

bool DirectX11PixelShader::Finalise()
{
	if(!mResourceData->mMultiThreadLoad && finaliseLoad())
	{
		const auto &error = StringUtility::format("Failed to Create Pixel Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName);
		if(mResourceData->mMultiThreadLoad)
			mResourceData->mConsole.threadSafePrintError(error);
		else
			mResourceData->mConsole.printError(error);

		return true;
	}

	registerForChanges();

	return false;
}

bool DirectX11PixelShader::finaliseLoad()
{
	if(FAILED(mResourceData->mD3D11Device->CreatePixelShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), nullptr, mPixelShader.getModifieablePointer())))
		return true;

	mShaderBuffer.Release();

	return false;
}

void DirectX11PixelShader::Release()
{
	unregisterForChanges();
	mPixelShader.Release();
	mShaderBuffer.Release();
}