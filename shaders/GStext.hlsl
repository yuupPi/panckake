
struct GS_INPUT
{
	float2 topLeft    : ANCHOR;
	float2 dimensions : DIMENSIONS;
	uint id           : ID;
	uint font         : FONT;	
	float opacity     : OPACITY;

};

struct PS_INPUT
{
	float4 pos     : SV_POSITION; 
	float2 tex     : TEXCOORD;
	uint id        : ID;
	uint font      : FONT;	
	float opacity  : OPACITY;	

};


[maxvertexcount(4)]
void GSmain( point GS_INPUT vertex[1], inout TriangleStream<PS_INPUT> triStream )
{
    PS_INPUT v;
	v.id = vertex[0].id;
	v.font = vertex[0].font;
	v.opacity = vertex[0].opacity;
 
    //bottom left
    v.pos = float4(vertex[0].topLeft[0], vertex[0].topLeft[1] - vertex[0].dimensions[1], 0, 1);
    v.tex = float2(0,1);
    triStream.Append(v);
 
    //top left
    v.pos = float4(vertex[0].topLeft[0], vertex[0].topLeft[1], 0, 1);
    v.tex = float2(0,0);
    triStream.Append(v);
 
    //bottom right
    v.pos = float4(vertex[0].topLeft[0] + vertex[0].dimensions[0], vertex[0].topLeft[1] - vertex[0].dimensions[1], 0, 1);
    v.tex = float2(1,1);
    triStream.Append(v);
 
    //top right
    v.pos = float4(vertex[0].topLeft[0] + vertex[0].dimensions[0], vertex[0].topLeft[1], 0, 1);
    v.tex = float2(1,0);
    triStream.Append(v);
}

