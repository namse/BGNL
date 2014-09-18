#include "stdafx.h"
#include "Component.h"
#include "EventManager.h"

Component::Component()
	: isVisible_(true),
	matrix_(D2D1::Matrix3x2F::Identity()),
	positionAnimation_(&position_),
	scaleAnimation_(&scale_),
	centerAnimation_(&center_),
	rotationAnimation_(&rotation_)
{
	scale_.x = 1.f,	scale_.y = 1.f;
}


Component::~Component()
{
	EventManager::GetInstance()->RemoveEventListener(this);
}

void Component::Update(float dTime)
{	
	positionAnimation_.OnAnimate(dTime);
	scaleAnimation_.OnAnimate(dTime);
	centerAnimation_.OnAnimate(dTime);
	rotationAnimation_.OnAnimate(dTime);


	for (const auto& child : childs_)
	{
		child->Update(dTime);
	}
}

void Component::Render()
{
	matrix_ = D2D1::Matrix3x2F::Identity()
		* D2D1::Matrix3x2F::Translation(-center_.x, -center_.y)
		* D2D1::Matrix3x2F::Scale(scale_.x, scale_.y)
		* D2D1::Matrix3x2F::Rotation(180.f * rotation_ / M_PI)
		* D2D1::Matrix3x2F::Translation(position_.x, position_.y);

	if (parent_)
	{
		matrix_ = matrix_ * parent_->GetMatrix();
	}

	for (const auto& child : childs_)
	{
		if (child->IsVisible() == true)
		{
			child->Render();
		}
	}
	
}

void Component::AddChild(Component* child)
{
	childs_.push_back(child);
	child->SetParent(this);
}



void Component::DoAnimate(ANIMATION_TYPE type, void* purpose, float duration)
{
	switch (type)
	{
	case POSITION:
	{
		positionAnimation_.DoAnimate(*(D2D_POINT_2F*)purpose, duration);
	}break;
	case ROTATION:
	{
		rotationAnimation_.DoAnimate(*(float*)purpose, duration);
	}break;
	case SCALE:
	{
		scaleAnimation_.DoAnimate(*(D2D_POINT_2F*)purpose, duration);
	}break;
	case CENTER:
	{
		centerAnimation_.DoAnimate(*(D2D_POINT_2F*)purpose, duration);
	}break;

	}
}

void Component::RemoveChild(Component* child)
{
	for (auto& iter = childs_.begin(); iter != childs_.end(); iter++)
	{
		if ((*iter) == child)
		{
			childs_.erase(iter);
			break;
		}
	}
}