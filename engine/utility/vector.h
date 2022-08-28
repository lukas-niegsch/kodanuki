#pragma once
#include <array>

namespace Kodanuki
{

template <typename T>
struct Vector2D
{
	T x;
	T y;
};

template <>
struct Vector2D<float> {};

template <typename T>
struct Vector3D
{
	T x;
	T y;
	T z;
};

template <>
struct Vector3D<float> {};

template <typename T>
struct Vector4D
{
	T x;
	T y;
	T z;
	T w;
};

template <>
struct Vector4D<float> {};

};
