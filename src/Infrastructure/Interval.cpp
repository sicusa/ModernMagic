#include "stdafx.h"

#include "../Util/MathUtil.h"
#include "Interval.h"

MM_BEGIN

float IF_Linear(float k) {
	return k;
}

float IF_Smooth(float k) {
	float num = MathUtil::Clamp(k, 0.0f, 1.0f);
	return (num * num) * (3.0f - (2.0f * num));
}

float IF_SineIn(float k) {
	return -1 * cosf(MathUtil::PiOver2 * k) + 1;
}

float IF_SineOut(float k) {
	return sinf(MathUtil::PiOver2 * k);
}

float IF_SineInOut(float k) {
	return -0.5f * (cosf(MathUtil::Pi * k) - 1);
}

float IF_ExponentialIn(float k) {
	return k == 0 ? 0 : powf(2, 10 * (k - 1)) - 1 * 0.001f;
}

float IF_ExponentialOut(float k) {
	return -powf(2, -10 * k) + 1;
}

float IF_ExponentialInOut(float k) {
	float tmp = k * 2;
	return tmp < 1.0f ?
		0.5f * powf(2, 10 * (tmp - 1)) :
		0.5f * (-powf(2, -10 * (tmp - 1)) + 2);
}

MM_END