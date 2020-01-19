## Coordinates
We'll use z up, x into page, y right. This is left handed

Yaw rotates around z (thumb)
Pitch rotates around y (middle finger)
Roll rotates around x (index finger)

(Alphabetical on a left hand)

Eular angles - harder way
Rotation = (RollMatrix)(PitchMatrix)(YawMatrix)
X, then y, then z.

## Quaternions - easier way (usually)

Use unit quaternions, or everything breaks.

q = [qv, qs] // vector, and scalar component

qv = âsin(θ/2) // where â is normalised axis of rotation
qs = cos(θ/2)

VectorToNewPoint = newPoint - pos / ||newPoint - pos||

â = <1,0,0> x VectorToNewPoint / ||<1,0,0> x VectorToNewPoint||

θ = arccos(<1,0,0>•VectorToNewPoint)

If you were already facing that way, you'll get a zero vector. Avoid dividing by this. If you're antiparallel, rotate about z.

Grassman product - rotation q, then p
(pq)v = ps qv + qs pv + pv x qv
(pq)s = ps qs - pv • qv

Inverse quaternion: q^-1 = [-qv, qs]
Identity quaternion:
iv = <0,0,0>
is = 1

Rotate a vector (v) by a quaternion:

r = [v, 0] // quaternion of vector v
r' = (qr)q^-1 // quaternion of v with rotation applied, can drop the scalar component to get the final vector

Spherical Linear Interpolation (slerp)
Slerp(a, b, 0.25) // a quaternion 25% between quaternions a and b

The rotation matrix converted from a quaternion is a disgusting mess.

Because quaternions are applied right to left, to multiply quaternions, Math.h uses:
Quaternion::Concatenate(q, p) // p, then q

## 3D Models
Created in an application such as Blender or Autodesk Maya, the converted to an exchange format such as FBX or COLLADA. In practice, proprietary formats are often used because these models contain too much data. These formats are binary files, we use JSON in this chapter (in gpmesh files), and chapter 14 will talk about this more.

Github repo includes exporter scripts to gpmesh for Blender and Unreal

pos,    normal, texture
x, y, z, x, y, z, u, v

## View Matrix
A position and orientation of the camera. A lookat matrix is the minimal implementation, taking a position, target position, and up vector.

## Projection Matrix
Orthographic - objects are the same size regardless of distance - 2D
Perspective - objects further away are smaller - 3D. This is done with a perspective divide - scaling the perspective matrix so w is always 1. This is done behind the scenes in OpenGL.

Near plane - close to the camera. Anything between the camera and the near plane is not drawn

Far plane - further from the camera. Anything beyond the far plane is not drawn

ViewProjection = (View)(Projection) // convert from world space into clip space

## Z-buffering
Painters algorithm has issues in 3D, so we compute a distance to each pixel, then draw front to back to eliminate overdraw.

OpenGL handles this behind the scenes (calls it depth buffering)

Then, disable Z-buffering, and draw all transparent objects (handled by alpha-blending) using the painters algorithm, followed by 2D sprites.

- render 3D with alpha blending disabled, z-buffering enabled
- render transparent objects with alpha blending enabled, and z-buffering disabled
- render 2D with alpha-blending enabled, and z-buffering disabled

## Lighting
For lighting, you need to know the normal at the vertices. For this, you average between the normals of all triangles this vertex is a member of.

This works poorly for sharp edges, so you instead add vertices to the corner with normals pointing in the correct direction.

Ambient light - lights all things from all sides evenly
Directional light - illuminates one side of objects, but not the other. Has an RGB, and normalised vector for direction
Point light - light in all directions coming from some point. Usually has a radius, RGB value, and position
Spotlight - light in a cone. Needs a position, radius, RGB, and angle.

Bidirectional reflectance distribution function (BRDF) approximates light reflection. A classic BRDF is Phong reflection model.

- Local (calculates primary reflections only)
- Ambient component - the ambient lighting
- Diffuse component - the primary reflection, doesn't depend on camera position
- Specular component - shiny reflections, depends on camera position

Book has Phong equations here, works out a final colour from the object, and the colour calculated from each Phong component. You could also work out seperate colours for each component.

Can compute lighting:
- once per surface - flat shading
- once per vertex - Gouraud shading
- once per pixel - Phong shading (expensive, but easily handled on modern hardware). The book does this

We then need a shader to handle this.

To get the camera position from the view matrix, invert it. Then the first 3 components of the 4th row are the position of the camera.

The vertex shader needs to convert the surface from clip space to world space, and the input vertex normals from object space to world space.
