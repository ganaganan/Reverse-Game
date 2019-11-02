#pragma once

#include <framework.h>
#include <DirectXMath.h>
//#include <cmath>

/*------------------------------------------*/
//	Vector2
/*------------------------------------------*/
namespace Gana
{

#pragma region struct Vector2

	struct Vector2 : public DirectX::XMFLOAT2
	{

	public:
		// Initialize and uninitialize
		Vector2() : XMFLOAT2() {}
		Vector2(float x, float y) : XMFLOAT2(x, y) {}
		Vector2(const XMFLOAT2& val) : XMFLOAT2(val) {}
		Vector2(const Vector2& val) : XMFLOAT2(val) {}
		~Vector2() = default;

	public:
		// Operator fanctions
		Vector2& operator = (float val) { x = y = val; return *this; }
		Vector2& operator = (const XMFLOAT2& val) { x = val.x; y = val.y; return *this; }
		Vector2& operator = (const Vector2& val) { x = val.x; y = val.y; return *this; }

		Vector2 operator - () const { return Vector2{ -x, -y }; }
		Vector2 operator += (float val) { x += val; y += val; return *this; }
		Vector2 operator += (const Vector2& val) { x += val.x; y += val.y; return *this; }
		Vector2 operator += (const XMFLOAT2& val) { x += val.x; y += val.y; return *this; }
		Vector2 operator -= (float val) { x -= val; y -= val; return *this; }
		Vector2 operator -= (const Vector2& val) { x -= val.x; y -= val.y; return *this; }
		Vector2 operator -= (const XMFLOAT2& val) { x -= val.x; y -= val.y; return *this; }
		Vector2 operator *= (float val) { x *= val; y *= val; return *this; }
		Vector2 operator /= (float val) { x /= val; y /= val; return *this; }

	public:
		// Handy calclate function
		float Dot(const Vector2& val) const
		{
			return (x * val.x) + (y * val.y);
		}
		float Dot(const XMFLOAT2& val) const
		{
			return (x * val.x) + (y * val.y);
		}
		float Cross(const Vector2& val) const
		{
			return (x * val.y) - (y * val.x);
		}
		float Cross(const XMFLOAT2& val) const
		{
			return (x * val.y) - (y * val.x);
		}

		float Length() const { return sqrtf(LengthSq()); }
		float LengthSq() const { return(x * x) + (y * y); }

		Vector2 Normalize()
		{
			float dist = Length();
			x /= dist;
			y /= dist;
			return *this;
		}

	public:
		static float Dot(const Vector2& L, const Vector2& R) { return L.Dot(R); }
		static float Dot(const XMFLOAT2& L, const XMFLOAT2& R) { return Vector2(L).Dot(R); }
		static float Cross(const Vector2& L, const Vector2& R) { return L.Cross(R); }
		static float Cross(const XMFLOAT2& L, const XMFLOAT2& R) { return Vector2(L).Dot(R); }
		static float Length(const Vector2& val) { return sqrtf(val.LengthSq()); }
		static float Length(const XMFLOAT2& val) { return sqrtf(Vector2(val).LengthSq()); }
	};

#pragma endregion


	/*------------------------------------------*/
	//	Vector3
	/*------------------------------------------*/
#pragma region struct Vector3

	struct Vector3 : public DirectX::XMFLOAT3
	{
	public:
		// Initialize and uninitialize
		Vector3() : XMFLOAT3() {}
		Vector3(float x, float y, float z) : XMFLOAT3(x, y, z) {}
		Vector3(const Vector3& val) : XMFLOAT3(val) {}
		Vector3(const XMFLOAT3& val) : XMFLOAT3(val) {}
		~Vector3() = default;

	public:
		// Operator functions
		Vector3 operator - () { return Vector3{ -x, -y, -z }; }
		Vector3 operator += (float val) { x += val; y += val; z += val; return *this; }
		Vector3 operator += (const Vector3& val) { x += val.x; y += val.y; z += val.z; return *this; }
		Vector3 operator += (const XMFLOAT3& val) { x += val.x; y += val.y; z += val.z; return *this; }
		Vector3 operator -= (float val) { x -= val; y -= val; z -= val; return *this; }
		Vector3 operator -= (const Vector3& val) { x -= val.x; y -= val.y; z -= val.z; return *this; }
		Vector3 operator -= (const XMFLOAT3& val) { x -= val.x; y -= val.y; z -= val.z; return *this; }
		Vector3 operator *= (float val) { x *= val; y *= val; z *= val; return *this; }
		Vector3 operator /= (float val) { x /= val; y /= val; z /= val; return *this; }
	public:
		// Handy calclate function
		float Dot(const Vector3& val) const
		{
			return (x * val.x) + (y * val.y) + (z * val.z);
		}
		float Dot(const XMFLOAT3& val) const
		{
			return (x * val.x) + (y * val.y) + (z * val.z);
		}
		Vector3 Cross(const Vector3& val) const
		{
			return Vector3
			{
				(y * val.z) - (z * val.y),
				(z * val.x) - (x * val.z),
				(x * val.y) - (y * val.x)
			};
		}
		Vector3 Cross(const XMFLOAT3& val) const
		{
			return Vector3
			{
				(y * val.z) - (z * val.y),
				(z * val.x) - (x * val.z),
				(x * val.y) - (y * val.x)
			};
		}

		float Length() const { return sqrtf(LengthSq()); }
		float LengthSq() const { return(x * x) + (y * y) + (z * z); }

		Vector3 Normalize()
		{
			float dist = Length();
			x /= dist;
			y /= dist;
			z /= dist;
			return *this;
		}

	public:
		static float Dot(const Vector3& L, const Vector3& R) { return L.Dot(R); }
		static float Dot(const XMFLOAT3& L, const XMFLOAT3& R) { return Vector3(L).Dot(R); }
		static Vector3 Cross(const Vector3& L, const Vector3& R) { return L.Cross(R); }
		static Vector3 Cross(const XMFLOAT3& L, const XMFLOAT3& R) { return Vector3(L).Cross(R); }
		static float Length(const Vector3& val) { return sqrtf(val.Length()); }
		static float Length(const XMFLOAT3& val) { return sqrtf(Vector3(val).Length()); }
	};

#pragma endregion

}