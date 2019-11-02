#ifndef _CALCULATION_H_
#define _CALCULATION_H_

#include <Vector.h>

namespace Gana
{
	///<summary>
	/// Return [ -180 Å` 180 ].
	///</summary>
	void NormalizeDegree(float& _out, float _degree);
	///<summary>
	/// Return [ -180 Å` 180 ].
	///</summary>
	float NormalizeDegree(float _degree);

	///<summary>
	/// Return [ -180 Å` 180 ].
	///</summary>
	void NormalizeRadian(float& _out, float _radian);
	///<summary>
	/// Return [ -180 Å` 180 ].
	///</summary>
	float NormalizeRadian(float _radian);

}

#endif // !_CALCULATION_H_
