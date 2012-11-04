#include "scge\Graphics\DirectX11\DirectX11VertexShader.h"

#include "scge\Utility.h"

DirectX11VertexShaderData::DirectX11VertexShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments)
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
	mFunctionName = (tokens.size() > 1 ? tokens[1] : "VSMain");
	mLayoutName = (tokens.size() > 2 ? tokens[2] : "ModelLayout");
}

std::string DirectX11VertexShaderData::getIdentifier() const
{
	return mFileName + " " + mFunctionName;
}

std::unique_ptr<Resource> DirectX11VertexShaderData::createResource() const
{
	return std::unique_ptr<Resource>(new DirectX11VertexShader(this));
}

bool DirectX11VertexShader::Load()
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
	if(FAILED(D3DX11CompileFromFile(fileName.c_str(), mResourceData->mShaderDefines, nullptr, mResourceData->mFunctionName.c_str(), "vs_5_0", shaderFlags, 0, nullptr, mShaderBuffer.getModifieablePointer(), errorMessage.getModifieablePointer(), nullptr)))
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to Compile Vertex Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName));

		if(errorMessage)
			mResourceData->mConsole.threadSafePrintError(static_cast<const char *>(errorMessage->GetBufferPointer()));

		return true;
	}

	return mResourceData->mMultiThreadLoad && finaliseLoad();
}

bool DirectX11VertexShader::Finalise()
{
	if(!mResourceData->mMultiThreadLoad && finaliseLoad())
		return true;

	registerForChanges();

	return false;
}

bool DirectX11VertexShader::finaliseLoad()
{
	if(FAILED(mResourceData->mD3D11Device->CreateVertexShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), nullptr, mVertexShader.getModifieablePointer())))
	{
		const auto &error = StringUtility::format("Failed to Create Vertex Shader : %, function : %", mResourceData->mFileName, mResourceData->mFunctionName);
		if(mResourceData->mMultiThreadLoad)
			mResourceData->mConsole.threadSafePrintError(error);
		else
			mResourceData->mConsole.printError(error);

		return true;
	}

	unsigned int numElements = 0;
	D3D11_INPUT_ELEMENT_DESC const *elements = nullptr;

	static const D3D11_INPUT_ELEMENT_DESC modelLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if(mResourceData->mLayoutName == "ModelLayout")
	{
		elements = modelLayout;
		numElements = ARRAYSIZE(modelLayout);
	}
	else if(mResourceData->mLayoutName == "QuadLayout")
	{
		elements = quadLayout;
		numElements = ARRAYSIZE(quadLayout);
	}

	if(!elements)
	{
		const auto &error = StringUtility::format("Failed to Create Vertex Shader : %, function : %, unknown layout : %", mResourceData->mFileName, mResourceData->mFunctionName, mResourceData->mLayoutName);
		if(mResourceData->mMultiThreadLoad)
			mResourceData->mConsole.threadSafePrintError(error);
		else
			mResourceData->mConsole.printError(error);

		return true;
	}

	if(FAILED(mResourceData->mD3D11Device->CreateInputLayout(elements, numElements, mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), mVertexLayout.getModifieablePointer())))
	{
		const auto &error = StringUtility::format("Failed to Create Input Layout for Vertex Shader : %, function : %, layout : %", mResourceData->mFileName, mResourceData->mFunctionName, mResourceData->mLayoutName);
		if(mResourceData->mMultiThreadLoad)
			mResourceData->mConsole.threadSafePrintError(error);
		else
			mResourceData->mConsole.printError(error);

		return true;
	}

	mShaderBuffer.Release();

	return false;
}

void DirectX11VertexShader::Release()
{
	unregisterForChanges();
	mVertexLayout.Release();
	mVertexShader.Release();
	mShaderBuffer.Release();
}