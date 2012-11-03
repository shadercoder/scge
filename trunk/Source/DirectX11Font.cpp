#include "scge\Graphics\DirectX11\DirectX11Font.h"
#include "scge\Utility.h"

#define min std::min
#define max std::max
#include <gdiplus.h>
#undef max
#undef min

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

DirectX11FontData::DirectX11FontData(ID3D11Device *d3d11Device, FileSystem &fileSystem, const std::string &arguments)
	: FileResourceData(fileSystem)
	, mD3D11Device(d3d11Device)
	, mAA(true)
	, mFontStyle(FontStyle::Regular)
	, mFontSize(11.0f)
{
	StringUtility::foreachTokenSection(arguments, [this](const std::vector<std::string> &arguments, const std::string &line)
	{
		switch(arguments.size())
		{
		case 4: mFontSize = StringUtility::toVariable<float>(arguments[3]);
		case 3: mFontStyle = StringUtility::toVariable<int>(arguments[2]);
		case 2: mAA = StringUtility::toVariable<bool>(arguments[1]);
		case 1: mFileName = arguments[0];
		}
	});
}

std::string DirectX11FontData::getIdentifier() const
{
	std::stringstream ss;
	ss << mFileName;
	ss << " ";
	ss << StringUtility::toString(mAA);
	ss << " ";
	ss << StringUtility::toString(mFontStyle);
	ss << " ";
	ss << StringUtility::toString(mFontSize);

	return ss.str();
}

std::shared_ptr<Resource> DirectX11FontData::createResource() const
{
	return std::shared_ptr<Resource>(new DirectX11Font(this));
}

class GdiPlusUser
{
	ULONG_PTR mToken;
	bool mFailed;

public:
	GdiPlusUser() : mToken(), mFailed()
	{
		Gdiplus::GdiplusStartupInput startUpInput(nullptr, true, true);
		Gdiplus::GdiplusStartupOutput startUpOutput;
		mFailed = (Gdiplus::GdiplusStartup(&mToken, &startUpInput, &startUpOutput) != Gdiplus::Ok);
	}

	~GdiPlusUser()
	{
		Gdiplus::GdiplusShutdown(mToken);
	}

	bool Failed() { return mFailed; }
};

bool DirectX11Font::Load()
{
	return false;
}

bool DirectX11Font::Finalise()
{
	/*Gdiplus::TextRenderingHint hint = mResourceData->mAA ? Gdiplus::TextRenderingHintAntiAliasGridFit : Gdiplus::TextRenderingHintSystemDefault;

	// Init GDI+
	GdiPlusUser GDIPlusToken;
	if(GDIPlusToken.Failed())
		return true;

	std::string fontName = mResourceData->mFileName;

	// Create the font
	Gdiplus::Font font(fontName.c_str(), mResourceData->mFontSize, mResourceData->mFontStyle, Gdiplus::UnitPixel);

	// Check for error during construction
	if(font.GetLastStatus() != Gdiplus::Ok)
		return true;

	// Create a temporary Bitmap and Graphics for figuring out the rough size required
	// for drawing all of the characters
	int size = static_cast<int>(mResourceData->mFontSize * NumChars * 2) + 1;
	Gdiplus::Bitmap sizeBitmap(size, size, PixelFormat32bppARGB);
	if(sizeBitmap.GetLastStatus() != Gdiplus::Ok)
		return true;

	Gdiplus::Graphics sizeGraphics(&sizeBitmap);
	if(sizeGraphics.GetLastStatus() != Gdiplus::Ok)
		return true;

	if(sizeGraphics.SetTextRenderingHint(hint) != Gdiplus::Ok)
		return true;

	mCharHeight = font.GetHeight(&sizeGraphics) * 1.5f;

	wchar_t allChars[NumChars + 1];
	for(wchar_t i = 0; i < NumChars; ++i)
		allChars[i] = i + StartChar;
	allChars[NumChars] = 0;

	Gdiplus::RectF sizeRect;
	if(sizeGraphics.MeasureString(allChars, NumChars, &font, Gdiplus::PointF(0, 0), &sizeRect) != Gdiplus::Ok)
		return true;

	int numRows = static_cast<int>(sizeRect.Width / TexWidth) + 1;        
	mTextureHeight = static_cast<int>(numRows * mCharHeight) + 1;

	// Create a temporary Bitmap and Graphics for drawing the characters one by one
	int tempSize = static_cast<int>(mResourceData->mFontSize * 2);
	Gdiplus::Bitmap drawBitmap(tempSize, tempSize, PixelFormat32bppARGB);
	if(drawBitmap.GetLastStatus() != Gdiplus::Ok)
		return true;

	Gdiplus::Graphics drawGraphics(&drawBitmap);
	if(drawGraphics.GetLastStatus() != Gdiplus::Ok)
		return true;

	if(drawGraphics.SetTextRenderingHint(hint))
		return true;

	// Create a temporary Bitmap + Graphics for creating a full character set
	Gdiplus::Bitmap textBitmap (TexWidth, mTextureHeight, PixelFormat32bppARGB);
	if(textBitmap.GetLastStatus() != Gdiplus::Ok)
		return true;

	Gdiplus::Graphics textGraphics (&textBitmap);
	if(textGraphics.GetLastStatus() != Gdiplus::Ok)
		return true;

	if(textGraphics.Clear(Gdiplus::Color(0, 255, 255, 255)) != Gdiplus::Ok)
		return true;

	if(textGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy))
		return true;

	// Solid brush for text rendering
	Gdiplus::SolidBrush brush (Gdiplus::Color(255, 255, 255, 255));
	if(brush.GetLastStatus() != Gdiplus::Ok)
		return true;

	// Draw all of the characters, and copy them to the full character set               
	WCHAR charString [2];
	charString[1] = 0;        
	int currentX = 0;
	int currentY = 0;
	for(UINT64 i = 0; i < NumChars; ++i)
	{
		charString[0] = static_cast<WCHAR>(i + StartChar);

		// Draw the character
		if(drawGraphics.Clear(Gdiplus::Color(0, 255, 255, 255)) != Gdiplus::Ok)
			return true;

		if(drawGraphics.DrawString(charString, 1, &font, Gdiplus::PointF(0, 0), &brush) != Gdiplus::Ok)
			return true;

		// Figure out the amount of blank space before the character
		int minX = 0;            
		for(int x = 0; x < tempSize; ++x)
		{
			for(int y = 0; y < tempSize; ++y)
			{
				Gdiplus::Color colour;
				if(drawBitmap.GetPixel(x, y, &colour) != Gdiplus::Ok)
					return true;

				if(colour.GetAlpha() > 0)
				{
					minX = x;                        
					x = tempSize;
					break;
				}
			}
		}

		// Figure out the amount of blank space after the character
		int maxX = tempSize - 1;
		for(int x = tempSize - 1; x >= 0; --x)
		{
			for(int y = 0; y < tempSize; ++y)
			{
				Gdiplus::Color colour;
				if(drawBitmap.GetPixel(x, y, &colour) != Gdiplus::Ok)
					return true;

				if(colour.GetAlpha() > 0)
				{
					maxX = x;
					x = -1;
					break;
				}
			}
		}

		int charWidth = maxX - minX + 1;

		// Figure out if we need to move to the next row
		if (currentX + charWidth >= TexWidth)
		{
			currentX = 0;
			currentY += static_cast<int>(mCharHeight) + 1;
		}

		// Fill out the structure describing the character position
		mCharDescs[i].X = static_cast<float>(currentX);
		mCharDescs[i].Y = static_cast<float>(currentY);
		mCharDescs[i].Width = static_cast<float>(charWidth);
		mCharDescs[i].Height = static_cast<float>(mCharHeight);

		// Copy the character over 
		int height = static_cast<int>(mCharHeight + 1);
		if(textGraphics.DrawImage(&drawBitmap, currentX, currentY, minX, 0, charWidth, height, Gdiplus::UnitPixel) != Gdiplus::Ok)
			return true;

		currentX += charWidth + 1;
	}        

	// Figure out the width of a space character
	charString[0] = ' ';
	charString[1] = 0;
	if(drawGraphics.MeasureString(charString, 1, &font, Gdiplus::PointF(0, 0), &sizeRect) != Gdiplus::Ok)
		return true;

	mSpaceWidth = sizeRect.Width;           

	// Lock the bitmap for direct memory access
	Gdiplus::BitmapData bmData;
	Gdiplus::Rect tempTect(0, 0, TexWidth, mTextureHeight);
	if(textBitmap.LockBits(&tempTect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData) != Gdiplus::Ok)
		return true;

	// Create a D3D texture, initialized with the bitmap data
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = TexWidth;
	texDesc.Height = mTextureHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;        
	data.pSysMem = bmData.Scan0;
	data.SysMemPitch = TexWidth * 4;
	data.SysMemSlicePitch = 0;

	mTexture.Release();
	if(FAILED(mResourceData->mD3D11Device->CreateTexture2D(&texDesc, &data, &mTexture)))
		return true;

	if(textBitmap.UnlockBits(&bmData) != Gdiplus::Ok)
		return true;

	// Create the shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MipLevels = 1;
	srDesc.Texture2D.MostDetailedMip = 0;

	mTextureSRV.Release();
	if(FAILED(mResourceData->mD3D11Device->CreateShaderResourceView(mTexture, &srDesc, &mTextureSRV)))
		return true;
	*/
	return false;
}

void DirectX11Font::Release()
{
	mTextureSRV.Release();
	mTexture.Release();
}
