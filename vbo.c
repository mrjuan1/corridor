#include "vbo.h"

const int _stride=5*sizeof(float);

//VBO setup stuff, similar style to "basic.c", so be sure to check the comments there
byte _vbo_init=0;

unint _vbo=0;
//Local var for vbo vertex data, gets freed when sending data to GPU
char *_vdata=NULL;
int _vdata_size=0;

int get_stride(void)
{
	return _stride;
}

byte init_vbo(void)
{
	if(!_vbo_init && init_basic())
	{
		glGenBuffers(1,&_vbo);
		glBindBuffer(GL_ARRAY_BUFFER,_vbo);
		send_attribs();

		_vbo_init=1;
	}

	return _vbo_init;
}

void done_vbo(void)
{
	if(_vbo_init)
	{
		if(_vdata!=NULL)
		{
			free((void*)_vdata);
			_vdata=NULL;
			_vdata_size=0;
		}

		glDeleteBuffers(1,&_vbo);
		_vbo_init=0;
	}
}

//Function for loading models into the VBO buffer before sending to the GPU
void add_to_vbo(vbo_data *v, const void *data, int size)
{
	const int offset=_vdata_size;
	_vdata_size+=size;

	if(_vdata==NULL) _vdata=(char*)malloc(_vdata_size);
	else _vdata=(char*)realloc((void*)_vdata,_vdata_size);

	memcpy((void*)(_vdata+offset),data,size);

	v->offset=offset/_stride;
	v->count=size/_stride;
}

//Function for sending the VBO to the GPU and freeing the local vertex buffer
void update_vbo(void)
{
	if(_vdata!=NULL)
	{
		glBufferData(GL_ARRAY_BUFFER,_vdata_size,(const void*)_vdata,GL_STATIC_DRAW);

		free((void*)_vdata);
		_vdata=NULL;
		_vdata_size=0;
	}
	else info("No data in VBO to update\n");
}

//Function for drawing a section of the VBO
void draw_vbo(const vbo_data *v)
{
	glDrawArrays(GL_TRIANGLES,v->offset,v->count);
}
