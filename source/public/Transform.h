#pragma once
#include "glm/vec3.hpp"

template<class T>
class Position
{
public:
	Position(T& inPosition = T(0)) { SetPosition(inPosition); };

public:
	T& GetPosition() { return position; };
	void SetPosition(T& inPosition) { position = inPosition; };
	void Move(glm::vec3 movement) { position += movement; };

protected:
	T position;
};

class Rotation
{
public:
	Rotation(glm::vec3& inRotation = glm::vec3(0)) { SetRotation(inRotation); };

public:
	glm::vec3& GetRotation() { return rotation; };
	void SetRotation(glm::vec3& inRotation) { rotation = inRotation; };
	virtual void Rotate(glm::vec3& rotate) { rotation -= rotate; };

	glm::vec3 GetDirection() const;
	glm::vec3 GetRightDirection() const;

protected:
	glm::vec3 rotation;
};

class Scale
{
public:
	Scale(glm::vec3& inScale = glm::vec3(1)) { SetScale(inScale); };

public:
	glm::vec3& GetScale() { return scale; };
	void SetScale(glm::vec3& inScale) { scale = inScale; };

protected:
	glm::vec3 scale;
};

template<class T>
class Transform : public Position<T>, public Rotation, public Scale
{
public:
	Transform(T& inPosition = T(0), glm::vec3& inRotation = glm::vec3(0), glm::vec3& inScale = glm::vec3(1)) : 
		Position(inPosition), Rotation(inRotation), Scale(inScale) { }

};

