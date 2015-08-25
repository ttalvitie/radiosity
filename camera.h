#pragma once

#include "common.h"
#include "vector.h"

/// Structure containing information about camera direction and position, used
/// for rendering a scene.
typedef struct {
	/// The position of the camera.
	vector position;
	
	/// The direction the camera looks at.
	vector forward;
	
	/// The direction directly towards the right from the camera.
	vector rightwards;
	
	/// The width of the picture taken by the camera.
	double width;
	
	/// The height of the picture taken by the camera.
	double height;
	
	/// The horizontal field of view of the camera in degrees.
	double fov_angle;
} camera;
