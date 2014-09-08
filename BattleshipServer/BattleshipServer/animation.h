template <typename T>
class Animation
{
public:
	Animation(T* object)
	{
		object_ = object;
		isAnimating_ = false;
		isFirst = true;
	}

	~Animation()
	{

	}

	void DoAnimate(T nextObject, float duration)
	{
		if (isFirst != true)
		{
			*object_ = nextObject_;
		}
		else
			isFirst = true;
		prevObject_ = *object_;
		nextObject_ = nextObject;
		isAnimating_ = true;
		duration_ = duration;
		currentTime_ = 0.f;
	}

	void OnAnimate(float dTime)
	{
		if (isAnimating_ == true)
		{
			currentTime_ += dTime;
			if (currentTime_ >= duration_)
			{
				// end of Animate;
				OnEnd();
			}
			else
			{
				*object_ = prevObject_ + (nextObject_ - prevObject_) * (currentTime_ / duration_);
			}
		}
	}

private:
	void OnEnd()
	{
		currentTime_ = duration_;
		isAnimating_ = false;
		*object_ = nextObject_;
	}

private:
	T* object_;
	bool isAnimating_;
	float duration_;
	float currentTime_;
	T prevObject_;
	T nextObject_;
	bool isFirst;
};