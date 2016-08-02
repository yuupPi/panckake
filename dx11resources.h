#ifndef DX11_RESOURCES
#define DX11_RESOURCES

#define SAFE_RELEASE(x) if(x) { x->Release(); x=0; }
#define _CS_ 0
#define _VS_ 1
#define _HS_ 2
#define _DS_ 3
#define _GS_ 4
#define _PS_ 5
#define NUM_TYPES 6

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <vector>

float convertPixelsToClipSpace( const int pixelDimension, const int pixels )
{
	return (float)pixels/pixelDimension*2 -1;
}

float convertPixelsToClipSpaceDistance( const int pixelDimension, const int pixels )
{
	return (float)pixels/pixelDimension*2;
}

struct CreateShaderParams
{
    char* version;
	char* entryPoint;
};

const CreateShaderParams CreationParams[NUM_TYPES] ={
	{"cs_5_0", "CSmain"},
	{"vs_5_0", "VSmain"},
	{"hs_5_0", "HSmain"},
	{"ds_5_0", "DSmain"},
	{"gs_5_0", "GSmain"},
	{"ps_5_0", "PSmain"}
};

struct VertexLayout
{
	D3D11_INPUT_ELEMENT_DESC* layoutDesc;
	int numElements;
	VertexLayout() : layoutDesc(0) {}
};

struct BufferResource
{
	ID3D11Buffer* buffer;
	ID3D11ShaderResourceView* srv;
    ID3D11UnorderedAccessView* uav;
	BufferResource() : buffer(0), srv(0), uav(0) {}
   ~BufferResource(){
        SAFE_RELEASE(buffer)
        SAFE_RELEASE(srv)
		SAFE_RELEASE(uav)
   }
};

class DX11Resources
{
		std::vector<char*>                       m_shaderFileNames[NUM_TYPES];
	    std::vector<char*>                       m_textureFileNames;
		std::vector<ID3D11ComputeShader*>        m_CS;
		std::vector<ID3D11VertexShader*>         m_VS;
        std::vector<ID3D11HullShader*>           m_HS;
		std::vector<ID3D11DomainShader*>         m_DS;
		std::vector<ID3D11GeometryShader*>       m_GS;
	    std::vector<ID3D11PixelShader*>          m_PS;
		std::vector<ID3D11ShaderResourceView*>   m_textureSRV;
		//std::vector<VertexLayout>                m_vertexLayout;
		std::vector<ID3D11InputLayout*>          m_inputLayout;
		std::vector<BufferResource> m_bufferResources;

public:	
	    ID3D11Device*  m_pD3DDevice;

		DX11Resources(){}

		DX11Resources(ID3D11Device* pD3DDevice) :
		m_pD3DDevice(pD3DDevice){}

	   virtual ~DX11Resources()
	   {
			for(int i=0; i < m_textureSRV.size(); ++i)  
				SAFE_RELEASE(m_textureSRV[i])

            for(int i=0; i < m_inputLayout.size(); ++i)
				SAFE_RELEASE(m_inputLayout[i])

            for(int i=0; i < m_CS.size(); ++i)
				SAFE_RELEASE(m_CS[i])
            for(int i=0; i < m_VS.size(); ++i)
		        SAFE_RELEASE(m_VS[i])
            for(int i=0; i < m_HS.size(); ++i)
                SAFE_RELEASE(m_HS[i])
            for(int i=0; i < m_DS.size(); ++i)
		        SAFE_RELEASE(m_DS[i])
            for(int i=0; i < m_GS.size(); ++i)
		        SAFE_RELEASE(m_GS[i])
            for(int i=0; i < m_PS.size(); ++i)
	            SAFE_RELEASE(m_PS[i])
	   }

void SetDevice(ID3D11Device* pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

void AddComputeShader(char* fileName, unsigned index)
{
	if (m_shaderFileNames[_CS_].size() < index + 1)
	{
		m_shaderFileNames[_CS_].resize(index + 1, 0);
		m_CS.resize(index + 1, 0);
	}
	if (!m_CS[index])
		m_shaderFileNames[_CS_][index] = fileName;
}

void AddVertexShader(char* fileName, unsigned index/*, const VertexLayout &vertexLayout*/)
{
	if (m_shaderFileNames[_VS_].size() < index + 1)
	{
		m_shaderFileNames[_VS_].resize(index + 1, 0);
		m_VS.resize(index + 1, 0);
		m_inputLayout.resize(index + 1, 0);
		//m_vertexLayout.resize(index + 1);
	}
	if (!m_VS[index])
	    m_shaderFileNames[_VS_][index] = fileName;
	//m_vertexLayout[index] = vertexLayout;
}

void AddHullShader(char* fileName, unsigned index)
{
	if (m_shaderFileNames[_HS_].size() < index + 1)
	{
		m_shaderFileNames[_HS_].resize(index + 1, 0);
		m_HS.resize(index + 1, 0);
	}
	if (!m_HS[index])
	    m_shaderFileNames[_HS_][index] = fileName;
}

void AddDomainShader(char* fileName, unsigned index)
{
	if (m_shaderFileNames[_DS_].size() < index + 1)
	{
		m_shaderFileNames[_DS_].resize(index + 1, 0);
		m_DS.resize(index + 1, 0);
	}
	if (!m_DS[index])
	    m_shaderFileNames[_DS_][index] = fileName;
}

void AddGeometryShader(char* fileName, unsigned index)
{
	if (m_shaderFileNames[_GS_].size() < index + 1)
	{
		m_shaderFileNames[_GS_].resize(index + 1, 0);
		m_GS.resize(index + 1, 0);
	}
	if (!m_GS[index])
		m_shaderFileNames[_GS_][index] = fileName;
}

void AddPixelShader(char* fileName, unsigned index)
{
	if (m_shaderFileNames[_PS_].size() < index + 1)
	{
		m_shaderFileNames[_PS_].resize(index + 1, 0);
		m_PS.resize(index + 1, 0);
	}
	if (!m_PS[index])
	    m_shaderFileNames[_PS_][index] = fileName;
}

void AddTexture(char* fileName, unsigned index)
{
	if (m_textureFileNames.size() < index + 1)
	{
		m_textureFileNames.resize(index + 1, 0);
		m_textureSRV.resize(index + 1, 0);
	}
	if (!m_textureSRV[index])
	    m_textureFileNames[index] = fileName;
}


bool CompileShaders(void)
{
	ID3D10Blob* pErrorMsgs = 0;
	ID3D10Blob* pShaderBuffer = 0;

	for(unsigned i=0; i < NUM_TYPES; ++i)
	{
		for(unsigned j=0; j < m_shaderFileNames[i].size(); ++j)
		{
			if (m_shaderFileNames[i][j])
			{
				if ( FAILED( D3DX11CompileFromFile(	m_shaderFileNames[i][j], 
													NULL, NULL,
													CreationParams[i].entryPoint,
													CreationParams[i].version, 
													0, 
													0, 
													0,
													&pShaderBuffer, 
													&pErrorMsgs,
													0) ) ) 
													{
														MessageBox(0, (LPCTSTR)pErrorMsgs->GetBufferPointer(),0,MB_OK);
														SAFE_RELEASE(pErrorMsgs)
														SAFE_RELEASE(pShaderBuffer);
														return 0;
													}
   
				 
				switch (i){
					case(_CS_):
						m_pD3DDevice->CreateComputeShader (pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_CS[j]);
					break;
					case(_VS_):
						m_pD3DDevice->CreateVertexShader  (pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_VS[j]);
						//if (m_vertexLayout[j].layoutDesc)
						//m_pD3DDevice->CreateInputLayout( m_vertexLayout[j].layoutDesc, m_vertexLayout[j].numElements, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), &m_inputLayout[j]);
						CreateInputLayoutDescFromVertexShaderSignature( pShaderBuffer, m_pD3DDevice, &m_inputLayout[j]);
					break;
					case(_HS_):
						m_pD3DDevice->CreateHullShader    (pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_HS[j]);
					break;
					case(_DS_):
						m_pD3DDevice->CreateDomainShader  (pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_DS[j]);
					break;
					case(_GS_):
						m_pD3DDevice->CreateGeometryShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_GS[j]);
					break;
					case(_PS_):
						m_pD3DDevice->CreatePixelShader   (pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), 0,
															&m_PS[j]);
					break;
				}
				m_shaderFileNames[i][j] = 0;
			}
		}
	}

	SAFE_RELEASE(pShaderBuffer)
	return true;
}

bool CreateTextureSRV(void)
{
	for(int i=0; i < m_textureFileNames.size(); ++i)
	{
		if (m_textureFileNames[i])
		{
			if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, m_textureFileNames[i], NULL, NULL, &m_textureSRV[i], NULL)) )
				return 0;
			m_textureFileNames[i] = 0;
		}
	}
	return true;
}

bool Initilize(void)
{
	if (!CompileShaders() || !CreateTextureSRV())
		return 0;
	return true;
}

HRESULT CreateInputLayoutDescFromVertexShaderSignature( ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout )
{
    ID3D11ShaderReflection* pVertexShaderReflection = NULL;
    if ( FAILED( D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &pVertexShaderReflection ) ) )
    {
        return S_FALSE;
    }
 
    D3D11_SHADER_DESC shaderDesc;
    pVertexShaderReflection->GetDesc( &shaderDesc );

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
    for ( int i=0; i< shaderDesc.InputParameters; i++ )
    {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc );
 
        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = paramDesc.SemanticName;
        elementDesc.SemanticIndex = paramDesc.SemanticIndex;
        elementDesc.InputSlot = 0;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        elementDesc.InstanceDataStepRate = 0;   
 
        if ( paramDesc.Mask == 1 )
        {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
        }
        else if ( paramDesc.Mask <= 3 )
        {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if ( paramDesc.Mask <= 7 )
        {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if ( paramDesc.Mask <= 15 )
        {
            if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
 
        inputLayoutDesc.push_back(elementDesc);
    }       
 
    HRESULT hr = pD3DDevice->CreateInputLayout( inputLayoutDesc.data(), inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout );
 
    pVertexShaderReflection->Release();
    return hr;
}

void CreateResource( D3D11_BUFFER_DESC* bufferDesc, /*D3D11_SHADER_RESOURCE_VIEW_DESC*/void* viewDesc, D3D11_SUBRESOURCE_DATA* initData, unsigned index)
{
	if (m_bufferResources.size() < index + 1)
		m_bufferResources.resize(index + 1);

	if (m_bufferResources[index].buffer)
		return;

	if(FAILED(m_pD3DDevice->CreateBuffer( bufferDesc, initData, &m_bufferResources[index].buffer ))) 
		return;

	if (bufferDesc->BindFlags & D3D11_BIND_UNORDERED_ACCESS)
        m_pD3DDevice->CreateUnorderedAccessView(m_bufferResources[index].buffer, (D3D11_UNORDERED_ACCESS_VIEW_DESC*) viewDesc,
		                                        &m_bufferResources[index].uav);  
	else
	    m_pD3DDevice->CreateShaderResourceView( m_bufferResources[index].buffer, (D3D11_SHADER_RESOURCE_VIEW_DESC*) viewDesc, 
	                                           &m_bufferResources[index].srv ); 
}

ID3D11ComputeShader* GetCS(unsigned index)
{
	if (m_CS.size() == 0 || index >= m_CS.size()) return 0;
	else return m_CS[index];
}

ID3D11VertexShader* GetVS(unsigned index)
{
	if (m_VS.size() == 0 || index >= m_VS.size()) return 0;
	else return m_VS[index];
}

ID3D11HullShader* GetHS(unsigned index)
{
	if (m_HS.size() == 0 || index >= m_HS.size()) return 0;
	else return m_HS[index];
}

ID3D11DomainShader* GetDS(unsigned index)
{
	if (m_DS.size() == 0 || index >= m_DS.size()) return 0;
	else return m_DS[index];
}

ID3D11GeometryShader* GetGS(unsigned index)
{
	if (m_GS.size() == 0 || index >= m_GS.size()) return 0;
	else return m_GS[index];
}

ID3D11PixelShader* GetPS(unsigned index)
{
	if (m_PS.size() == 0 || index >= m_PS.size()) return 0;
	else return m_PS[index];
}

ID3D11InputLayout* GetLayout(unsigned index)
{
	if (m_inputLayout.size() == 0 || index >= m_inputLayout.size()) return 0;
	else return m_inputLayout[index];
}

ID3D11ShaderResourceView** GetTexSRV(unsigned index)
{
	if (m_textureSRV.size() == 0 || index >= m_textureSRV.size() || m_textureSRV[index] == 0) return 0;
	else return &m_textureSRV[index];
}

ID3D11ShaderResourceView** GetBufSRV(unsigned index)
{
	if (m_bufferResources.size() == 0 || index >= m_bufferResources.size() || m_bufferResources[index].srv == 0) return 0;
	else return &m_bufferResources[index].srv;
}

ID3D11UnorderedAccessView** GetBufUAV(unsigned index)
{
	if (m_bufferResources.size() == 0 || index >= m_bufferResources.size() || m_bufferResources[index].uav == 0) return 0;
	else return &m_bufferResources[index].uav;
}

};

#endif

