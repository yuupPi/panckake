#ifndef TEXT_RENDERER
#define TEXT_RENDERER
#define ASCII_LIMIT 1500
#define FONT_LIMIT 10
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

#include "XML/tinyxml.h"
#pragma comment (lib, "XML/tinyxml.lib")

#include <algorithm> 
#include <string>
#include <utility>
#include "dx11resources.h"
#include "indices.h"

struct TextVertex
{
	float x, y;          
	float width, height; 
	unsigned id;         
	unsigned font;
	float opacity;
};

struct CharParam
{
	float x, y;          
	float width, height; 
};

struct ColorParam
{
	float color[4];
};

struct Text
{
	std::wstring text;
	float x, y;
	float width, height;
	float scale;
	unsigned font;
	Text() : 
	width(FLT_MAX),
	height(FLT_MAX),
	font(0){}
};

struct FontTable
{
	std::vector<int>   id;
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> width;
	std::vector<float> height;
	std::vector<float> xoffset;
	std::vector<float> yoffset;
	std::vector<float> xadvance;
	std::vector<int>   acsiiToIndex;
	float              scaleW;
	float              scaleH;
	float              lineHeight; 
};

class sgTextRenderer
{
	std::vector<Text>  m_text;
	std::vector<Text>  m_textPerFrame;
	          unsigned m_maxLength;
			  unsigned m_numToRender;
			  unsigned m_numToRenderPerFrame;
				 float m_scale;
				  bool m_bRenderText;
				  bool m_bRenderTextPerFrame;
				  bool m_bIsChanged;
				  bool m_bIsChangedPerFrame;
    	 ID3D11Buffer* m_pVertexBuffer;
	     ID3D11Buffer* m_pPerFrameVertexBuffer;
		 ID3D11Buffer* m_PSColorBuffer;
	     ID3D11Device* m_pD3DDevice;
		DX11Resources* m_pDXResources;
  ID3D11DeviceContext* m_pDeviceContext;

			  unsigned m_numFonts;
std::vector<FontTable> m_fonts;
 std::vector<unsigned> m_fontOffsets;
 std::vector<ColorParam> m_fontColors;
 std::vector<std::pair<char*, char*>> m_fontFileNames;

bool ReadTable(char* pFileName)
{
	TiXmlDocument doc(pFileName);
	if(!doc.LoadFile())
		return 0;

	TiXmlElement *pFont, *pChars, *pChar, *pCommon;
	pFont = doc.FirstChildElement( "font" );
	if (pFont)
	{
		unsigned index = m_fonts.size();
		m_fonts.resize(index + 1);

		pCommon = pFont->FirstChildElement("common");
		m_fonts[index].scaleW = atoi(pCommon->Attribute("scaleW"));
		m_fonts[index].scaleH = atoi(pCommon->Attribute("scaleH"));
		m_fonts[index].lineHeight = atoi(pCommon->Attribute("lineHeight"));
		pChars = pFont->FirstChildElement("chars");
		if (pChars)
			pChar = pChars->FirstChildElement("char");

        while ( pChar )
        {
			m_fonts[index].id.push_back(atoi(pChar->Attribute("id")));
			m_fonts[index].x.push_back(atoi(pChar->Attribute("x")));
			m_fonts[index].y.push_back(atoi(pChar->Attribute("y")));
			m_fonts[index].xoffset.push_back(atoi(pChar->Attribute("xoffset")));
			m_fonts[index].yoffset.push_back(atoi(pChar->Attribute("yoffset")));
			m_fonts[index].xadvance.push_back(atoi(pChar->Attribute("xadvance")));
			m_fonts[index].width.push_back(atoi(pChar->Attribute("width")));
			m_fonts[index].height.push_back(atoi(pChar->Attribute("height")));

            pChar = pChar->NextSiblingElement( "char" );
        }
	
		int size = *std::max_element(m_fonts[index].id.begin(), m_fonts[index].id.end());
		int limit = MIN(size + 1, ASCII_LIMIT);
		m_fonts[index].acsiiToIndex.assign(limit, -1);
		for(int i=0; i < m_fonts[index].id.size(); ++i)
        if (m_fonts[index].id[i] < m_fonts[index].acsiiToIndex.size())
			m_fonts[index].acsiiToIndex[m_fonts[index].id[i]] = i;

		m_numFonts++;
		return true;
	}

	return 0;
}

void UpdateBuffer(const std::vector<Text> &textGroup, ID3D11Buffer* pBuffer, unsigned &num)
{
	if (!pBuffer)
		return;

	unsigned count, index, font, k = 0;
	float x, y, scale;
	D3D11_MAPPED_SUBRESOURCE mapResource;
	m_pDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource ); 
	TextVertex* buffer = (TextVertex*)mapResource.pData;

   	for(int i=0; i < textGroup.size(); ++i)
    {
		count = textGroup[i].text.length();
		if (count != 0)
		{
			font = MIN(textGroup[i].font, m_numFonts - 1);
			x = MAX(textGroup[i].x, 0.f);
			y = MAX(textGroup[i].y, 0.f);
			scale = MAX(0.1f, MIN(textGroup[i].scale, 10.f)) * m_scale;

			for(int j=0; j < count; ++j)
			{
				if (k >= m_maxLength)
				{
					i = textGroup.size();
					break;
				}
				
				index = acsiiToIndex(font, textGroup[i].text[j]);
				if (index == -1)
					continue;

				if (textGroup[i].text[j] != 32 && (x > 1280 - m_fonts[font].xadvance[index] || x + m_fonts[font].xadvance[index] - textGroup[i].x > textGroup[i].width))
				{
					x = textGroup[i].x;
					y += m_fonts[font].lineHeight * scale;

					if (y - textGroup[i].y > textGroup[i].height)
					    break;
				}

				if (textGroup[i].text[j] == 32 && x != textGroup[i].x)
				{
					x += m_fonts[font].xadvance[index] * scale;
					continue;
				}

                x += m_fonts[font].xoffset[index] * scale;
    			buffer[k].id = index + m_fontOffsets[font];
				buffer[k].x = convertPixelsToClipSpace(1280, x);
				buffer[k].y = -convertPixelsToClipSpace(1024, y + m_fonts[font].yoffset[index] * scale);
				buffer[k].width = convertPixelsToClipSpaceDistance(1280, m_fonts[font].width[index] * scale);
				buffer[k].height = convertPixelsToClipSpaceDistance(1024, m_fonts[font].height[index] * scale);
				buffer[k].opacity = 1.f;
				buffer[k].font = font;
				x += m_fonts[font].xadvance[index] * scale;
				k++;
			}
		}
    }
    num = k;
    m_pDeviceContext->Unmap(pBuffer, 0);
}

void UpdateTextBuffer()
{
	if (!m_bIsChanged)
		return;

	UpdateBuffer(m_text, m_pVertexBuffer, m_numToRender);
	m_bIsChanged = 0;
}

void UpdateTextPerFrameBuffer()
{
	if (!m_bIsChangedPerFrame)
		return;

	UpdateBuffer(m_textPerFrame, m_pPerFrameVertexBuffer, m_numToRenderPerFrame);
	m_bIsChangedPerFrame = 0;
}

int acsiiToIndex(unsigned font, unsigned code)
{
	if (code < m_fonts[font].acsiiToIndex.size())
        return m_fonts[font].acsiiToIndex[code];
	else
		return -1;
}

bool CheckResources(void)
{
	if (m_numFonts == 0)
		return 0;

	if (!m_pDXResources || !m_pDXResources->GetPS(ps_text) || !m_pDXResources->GetVS(vs_text) || !m_pDXResources->GetGS(gs_text) || !m_pDXResources->GetBufSRV(text_buffer))
		return 0;

	for(int i = 0; i < m_numFonts; ++i)
	if (!m_pDXResources->GetTexSRV(i))
		return 0;

	return true;
}


public:
	sgTextRenderer() : 
	m_pDXResources(0),
	m_maxLength(1500),
	m_scale(1.f),
	m_numFonts(0),
	m_bIsChanged(0),
	m_bIsChangedPerFrame(0),
	m_numToRender(0),
	m_numToRenderPerFrame(0),
	m_bRenderText(0),
	m_bRenderTextPerFrame(0),
	m_pD3DDevice(0),
    m_pDeviceContext(0),
	m_pVertexBuffer(0),
	m_pPerFrameVertexBuffer(0),
	m_PSColorBuffer(0){}

	~sgTextRenderer()
	{
		SAFE_RELEASE(m_pVertexBuffer)
		SAFE_RELEASE(m_pPerFrameVertexBuffer)
		SAFE_RELEASE(m_PSColorBuffer)
	}

void SetFontName(char* discriptionFileName, char* textureFileName)
{
	if (m_fontFileNames.size() >= FONT_LIMIT)
		return;

	std::pair<char*, char*> p(discriptionFileName, textureFileName);
	m_fontFileNames.push_back(p);
}

void SetResources(DX11Resources* pDXResources, ID3D11DeviceContext* pDeviceContext)
{
	m_pDXResources = pDXResources;
	m_pD3DDevice = pDXResources->m_pD3DDevice;
	m_pDeviceContext = pDeviceContext;
}

bool CreateFonts(void)
{
	if (!m_pDXResources)
		return 0;

	if (m_fontFileNames.empty() || m_numFonts > 0)
		return 0;

	for(int i = 0; i < m_fontFileNames.size(); ++i)
		if (ReadTable(m_fontFileNames[i].first))
		m_pDXResources->AddTexture(m_fontFileNames[i].second, m_numFonts - 1);
				

	if (m_numFonts == 0)
		return 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.SysMemPitch = 0; 
	InitData.SysMemSlicePitch = 0; 

    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;

	unsigned bufferSize = 0;
	for(int i = 0; i < m_numFonts; ++i)
		bufferSize += m_fonts[i].id.size();

	m_fontOffsets.resize(m_numFonts);

    bufferDesc.ByteWidth = bufferSize * sizeof(CharParam);
	bufferDesc.StructureByteStride = sizeof(CharParam);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; 
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	bufferDesc.CPUAccessFlags = 0;

	viewDesc.Format = DXGI_FORMAT_UNKNOWN; 
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; 
	viewDesc.Buffer.ElementOffset = 0; 
	viewDesc.Buffer.ElementWidth = bufferSize;

	std::vector<CharParam> init;
	init.resize(bufferSize);

	m_fontOffsets[0] = 0;
	for(int i = 0; i < m_numFonts; ++i)
	{	
		if (i < m_numFonts - 1)
			m_fontOffsets[i+1] = m_fontOffsets[i] + m_fonts[i].id.size();

		for(int j = 0; j < m_fonts[i].id.size(); ++j)
		{
			init[m_fontOffsets[i] + j].x = m_fonts[i].x[j] / m_fonts[i].scaleW;
			init[m_fontOffsets[i] + j].y = m_fonts[i].y[j] / m_fonts[i].scaleH;
			init[m_fontOffsets[i] + j].height = m_fonts[i].height[j] / m_fonts[i].scaleH;
			init[m_fontOffsets[i] + j].width  = m_fonts[i].width[j] / m_fonts[i].scaleW;
		}
	}

    InitData.pSysMem = &init[0].x;
	m_pDXResources->CreateResource( &bufferDesc, &viewDesc, &InitData, text_buffer);
	
    m_pDXResources->AddVertexShader("VStext.hlsl", vs_text);
	m_pDXResources->AddPixelShader("PStext.hlsl", ps_text);
	m_pDXResources->AddGeometryShader("GStext.hlsl", gs_text);
	m_pDXResources->Initilize();

	bufferDesc.ByteWidth = sizeof( TextVertex ) * m_maxLength; 
	bufferDesc.MiscFlags = 0; 
	bufferDesc.StructureByteStride = 0; 
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
    if (FAILED(m_pD3DDevice->CreateBuffer( &bufferDesc, 0, &m_pVertexBuffer))) 
		return 0;

    if (FAILED(m_pD3DDevice->CreateBuffer( &bufferDesc, 0, &m_pPerFrameVertexBuffer))) 
		return 0;


	m_fontColors.resize(m_numFonts);
	for(int i = 0; i < m_numFonts; ++i)
	{
		m_fontColors[i].color[0] = 0.f;
		m_fontColors[i].color[1] = 0.f;
		m_fontColors[i].color[2] = 0.f;
		m_fontColors[i].color[3] = 1.f;
	}

	D3D11_BUFFER_DESC cbDesc;

	cbDesc.ByteWidth = sizeof(ColorParam) * m_numFonts;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	InitData.pSysMem = &m_fontColors[0].color[0];

   	if (FAILED(m_pD3DDevice->CreateBuffer(&cbDesc, &InitData, &m_PSColorBuffer)))
        return 0;
	



	//D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width = m_id.size();
 //   texDesc.Height = 1;
 //   texDesc.MipLevels = 1;
 //   texDesc.ArraySize = 1;
 //   texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 //   texDesc.SampleDesc.Count = 1;
	//texDesc.SampleDesc.Quality = 0;
 //   texDesc.Usage = D3D11_USAGE_DEFAULT;
 //   texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
 //   texDesc.CPUAccessFlags = 0;
 //   texDesc.MiscFlags = 0;
	//
	//std::vector<float> tex(texDesc.Width * texDesc.Height * 4);
	//for(int i=0; i < texDesc.Width * texDesc.Height; ++i)
	//{
	//	tex[i*4]    = m_x[i] / m_scaleW;
	//	tex[i*4 +1] = m_y[i] / m_scaleH;
	//	tex[i*4 +2] = m_height[i] / m_scaleH;
	//	tex[i*4 +3] = m_width[i] / m_scaleW;
	//}

	//InitData.pSysMem = &tex[0];
	//InitData.SysMemPitch = texDesc.Width * sizeof(float) * 4; 
	//InitData.SysMemSlicePitch = 0; 
 //   if (FAILED(m_pD3DDevice->CreateTexture2D( &texDesc, &InitData, &pTexture ))) 
 //   {
 //       printf("Create texture failed");
 //   }

	//ID3D11ShaderResourceView* texSRV;
	//m_pD3DDevice->CreateShaderResourceView(pTexture, NULL, &texSRV);
}

void Render()
{
	if(!m_bRenderText && !m_bRenderTextPerFrame)
		return;
	if(!CheckResources())
		return;

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(TextVertex); 
	offset = 0;
	m_pDeviceContext->PSSetShader(m_pDXResources->GetPS(ps_text),0,0);
	m_pDeviceContext->VSSetShader(m_pDXResources->GetVS(vs_text),0,0);
	m_pDeviceContext->GSSetShader(m_pDXResources->GetGS(gs_text),0,0);
	m_pDeviceContext->DSSetShader(0,0,0);
	m_pDeviceContext->HSSetShader(0,0,0);
	m_pDeviceContext->CSSetShader(0,0,0);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pDeviceContext->IASetInputLayout(m_pDXResources->GetLayout(vs_text));	
	m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_PSColorBuffer);
	m_pDeviceContext->PSSetShaderResources(0, m_numFonts, m_pDXResources->GetTexSRV(0));
	m_pDeviceContext->PSSetShaderResources(FONT_LIMIT, 1, m_pDXResources->GetBufSRV(text_buffer));

	if (m_bRenderText && m_pVertexBuffer)
	{
		UpdateTextBuffer();
		if (m_numToRender)
		{
		    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		    m_pDeviceContext->Draw(m_numToRender, 0);
		}
	}

	if (m_bRenderTextPerFrame && m_pPerFrameVertexBuffer)
	{
		UpdateTextPerFrameBuffer();
		if (m_numToRenderPerFrame)
		{
			m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pPerFrameVertexBuffer, &stride, &offset);
			m_pDeviceContext->Draw(m_numToRenderPerFrame, 0);
		}
	}
}




void SetText(const wchar_t* text, float x, float y, unsigned font, float scale = 1.f)
{
	Text t;
	t.text = text;
	t.x = x;
	t.y = y;
	t.scale = scale;
	if (font < m_numFonts)
		t.font = font;
	else
		t.font = 0;
    m_text.push_back(t);
	m_bIsChanged = true;
}

void SetText(const wchar_t* text, float x, float y, float width, float height, unsigned font, float scale = 1.f)
{
    SetText(text, x, y, font, scale);
	m_text.back().width = width;
	m_text.back().height = height;
}

void SetTextPerFrame(const wchar_t* text, float x, float y, int index, unsigned font, float scale = 1.f)
{
	Text t;
	t.text = text;
	t.x = x;
	t.y = y;
	t.scale = scale;
	if (font < m_numFonts)
		t.font = font;
	else
		t.font = 0;
	if (index < 0)
	    m_textPerFrame.push_back(t);
	else
	{
		if (index + 1 > m_textPerFrame.size())
			m_textPerFrame.resize(index + 1);
		m_textPerFrame[index] = t;
	}
	m_bIsChangedPerFrame = true;
}

void SetTextPerFrame(const wchar_t* text, float x, float y, float width, float height, int index, unsigned font, float scale = 1.f)
{
	SetTextPerFrame(text, x, y, index, font, scale);

	if (index < 0)
	{
		m_textPerFrame.back().width = width;
		m_textPerFrame.back().height = height;
	}
	else
	{
		m_textPerFrame[index].width = width;
		m_textPerFrame[index].height = height;
	}

}

void SetNumber(int n, float x, float y, unsigned font, float scale = 1.f)
{
	Text t;
	t.text = std::to_wstring((long double)n);//std::string(itoa(n,m_buffer,10));
	t.x = x;
	t.y = y;
	t.scale = scale;
	if (font < m_numFonts)
		t.font = font;
	else
		t.font = 0;
    m_text.push_back(t);
	m_bIsChanged = true;
}

void SetNumberPerFrame(int n, float x, float y, int index, unsigned font, float scale = 1.f)
{
	Text t;
	t.text = std::to_wstring((long double)n);
	t.x = x; 
	t.y = y;
	t.scale = scale;
	if (font < m_numFonts)
		t.font = font;
	else
		t.font = 0;
	if (index < 0)
	    m_textPerFrame.push_back(t);
	else
	{
		if (index + 1 > m_textPerFrame.size())
			m_textPerFrame.resize(index + 1);
		m_textPerFrame[index] = t;
	}

	m_bIsChangedPerFrame = true;
}

void ClearTextPerFrame(unsigned index)
{
	if (index < m_textPerFrame.size())
	{
		if (!m_textPerFrame[index].text.empty())
		{
			m_textPerFrame[index].text.clear();
			m_bIsChangedPerFrame = true;
		}
	}
}

void ClearTextPerFrame(void)
{
	for(int i = 0; i < m_textPerFrame.size(); ++i)
		m_textPerFrame[i].text.clear();
	m_bIsChangedPerFrame = true;
}

void ClearText(void)
{
	for(int i = 0; i < m_text.size(); ++i)
		m_text[i].text.clear();
	m_bIsChanged = true;
}

void SetColor(unsigned font, float r, float g, float b, float a = 1.f)
{
	if (font >= m_numFonts)
		return;

	m_fontColors[font].color[0] = r;
	m_fontColors[font].color[1] = g;
	m_fontColors[font].color[2] = b;
	m_fontColors[font].color[3] = a;

	D3D11_MAPPED_SUBRESOURCE mapResource;

	m_pDeviceContext->Map(m_PSColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource ); 
	ColorParam* buffer = (ColorParam*)mapResource.pData;
    memcpy(buffer, &m_fontColors[0].color[0], sizeof(ColorParam) * m_numFonts);
    m_pDeviceContext->Unmap(m_PSColorBuffer, 0);
}

void SetScale(float scale)
{
	m_scale = scale;
}




void EnableRender(void){
	m_bRenderText = true;
}

void DisableRender(void){
	m_bRenderText = 0;
}

void EnableRenderPerFrame(void){
	m_bRenderTextPerFrame = true;
}

void DisableRenderPerFrame(void){
	m_bRenderTextPerFrame = 0;
}

};








#endif