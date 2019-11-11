/*-------------------------------------*/
//	ポイントライト構造体
/*-------------------------------------*/

struct POINTLIGHT
{
	float	index;
	float	range;
	float	type;
	float	dummy;
	float4	pos;
	float4	color;
};
#define POINTMAX 32