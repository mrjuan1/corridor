//Fragment shader, this is where all the "texmode" references will start making sense
//Also, sorry that everything is in one file, I was in a hurry to get results, time was short when I wrote this, as it is now
#version 330 core

//Final transformation matrix
uniform mat4 pview;

//Attributes
in vec3 pos; //Vertex pos
in vec2 tc; //Texture coords

//Varying variables (see fragment shader)
out vec2 vtc;
out vec4 vsc;
out vec4 vpos;

void main(void) {
	vpos=vec4(pos,1.0f);
	gl_Position=pview*vpos;
	vtc=tc;

	vsc=gl_Position;
}
