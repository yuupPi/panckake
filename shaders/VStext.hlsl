
struct VS_INPUT
{
	float2 topLeft    : ANCHOR;
	float2 dimensions : DIMENSIONS;
	uint id           : ID;
    uint font         : FONT;	
	float opacity     : OPACITY;
};

struct GS_INPUT
{
	float2 topLeft    : ANCHOR;
	float2 dimensions : DIMENSIONS;
	uint id           : ID;
    uint font         : FONT;	
	float opacity     : OPACITY;

};


GS_INPUT VSmain( VS_INPUT input )
{
	return input;  
}

