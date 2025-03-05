#version 460 core


// random value for colors etc
uniform float random;


out vec4 FragColor;


void CalcColor(inout vec3 color, vec2 coord)
{   
    float d = length(coord);
    // sdf for circle
    d -= 0.5;
    d = abs(d);
    
    // removing black border
    d += 0.25;

    color *= d;
}

void main()
{
    // making a circle from point
    vec2 coord = gl_PointCoord - 0.5;

    // basically x^2 + y^2
    float distSquared = dot(coord, coord);

    float radius = (coord.x + coord.y) / 2.0;

    if(distSquared > 0.25) discard;


    vec3 color = vec3(1.0, 1.0, 1.0);
    CalcColor(color, coord);

    FragColor = vec4(color, 1.0);
}