#define FONT_LIMIT 10

cbuffer cbColor : register( b0 ) 
{    
    float4 g_color[FONT_LIMIT];
};

struct Char
{
	float2 pos;
	float width;
	float height;
};

struct PS_INPUT
{
	float4 pos    : SV_POSITION; 
	float2 tex    : TEXCOORD;
	uint id       : ID;
	uint font     : FONT;	
	float opacity : OPACITY;	

};

SamplerState linearSampler
{
    Filter = min_mag_mip_linear;
    AddressU = Clamp;
    AddressV = Clamp;
    MaxAnisotropy = 16;
};


Texture2D   colorMap [FONT_LIMIT]; 
StructuredBuffer<Char> char;       

float4 PSmain( PS_INPUT input ) : SV_Target
{   
    float2 texCoords;
	texCoords.x = input.tex.x * char[input.id].width + char[input.id].pos.x;
	texCoords.y = input.tex.y * char[input.id].height+ char[input.id].pos.y;
	
	float4 color;
			
	switch	(input.font){
		
		case 0:	color = colorMap[0].Sample(linearSampler, texCoords);
		break;
		case 1:	color = colorMap[1].Sample(linearSampler, texCoords);
		break;
		case 2:	color = colorMap[2].Sample(linearSampler, texCoords);
		break;
		case 3:	color = colorMap[3].Sample(linearSampler, texCoords);
		break;
		case 4:	color = colorMap[4].Sample(linearSampler, texCoords);
		break;
		case 5:	color = colorMap[5].Sample(linearSampler, texCoords);
		break;
		case 6:	color = colorMap[6].Sample(linearSampler, texCoords);
		break;
		case 7:	color = colorMap[7].Sample(linearSampler, texCoords);
		break;
		case 8:	color = colorMap[8].Sample(linearSampler, texCoords);
		break;
		case 9:	color = colorMap[9].Sample(linearSampler, texCoords);
		break;
	}
	
	if (g_color[input.font].r == 0.f && g_color[input.font].g == 0.f && g_color[input.font].b == 0.f && g_color[input.font].a == 1.f)
	    color.w = min(color.w, input.opacity);
	else
	{
	    color.rgb = g_color[input.font].rgb;
		color.w = min(g_color[input.font].w, min(color.w, input.opacity));
	}

	return color;      
}