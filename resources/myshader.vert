#version 330
in vec3 apos;
uniform mat4 m_transform;
void main()
{
    gl_Position = m_transform * vec4(apos.x, apos.y, apos.z, 1.0);
}