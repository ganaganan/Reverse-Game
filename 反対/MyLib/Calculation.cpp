#include <Calcluation.h>


/**********************************************
	NormalizeDegree

	Return : -180 ` 180
**********************************************/
void Gana::NormalizeDegree(float& _out, float _degree)
{
	while (_degree > 180)
	{
		_degree -= 360;
	}
	while (_degree < -180)
	{
		_degree += 360;
	}
	_out = _degree;
}

float Gana::NormalizeDegree(float _degree)
{
	while (_degree > 180)
	{
		_degree -= 360;
	}
	while (_degree < -180)
	{
		_degree += 360;
	}
	return _degree;
}


/**********************************************
	NormalizeRadian

	Return : -180 ` 180
**********************************************/
void Gana::NormalizeRadian(float& _out, float _radian)
{
	while (_radian > 180 * 0.01745f)
	{
		_radian -= 360 * 0.01745f;
	}
	while (_radian < -180 * 0.01745f)
	{
		_radian += 360 * 0.01745f;
	}
	_out = _radian;
}

float Gana::NormalizeRadian(float _radian)
{
	while (_radian > 180 * 0.01745f)
	{
		_radian -= 360 * 0.01745f;
	}
	while (_radian < -180 * 0.01745f)
	{
		_radian += 360 * 0.01745f;
	}
	return _radian;
}