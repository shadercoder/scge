#ifndef __DirectX11Font_h__
#define __DirectX11Font_h__

#include "scge\FileSystem\FileResource.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <atlbase.h>

#pragma comment(lib, "gdiplus.lib")

namespace FontStyle
{
	enum Type
	{
		Regular = 0,
		Bold = 1 << 0,
		Italic = 1 << 1,
		BoldItalic = Bold | Italic,
		Underline = 1 << 2,
		Strikeout = 1 << 3
	};
}

struct CharDesc
{
	float X;        
	float Y;
	float Width;
	float Height;
};

class DirectX11FontData : public FileResourceData
{
public:
	DirectX11FontData(ID3D11Device *d3d11Device, FileSystem &fileSystem, const std::string &arguments);

	virtual std::string getIdentifier() const;
	virtual std::string getFactory() const { return "Font"; }

	virtual std::shared_ptr<Resource> createResource() const;

	ID3D11Device *mD3D11Device;

	bool mAA;
	unsigned int mFontStyle;
	float mFontSize;
};

class DirectX11Font : public ResourceBase<DirectX11FontData, FileResource>
{
public:
	DirectX11Font(const DirectX11FontData *data) : ResourceBase(data) {}

	virtual bool Load();
	virtual bool Finalise();
	virtual void Release();

	static const wchar_t StartChar = '!';
	static const wchar_t EndChar = 127;
	static const unsigned int NumChars = EndChar - StartChar;
	static const unsigned int TexWidth = 1024;

	ID3D11ShaderResourceView* GetShaderResourceView() const { return mTextureSRV; }

	const CharDesc& GetCharDescriptor(wchar_t character) const { return mCharDescs[character - StartChar]; }

	float Size() const { return mResourceData->mFontSize; }

	unsigned int TextureWidth() const { return TexWidth; }
	unsigned int TextureHeight() const { return mTextureHeight; }

	float SpaceWidth() const { return mSpaceWidth; }
	float CharHeight() const { return mCharHeight; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *fileResource) : FileResource::Interface<I>(fileResource) {}
		virtual ~Interface() {}

		ID3D11ShaderResourceView* GetShaderResourceView() const { return getResource().GetShaderResourceView(); }

		const CharDesc& GetCharDescriptor(wchar_t character) const { return getResource().GetCharDescriptor(character); }

		float Size() const { return getResource().Size(); }

		unsigned int TextureWidth() const { return getResource().TextureWidth(); }
		unsigned int TextureHeight() const { return getResource().TextureHeight(); }

		float SpaceWidth() const { return getResource().SpaceWidth(); }
		float CharHeight() const { return getResource(). CharHeight(); }
	};

private:
	CComPtr<ID3D11Texture2D> mTexture;
	CComPtr<ID3D11ShaderResourceView> mTextureSRV;

	CharDesc mCharDescs[NumChars];
	unsigned int mTextureHeight;

	float mSpaceWidth;
	float mCharHeight;
};

#endif // __DirectX11Font_h__
