/* Copyright (c) 2016 youka

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgement in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution. */

precision lowp float;

uniform vec4 col;

uniform int texmode;
uniform sampler2D texmap;

uniform sampler2D noise_tex;
uniform sampler2D window_fbo;
uniform sampler2D floor_fbo;
uniform sampler2D floor_dist;

varying vec2 vtc;
varying vec4 vsc;
varying vec4 vpos;

void main(void)
{
	gl_FragColor=col;

	if(texmode==1)
		gl_FragColor*=texture2D(texmap,vtc);
	else if(texmode==2)
	{
		vec2 nsc=((vsc.xy/vsc.w)*0.5)+0.5;
		nsc+=((texture2D(noise_tex,vtc).xy*2.0)-1.0)*0.03;

		gl_FragColor*=texture2D(window_fbo,nsc);
	}
	else if(texmode==3)
	{
		vec4 tex=texture2D(texmap,vtc);

		vec2 nsc=((vsc.xy/vsc.w)*0.5)+0.5;
		nsc+=((texture2D(noise_tex,vtc).xy*2.0)-1.0)*0.001;
		nsc=vec2(nsc.x,1.0-nsc.y);
		vec4 refl=texture2D(floor_fbo,nsc);

		float thesh=0.11;
		if(tex.r>thesh && tex.g>thesh && tex.b>thesh)
		{
			thesh=0.8;
			if(refl.r>thesh && refl.g>thesh && refl.b>thesh)
				gl_FragColor=refl;
			else
			{
				float dist=texture2D(floor_dist,nsc).r;
				gl_FragColor=mix(tex,refl,dist*0.2);
			}
		}
		else gl_FragColor=tex;
	}
	else if(texmode==4)
		gl_FragColor.rgb=vec3(1.0-(vpos.z/1.0));

	if(gl_FragColor.a==0.0) discard;
}
