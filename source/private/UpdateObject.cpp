#include "UpdateObject.h"
#include "Window.h"
#include <glad/glad.h>

UpdateObject::UpdateObject(bool addToUpdateList)
{
	if(addToUpdateList)
	{
		Window::Instance().StartUpdatingObject(this);
	}
}

UpdateObject::~UpdateObject()
{
	Window::Instance().StopUpdatingObject(this);
}