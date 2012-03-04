#include "scge\Math\Vector2.h"
#include "scge\Math\Vector3.h"
#include "scge\Math\Vector4.h"

//-----------------------------------//

template<> const Vector2 Vector2::Zero(0.0f, 0.0f);
template<> const Vector2 Vector2::UnitX(1.0f, 0.0f);
template<> const Vector2 Vector2::UnitY(0.0f, 1.0f);
template<> const Vector2 Vector2::NegativeUnitX(-1.0f, 0.0f);
template<> const Vector2 Vector2::NegativeUnitY(0.0f, -1.0f);
template<> const Vector2 Vector2::UnitScale(1.0f, 1.0f);

//-----------------------------------//

template<> const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
template<> const Vector3 Vector3::UnitX(1.0f, 0.0f, 0.0f);
template<> const Vector3 Vector3::UnitY(0.0f, 1.0f, 0.0f);
template<> const Vector3 Vector3::UnitZ(0.0f, 0.0f, 1.0f);
template<> const Vector3 Vector3::NegativeUnitX(-1.0f, 0.0f, 0.0f);
template<> const Vector3 Vector3::NegativeUnitY(0.0f, -1.0f, 0.0f);
template<> const Vector3 Vector3::NegativeUnitZ(0.0f, 0.0f, -1.0f);
template<> const Vector3 Vector3::UnitScale(1.0f, 1.0f, 1.0f);

//-----------------------------------//

template<> const Vector4 Vector4::Zero(  0.0f, 0.0f, 0.0f, 0.0f );
template<> const Vector4 Vector4::UnitX( 1.0f, 0.0f, 0.0f, 0.0f );
template<> const Vector4 Vector4::UnitY( 0.0f, 1.0f, 0.0f, 0.0f );
template<> const Vector4 Vector4::UnitZ( 0.0f, 0.0f, 1.0f, 0.0f );
template<> const Vector4 Vector4::UnitW( 0.0f, 0.0f, 0.0f, 1.0f );

//-----------------------------------//