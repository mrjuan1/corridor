//Fragment shader, this is where all the "texmode" references will start making sense
//Also, sorry that everything is in one file, I was in a hurry to get results, time was short when I wrote this, as it is now
#version 330 core

precision lowp float;

//Input object colour
uniform vec4 col;

//Texmode and texture map
uniform int texmode;
uniform sampler2D texmap;

//Some additional texture maps for handling the various effects
uniform sampler2D noise_tex;
uniform sampler2D window_fbo;
uniform sampler2D floor_fbo;
uniform sampler2D floor_dist;

//Tex coord from vertex shader
in vec2 vtc;
//gl_Position value from vertex shader
in vec4 vsc;
//Un-transformed vertex position from vertex shader
in vec4 vpos;

out vec4 output_col;

vec4 stained_glass() {
	vec2 nsc=((vsc.xy/vsc.w)*0.5f)+0.5f;
	nsc+=((texture(noise_tex,vtc).xy*2.0f)-1.0f)*0.03f;

	return texture(window_fbo,nsc);
}

vec4 floor_reflection() {
	vec4 tex=texture(texmap,vtc);

	//Slightly warp texture coords
	vec2 nsc=((vsc.xy/vsc.w)*0.5f)+0.5f;
	nsc+=((texture(noise_tex,vtc).xy*2.0f)-1.0f)*0.001f;
	nsc=vec2(nsc.x,1.0f-nsc.y);

	//Get view from under floor
	vec4 refl=texture(floor_fbo,nsc);

	//Reflect only a short distance, and the light
	float thesh=0.11f;
	if(tex.r>thesh && tex.g>thesh && tex.b>thesh) {
		thesh=0.8f;
		if(refl.r>thesh && refl.g>thesh && refl.b>thesh)
			return refl;

		float dist=texture(floor_dist,nsc).r;
		return mix(tex,refl,dist*0.2f);
	}

	return tex;
}

void main(void) {
	output_col=col;

	switch(texmode) {
		//Basic texture output
		case 1: output_col*=texture(texmap,vtc); break;
		//Stained glass effect (warping texture coords according to the provided noise texture)
		case 2: output_col*=stained_glass(); break;
		//Floor reflection
		case 3: output_col=floor_reflection(); break;
		//Not sure what I was trying to do here, this mode is used in app.c
		case 4: output_col.rgb=vec3(1.0f-(vpos.z/1.0f));
	}

	//Discard fragment if alhpa is 0
	if(output_col.a==0.0f) discard;
}
