#pragma once

namespace glsl::fs
{
    const inline auto text = R"(#version 450 core

out vec4 o_color;

void main()
{
    o_color = vec4(0.8f, 0.3f, 0.1f, 0.4f);
}
)";
}
