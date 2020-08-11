#version 330

// This is what we're expecting to come into our geometry shader.
// We don't even have to specify a name for our variable!
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 lightMatrices[6];

out vec4 FragPos;

void main()
{
  for (int face = 0; face < 6; face++)
  {
    // the layer we're currently drawing on. gl_Layer is built into GLSL
    gl_Layer = face;
    for (int i = 0; i < 3; i++)
    {
      // gl_in is another build in variable. It takes the 'in' from the top of the code
      FragPos = gl_in[i].gl_Position;
      gl_Position = lightMatrices[face] * FragPos;
      EmitVertex();
    }

    // Finish drawing one of our six triangles
    EndPrimitive();
  }
}
