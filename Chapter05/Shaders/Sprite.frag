// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;

in vec3 vertRGB;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

void main()
{
    vec4 averagedColour = texture(uTexture, fragTexCoord);
	averagedColour.x = (averagedColour.x + vertRGB.x) / 2; // need to make sure these don't go out of bounds
	averagedColour.y = (averagedColour.y + vertRGB.y) / 2;
	averagedColour.z = (averagedColour.z + vertRGB.z) / 2;

	// Sample color from texture
	outColor = averagedColour;
	//outColor = texture(uTexture, fragTexCoord);
}
