//This file holds the main application and handles input/interactions
#include "app.h"

//Local assets
vbo_data models[6];
unint textures=0;

//Local player and matrix variables
float x=0.0f, y=0.0f, z=1.7f;
float dir=90.0f, tilt=0.0f;
mat4 proj, view, pview;

//Some fx-related variables
unint noise_tex=0;
fbo_data window_fbo;
fbo_data floor_fbo, floor_dist;

//Preparation function
void init(void)
{
	//Setup model-loading system
	if(init_vbo())
	{
		//Load models
		const char *model_fnames[6]={
			"corridor-ceiling.bin",
			"corridor-floor.bin",
			"corridor-wall.bin",
			"door.bin",
			"room.bin",
			"window.bin"
		};
		int i=0;

		for(i=0 ; i<6 ; i++)
		{
			int size=0; void *data=NULL;
			if(fload(model_fnames[i],&size,&data))
			{
				add_to_vbo(&models[i],data,size);
				free(data);
			}
			else break;
		}

		//Check that they all loaded before continuing...
		if(i==6)
		{
			//Send all vertex data to the GPU
			update_vbo();

			//Load giant texture map generated in Blender and continue if successfull...
			glGenTextures(1,&textures);
			if(load_tex(textures,"textures.data",2048,2048,GL_RGB,tf_mipmap))
			{
				//Generate noise texture for the stained glass
				const int noise_size=(sw()/2)*(sh()/2)*3;
				byte *noise_data=NULL;
				int i=0;

				//Change the "texture mode", see the fragment shaders for more info on this
				texmode(1);
				//Generate perspective projection matrix
				persp(proj,75.0f,asp(),0.1f,500.0f);

				//Continue with noise texture...
				glGenTextures(1,&noise_tex);
				glActiveTexture(GL_TEXTURE1);
				use_tex(noise_tex);
				noise_data=(byte*)malloc(noise_size);
				for(i=0 ; i<noise_size ; i++)
					noise_data[i]=(byte)(((float)rand()/(float)RAND_MAX)*255.0f);
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,sw()/2,sh()/2,0,GL_RGB,GL_UNSIGNED_BYTE,(const void*)noise_data);
				free((void*)noise_data);
				filter_tex(tf_mipmap);

				glActiveTexture(GL_TEXTURE0);
				//Setup framebuffer objects for post-processing
				init_fbo(&window_fbo,sw()/2,sh()/2,tf_linear);
				init_fbo(&floor_fbo,sw()/2,sh()/2,tf_linear);
				init_fbo(&floor_dist,sw()/2,sh()/2,tf_linear);
				//Bind the giant texture map
				use_tex(textures);
			}
			//quit, quit, quit, I hate that I used to do this...
			else quit();
		}
		else quit();
	}
	else quit();
}

//Main loop
void loop(void)
{
	//If the mouse moved
	if(mm())
	{
		//Update the camera direction and tilt
		dir-=mx();
		tilt-=my();

		if(dir<-180.0f) dir+=360.0f;
		else if(dir>180.0f) dir-=360.0f;
		if(tilt<-89.999f) tilt=-89.999f;
		else if(tilt>89.999f) tilt=89.999f;
	}

	//If one of the directional keys are being pressed (asses WSAD for righty's)
	if(ki(SDL_SCANCODE_UP) || ki(SDL_SCANCODE_DOWN) || ki(SDL_SCANCODE_LEFT) || ki(SDL_SCANCODE_RIGHT) ||
		ki(SDL_SCANCODE_W) || ki(SDL_SCANCODE_S) || ki(SDL_SCANCODE_A) || ki(SDL_SCANCODE_D))
	{
		const float spd=5.0f*dt();
		float t=dir, r=0.0f;

		if((ki(SDL_SCANCODE_UP) || ki(SDL_SCANCODE_W)) && (ki(SDL_SCANCODE_LEFT) || ki(SDL_SCANCODE_A))) t+=45.0f;
		else if((ki(SDL_SCANCODE_UP) || ki(SDL_SCANCODE_W)) && (ki(SDL_SCANCODE_RIGHT) || ki(SDL_SCANCODE_D))) t-=45.0f;
		else if((ki(SDL_SCANCODE_DOWN) || ki(SDL_SCANCODE_S)) && (ki(SDL_SCANCODE_LEFT) || ki(SDL_SCANCODE_A))) t+=135.0f;
		else if((ki(SDL_SCANCODE_DOWN) || ki(SDL_SCANCODE_S)) && (ki(SDL_SCANCODE_RIGHT) || ki(SDL_SCANCODE_D))) t-=135.0f;
		else if(ki(SDL_SCANCODE_DOWN) || ki(SDL_SCANCODE_S)) t+=180.0f;
		else if(ki(SDL_SCANCODE_LEFT) || ki(SDL_SCANCODE_A)) t+=90.0f;
		else if(ki(SDL_SCANCODE_RIGHT) || ki(SDL_SCANCODE_D)) t-=90.0f;

		//Convert to radians and move in that direction
		r=d2r(t);
		x+=cosf(r)*spd;
		y+=sinf(r)*spd;

		if(x<-1.2f) x=-1.2f;
		else if(x>1.2f) x=1.2f;
		if(y<-5.0f) y+=5.0f;
		else if(y>5.0f) y-=5.0f;
	}

	//Setup projection and view matrices
	const float r=d2r(dir);
	float c=cosf(r), s=sinf(r), t=tanf(d2r(tilt));

	mat4 floor_view, floor_pview;
	int i=0;

	look(view,v3(x,y,z),v3(x+c,y+s,z+t));
	mult(pview,proj,view);

	look(floor_view,v3(x,y,-z),v3(x+c,y+s,-z-t));
	mult(floor_pview,proj,floor_view);

	//Draw to window "refraction" framebuffer
	use_fbo(&window_fbo);
	clear();
	texmode(1);

	//Only draw 5 rooms (behind the window) ahead and behind
	for(i=-5 ; i<6 ; i++)
	{
		mat4 t[2];

		memcpy((void*)t[0],(const void*)pview,sizeof(mat4));
		trans(t[0],v3(0.0f,(float)i*5.0f,0.0f));
		send_pview(t[0]);
		draw_vbo(&models[4]);

		memcpy((void*)t[1],(const void*)t[0],sizeof(mat4));
		rotz(t[1],180.0f);
		send_pview(t[1]);
		draw_vbo(&models[4]);
	}

	unuse_fbo();
	glActiveTexture(GL_TEXTURE2);
	use_tex(window_fbo.texs[0]);
	glActiveTexture(GL_TEXTURE0);

	//Draw to floor "reflection" framebuffer
	use_fbo(&floor_fbo);
	clear();

	//Only draw 10 floor reflections (the rest of the corridor) ahead and behind
	for(i=-10 ; i<11 ; i++)
	{
		mat4 t[2];
		int j=0;

		memcpy((void*)t[0],(const void*)floor_pview,sizeof(mat4));
		trans(t[0],v3(0.0f,(float)i*5.0f,0.0f));
		send_pview(t[0]);

		memcpy((void*)t[1],(const void*)t[0],sizeof(mat4));
		rotz(t[1],180.0f);

		draw_vbo(&models[0]);
		for(j=2 ; j<4 ; j++)
		{
			draw_vbo(&models[j]);
			send_pview(t[1]);
			draw_vbo(&models[j]);
			send_pview(t[0]);
		}

		send_col(v3f(0.25f));
		texmode(0);

		draw_vbo(&models[5]);
		send_pview(t[1]);
		draw_vbo(&models[5]);

		send_col(v1());
		texmode(1);
	}

	unuse_fbo();
	glActiveTexture(GL_TEXTURE3);
	use_tex(floor_fbo.texs[0]);
	glActiveTexture(GL_TEXTURE0);

	//Draw to floor "distance" framebuffer (I don't quite remember why this exists)
	use_fbo(&floor_dist);
	clear();
	texmode(4);

	for(i=-10 ; i<11 ; i++)
	{
		mat4 t[2];
		int j=0;

		memcpy((void*)t[0],(const void*)floor_pview,sizeof(mat4));
		trans(t[0],v3(0.0f,(float)i*5.0f,0.0f));
		send_pview(t[0]);

		memcpy((void*)t[1],(const void*)t[0],sizeof(mat4));
		rotz(t[1],180.0f);

		draw_vbo(&models[0]);
		for(j=2 ; j<4 ; j++)
		{
			draw_vbo(&models[j]);
			send_pview(t[1]);
			draw_vbo(&models[j]);
			send_pview(t[0]);
		}

		draw_vbo(&models[5]);
		send_pview(t[1]);
		draw_vbo(&models[5]);
	}

	unuse_fbo();
	glActiveTexture(GL_TEXTURE4);
	use_tex(floor_dist.texs[0]);
	glActiveTexture(GL_TEXTURE0);

	//Reset the viewport
	glViewport(0,0,sw(),sh());
	glScissor(0,0,sw(),sh());

	//Actually draw the main scene
	clear();

	//Draw 50 corridors ahead and behind
	for(i=-50 ; i<51 ; i++)
	{
		mat4 t[2];
		int j=0;

		memcpy((void*)t[0],(const void*)pview,sizeof(mat4));
		trans(t[0],v3(0.0f,(float)i*5.0f,0.0f));
		send_pview(t[0]);

		memcpy((void*)t[1],(const void*)t[0],sizeof(mat4));
		rotz(t[1],180.0f);

		draw_vbo(&models[0]);
		//Change effect being used, check why 3 in the fragment shader
		texmode(3);
		draw_vbo(&models[1]);
		texmode(1);
		for(j=2 ; j<4 ; j++)
		{
			draw_vbo(&models[j]);
			send_pview(t[1]);
			draw_vbo(&models[j]);
			send_pview(t[0]);
		}
		//Change effect being used, check why 2 in the fragment shader
		texmode(2);
		draw_vbo(&models[5]);
		send_pview(t[1]);
		draw_vbo(&models[5]);
		texmode(1);
	}

	//Quit if escape is pressed
	if(kp(SDL_SCANCODE_ESCAPE)) quit();
}

//Free assets and resources
void done(void)
{
	done_fbo(&floor_fbo);
	done_fbo(&window_fbo);
	glDeleteTextures(1,&noise_tex);
	glDeleteTextures(1,&textures);
	done_vbo();
	done_basic();
}
