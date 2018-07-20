//This file sets up the basic shaders being used (even though onyl one set of shaders are being used)
#include "basic.h"

//Poorly-named constant of texcoord offste in main VBO (see closer to the end of this file)
const void *_basic_offset=(const void*)(3*sizeof(float));

//Store where or not initialization succeeded
byte _basic_init=0;

//Variables related to the shader program
unint _basic_p=0;
struct {
	int pview, pos, tc;
	int col, texmode, texmap;

	int noise_tex, window_fbo;
	int floor_fbo, floor_dist;
} _basic_pv;

byte init_basic(void)
{
	//Dont' allow initializing again
	if(!_basic_init)
	{
		//Create the program, load the shaders and link them
		_basic_p=glCreateProgram();
		if(load_pro(_basic_p,"vs.glsl","fs.glsl"))
		{
			mat4 mat;

			glUseProgram(_basic_p);

			_basic_pv.pview=glGetUniformLocation(_basic_p,"pview");
			_basic_pv.pos=glGetAttribLocation(_basic_p,"pos");
			_basic_pv.tc=glGetAttribLocation(_basic_p,"tc");

			_basic_pv.col=glGetUniformLocation(_basic_p,"col");
			_basic_pv.texmode=glGetUniformLocation(_basic_p,"texmode");
			_basic_pv.texmap=glGetUniformLocation(_basic_p,"texmap");

			_basic_pv.noise_tex=glGetUniformLocation(_basic_p,"noise_tex");
			_basic_pv.window_fbo=glGetUniformLocation(_basic_p,"window_fbo");
			_basic_pv.floor_fbo=glGetUniformLocation(_basic_p,"floor_fbo");
			_basic_pv.floor_dist=glGetUniformLocation(_basic_p,"floor_dist");

			//Send defaults to the shader
			idmat(mat);
			send_pview(mat);
			glEnableVertexAttribArray(_basic_pv.pos);
			glEnableVertexAttribArray(_basic_pv.tc);

			send_col(v1());
			texmode(0);
			glUniform1i(_basic_pv.texmap,0);
			glActiveTexture(GL_TEXTURE0);

			glUniform1i(_basic_pv.noise_tex,1);
			glUniform1i(_basic_pv.window_fbo,2);
			glUniform1i(_basic_pv.floor_fbo,3);
			glUniform1i(_basic_pv.floor_dist,4);

			_basic_init=1;
		}
		else //If initialization fails...
		{
			glDeleteProgram(_basic_p);
			info("Program not created\n");
		}
	}

	return _basic_init;
}

void done_basic(void)
{
	//Delete program if initialized
	if(_basic_init)
	{
		glDeleteProgram(_basic_p);
		_basic_init=0;
	}
}

//Send final matrix to shader
void send_pview(const mat4 mat)
{
	glUniformMatrix4fv(_basic_pv.pview,1,GL_FALSE,(const float*)mat);
}

//Setup attrib info for used program
void send_attribs(void)
{
	glVertexAttribPointer(_basic_pv.pos,3,GL_FLOAT,GL_FALSE,stride(),NULL);
	glVertexAttribPointer(_basic_pv.tc,2,GL_FLOAT,GL_FALSE,stride(),_basic_offset);
}

//Send base-colour to shader
void send_col(vec4 col)
{
	glUniform4fv(_basic_pv.col,1,(const float*)&col);
}

//Change the render mode, see fragment shader for different render modes
void texmode(int mode)
{
	glUniform1i(_basic_pv.texmode,mode);
}
