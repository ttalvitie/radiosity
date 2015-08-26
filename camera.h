/// @file
#pragma once

#include "common.h"
#include "vec3.h"

/// Structure containing information about camera direction and position, used
/// for rendering a scene.
typedef struct {
	/// The position of the camera.
	vec3 position;
	
	/// The direction the camera looks at.
	vec3 forward;
	
	/// The direction directly towards the right from the camera.
	vec3 rightwards;
	
	/// The width of the picture taken by the camera.
	double width;
	
	/// The height of the picture taken by the camera.
	double height;
	
	/// The horizontal field of view of the camera in degrees.
	double fov_angle;
} camera;
