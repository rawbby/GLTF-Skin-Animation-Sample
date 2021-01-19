#version 400 core

layout(location = 0) in vec3  v_vertex;
layout(location = 1) in vec3  v_normal;
layout(location = 2) in vec2  v_texcoord;
layout(location = 3) in vec4 v_joint_indices;
layout(location = 4) in vec4  v_joint_weights;

uniform mat4 u_projection_matrix;
uniform mat4 u_model_view_matrix;
uniform mat4 u_joints[128];
void main()
{
    mat4 joint_transformation = u_joints[int(v_joint_indices[0])] * v_joint_weights[0];
    joint_transformation += u_joints[int(v_joint_indices[1])] * v_joint_weights[1];
    joint_transformation += u_joints[int(v_joint_indices[2])] * v_joint_weights[2];
    joint_transformation += u_joints[int(v_joint_indices[3])] * v_joint_weights[3];
    gl_Position = u_projection_matrix * u_model_view_matrix * joint_transformation * vec4(v_vertex, 1.0f);
    //gl_Position = u_projection_matrix * u_model_view_matrix * vec4(v_vertex, 1.0f);
}