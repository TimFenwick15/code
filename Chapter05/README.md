## Coordinates
SDL - (0,0) top left, positive x is right, positive y is down
OpenGL - (0,0) center, (-1,-1) bottom left, (1,1) top right. This is normalised device coordinates (NDC)

## OpenGL Objects
Vertex buffer - array of points
Index buffer - list of indices. Each 3 specify 3 points from the vertex buffer to make a triangle

OpenGL doesn't tend to return pointers; it prefers object IDs. These are IDs are unique for each object type, but two objects of a different type could have the same ID.

glGenVertexArrays(1, &mVertexArray) -> glBindVertexArray(mVertexArray)
glGenBuffers(1, &mVertexBuffer) -> glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer)
glEnableVertexAttribArray(0) -> glVertexAttribPointer()

glBufferData takes an object type instead of an ID, and acts on the last buffer bound using glBindBuffer.

"Vertex array object" contains "vertex buffer" and "index buffer"
"Vertex layout" (or "Vertex attribute") is what data you want to store for each vertex

## Shaders
Vertex shader - runs per vertex, using the vertex attribute data. So if triangles share vertices, the shader needs to run fewer times

Fragment shader - runs per pixel. Decides on the colour for each pixel. Breaking triangles into pixels is called rasterisation.

Shaders are written in GLSL (C like, but not C++)

This book keeps shaders in .vert and .frag files.

.vert:
in vec3 inPosition;
gl_Position = vec4(inPosition, w); // to store output, w will be explained in a second

gl_Position is a global that sets the position in the fragment shader.

.frag:
out vect4 outColour;
outColour = vec4(0.0, 0.0, 1.0, 1.0); // blue

Shader code -> compile -> link -> shader program

Instead of defining new vertex buffers, it's more efficient to reuse, transforming them to show differences.

Sprites live in an object space. Their coordinates are relative to that. The object space is a cuboid around the sprite

The object space can be scaled, rotated, transformed, and positioned. You place it relative to the world space. You need to transfer from object space coordinates to world space NDC coordinates.

Need to check the basis vectors match (ie x, y, and z point in the same direction.

The wrong way: scale, rotate, then transform. Modify each coordinate with a factor, a trig, and an offset respectively. Then convert to NDC.

## Matrix transformations

If Am == Bn:
C = AB =
  First A row dot first B column, first A row dot second B column
  Second A row dot first B column, second A row dot second B column

AB != BA (not commutative)
A(BC) == (AB)C (associative)

Point p = [ x,y ]. This can also be a column, graphics uses both, we will use row matrices. To transform:

p' = pT (transform by T)
p' = pTR (transform by T, then R)

Transpose is a flip around top left to bottom right. You can flip to column vectors with a transpose

p' = pTR = transpose(RTp)

Identity matrix
I3 =
  1 0 0
  0 1 0
  0 0 1

Scale:
S(Sx,Sy) =
  Sx 0
  0 Sy

Rotate
R(θ) =
  cosθ sinθ
  -sinθ cosθ

Translate
T(a,b) =
  1 0 0
  0 1 0
  a b 1
A 2x1 row matrix could not be multiplied by a 3x3 matrix. So we insert a column making
p= [x,y,1]
This is the w component. Not z, this is 2D and we don't have a z. Note that you might have to add a w component to the other transformation matrices you want to use.

The product of your transforms is the world transformation matrix. Compute this once, and recompute only if your object moves.

Clip space is the vertices seen by the view-projection matrix - the camera view. For this chapter, we are 2D and mapping the world to the pixel window size 1 to 1, with an origin in the center.

So object space to clip space becomes:

v' = v(WorldTransform)(ViewProjection)

## Uniforms
A shader variable that can be set from outside the shader, and have the same value on each vertices/pixel. eg
uniform mat4 uWorldTransform;
uniform mat4 uViewProjection;

glGetUniformLocation(shader, char* name); // name for the name of the uniform in the shader. So we can pass in data from the C++ code

If the vertex shader has an "out" var, you can use "in" in the fragment shader to access it.

"in" variables for the vertex shader come from the vertex buffer. If you have one VertexAttribute (x, y, z), GLSL knows this is what you mean (otherwise use the layout directive).

If the fragment shader has an "out vec4" var, it becomes the colour of the currently bound pixel. This used to be done with a global called gl_FragColor, but this is deprecated.

(OpenGL has uniform buffer objects for sending groups of uniforms. More efficient if you have a lot of uniforms. Not widely supported, particularly on integrated graphics devices)

## Textures
Texture mapping - map an image to the triangles you draw. Store texture coordinates on the vertex. These are 0,0 (u, v) at the bottom left. Images are stored top left on disk, so reverse V (DirectX does the same).

Texture pixels (texels) are mapped to UV coordinates, ones between are interpolated. Chosing the correct colour for a texel based on the average source image colour is nearest-neighbor filtering. If you get close to the image in game, it will be stretched and become pixelated, so bilinear filtering averages between texels which blurs the image as you approach it.

SOIL is a simple image loading library for OpenGL.

Our VertexArray is now (x, y, z, u, v, ...). So in the vertex shader:
layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexCoord;
These layouts corespond to the slots in the glVertexAttribPointers calls. You then pass this to the fragment shader with:
out vec2 fragTexCoord;

uniform sampler2D utexture;
outColour = texture(uTexture, fragTexCoord); // get the colour from the texture

Transparency (using the alpha) is enabled from the OpenGL code. The final colour is calculated using:
outputColour = newAlpha * newColour + (1 - newAlpha) * currentColour;

## Exercise Notes
If the shader fails to compile, the game will exit.

Put a break point in the destructor, the command prompt will contain the shader compiler output.

Writing data to the VectexBuffer is only done once. For the vertices this doesn't matter because the world transform handles sprite movement.

For the colour exercise, this means the colour can't change once it has been written to the VertexBuffer.
