
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct VS_OUT_POINT
{
	float4 pos : SV_POSITION;
	float4 posw : POSITION0;
	float4 norw : NORMAL0;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	row_major float4x4 world_view_projection;
	row_major float4x4 world;
	float4 material_color;
	float4 light_direction;

	float4 lightPosRight;
	float4 lightColorRight;
	float4 lightPosLeft;
	float4 lightColorLeft;

	float4 attenuation;
}