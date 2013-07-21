#pragma once

MM_BEGIN

typedef float (*IntervalFunc)(float k);

float IF_Linear				(float k);
float IF_Smooth				(float k);
float IF_SineIn				(float k);
float IF_SineOut			(float k);
float IF_SineInOut			(float k);
float IF_ExponentialIn		(float k);
float IF_ExponentialOut		(float k);
float IF_ExponentialInOut	(float k);

template<class T>
class Interval
{
public:
	Interval();
	Interval(T start, T end, float duration, IntervalFunc func = IF_Linear);

	/**
	* @brief Start 初始化 Interval 的数据
	* @param start 起始值
	* @param end   结束值
	* @param duration 持续时间
	*/
	virtual void Start(T start, T end, float duration)
	{
		_start      = start;
		_end        = end;
		_current    = start;
		_duration   = duration;
		_timer      = 0.0f;
		_shouldStep = true;

		MM_ASSERT(_func);
	}

	T Step(float dt) {
		if (!_shouldStep) {
			return _current;
		}

		_timer += dt;

		if (_timer >= _duration) {
			_current = _end;
			_shouldStep = false;
		}

		_current = _start + (_end - _start) * _func(_timer / _duration);
		return _current;
	}

	virtual void Restart() {
		_timer = 0;
		_shouldStep = true;
	}

	virtual void Close() {
		_timer = 0;
		_shouldStep = false;
	}

private:
	// 计时器
	MM_PROPERTY_PBV(float, _timer, Timer)

	// 持续时间
	MM_PROPERTY_PBV(float, _duration, Duration)

	// 开始
	MM_PROPERTY_PBR(T, _start, Start)

	// 结束
	MM_PROPERTY_PBR(T, _end, End)

	// 当前的值
	MM_PROPERTY_PBR_NS(T, _current, Current)

	// 是否需要继续调用 Step
	MM_PROPERTY_BOOL_NS(_shouldStep, ShouldStep)

	// 间隔的插值函数
	MM_PROPERTY_PBV(IntervalFunc, _func, IntervalFunc)
};

template <typename T>
Interval<T>::Interval():
	_timer(0), _duration(0), _shouldStep(false), _func(IF_Linear)
{
}

template <typename T>
Interval<T>::Interval(T start, T end, float duration, IntervalFunc func):
	_timer(0), _duration(duration), _start(start), _end(end), _current(start),
	_shouldStep(true), _func(func)
{
	MM_ASSERT(_func);
}

MM_END
