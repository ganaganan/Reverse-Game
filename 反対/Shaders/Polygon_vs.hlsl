#include "Polygon.hlsli"
VS_OUT main(float4 pos : POSITION, float4 color : COLOR) 
{ 
	VS_OUT vout;
	vout.pos = pos;
	vout.color = color;
	return vout; 
}