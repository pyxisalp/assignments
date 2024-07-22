//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2022 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"
#include <GL/glut.h>

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;

// for simple shaders
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color, loc_primitive_alpha; // indices of uniform variables

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 13
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_flag_diffuse_texture_mapping, loc_flag_normal_texture_mapping, loc_flag_emissive_texture_mapping, loc_flag_normal_based_directX;
GLint loc_flag_fog; int flag_fog;

// for skybox shaders
GLuint h_ShaderProgram_skybox;
GLint loc_cubemap_skybox;
GLint loc_ModelViewProjectionMatrix_SKY;

// texture id
#define TEXTURE_INDEX_DIFFUSE	(0)
#define TEXTURE_INDEX_NORMAL	(1)
#define TEXTURE_INDEX_SPECULAR	(2)
#define TEXTURE_INDEX_EMISSIVE	(3)
#define TEXTURE_INDEX_SKYMAP	(4)

#define TEXTURE_COW				(0)
#define TEXTURE_IRONMAN			(1)
#define TEXTURE_SPIDER			(2)
#define TEXTURE_BUS				(3)
#define N_TEXTURES_USED			(4)

GLuint texture_names[N_TEXTURES_USED];

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

GLint loc_diffuse_texture;
int prevX, prevY;
GLint loc_ModelViewProjectionMatrix_simple;
GLint loc_flag_texture_mapping;
GLuint h_ShaderProgram_TXPHI;
GLint loc_ModelViewProjectionMatrix_TXPHI, loc_ModelViewMatrix_TXPHI, loc_ModelViewMatrixInvTrans_TXPHI;
GLint loc_texture;
bool flag_texture_mapping_HI = true;
loc_Material_Parameters loc_material2;
GLint loc_global_ambient_color2;
loc_light_Parameters loc_light2[NUMBER_OF_LIGHT_SUPPORTED];
Light_Parameters light2[NUMBER_OF_LIGHT_SUPPORTED];
bool flag_ps = true;
bool flag_blend = false;
GLint loc_u_flag_blending;
float cuboid_alpha = 1.0f;
float color = 0.0f;
bool flag_bus_color = false;
bool flag_random_color = false;

loc_Material_Parameters loc_material3;
GLint loc_global_ambient_color3;
GLuint h_ShaderProgram_PS;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
GLint loc_screen_effect, loc_screen_frequency, loc_screen_width;
GLint loc_blind_effect;

loc_Material_Parameters loc_material4;
GLint loc_global_ambient_color4;
GLuint h_ShaderProgram_GS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
Light_Parameters light4[NUMBER_OF_LIGHT_SUPPORTED];
loc_light_Parameters loc_light4[NUMBER_OF_LIGHT_SUPPORTED];


int flag_draw_screen, flag_screen_effect;
float screen_frequency, screen_width;

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

// texture id
#define TEXTURE_ID_DIFFUSE	(0)
#define TEXTURE_ID_NORMAL	(1)

#define CAM_TSPEED 100.0f
#define CAM_RSPEED 0.1f



// char* -> const char*
void My_glTexImage2D_from_file(const char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	FreeImage_FlipVertical(tx_pixmap);

	fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);
}

int read_geometry_vnt(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);

	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

// for multiple materials
int read_geometry_vntm(GLfloat** object, int bytes_per_primitive,
	int* n_matrial_indicies, int** material_indicies,
	int* n_materials, char*** diffuse_texture_names,
	Material_Parameters** material_parameters,
	bool* bOnce,
	char* filename) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}

	int n_faces;
	fread(&n_faces, sizeof(int), 1, fp);

	*object = (float*)malloc(n_faces * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...\n", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_faces, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	fread(n_matrial_indicies, sizeof(int), 1, fp);

	int bytes_per_indices = sizeof(int) * 2;
	*material_indicies = (int*)malloc(bytes_per_indices * (*n_matrial_indicies)); // material id, offset
	if (*material_indicies == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...\n", filename);
		return -1;
	}

	fread(*material_indicies, bytes_per_indices, (*n_matrial_indicies), fp);

	if (*bOnce == false) {
		fread(n_materials, sizeof(int), 1, fp);

		*material_parameters = (Material_Parameters*)malloc(sizeof(Material_Parameters) * (*n_materials));
		*diffuse_texture_names = (char**)malloc(sizeof(char*) * (*n_materials));
		for (int i = 0; i < (*n_materials); i++) {
			fread((*material_parameters)[i].ambient_color, sizeof(float), 3, fp); //Ka
			fread((*material_parameters)[i].diffuse_color, sizeof(float), 3, fp); //Kd
			fread((*material_parameters)[i].specular_color, sizeof(float), 3, fp); //Ks
			fread(&(*material_parameters)[i].specular_exponent, sizeof(float), 1, fp); //Ns
			fread((*material_parameters)[i].emissive_color, sizeof(float), 3, fp); //Ke

			(*material_parameters)[i].ambient_color[3] = 1.0f;
			(*material_parameters)[i].diffuse_color[3] = 1.0f;
			(*material_parameters)[i].specular_color[3] = 1.0f;
			(*material_parameters)[i].emissive_color[3] = 1.0f;

			(*diffuse_texture_names)[i] = (char*)malloc(sizeof(char) * 256);
			fread((*diffuse_texture_names)[i], sizeof(char), 256, fp);
		}
		*bOnce = true;
	}

	fclose(fp);

	return n_faces;
}

void set_material(Material_Parameters* material_parameters) {
	glUniform4fv(loc_material2.ambient_color, 1, material_parameters->ambient_color);
	glUniform4fv(loc_material2.diffuse_color, 1, material_parameters->diffuse_color);
	glUniform4fv(loc_material2.specular_color, 1, material_parameters->specular_color);
	glUniform1f(loc_material2.specular_exponent, material_parameters->specular_exponent);
	glUniform4fv(loc_material2.emissive_color, 1, material_parameters->emissive_color);

	glUniform4fv(loc_material4.ambient_color, 1, material_parameters->ambient_color);
	glUniform4fv(loc_material4.diffuse_color, 1, material_parameters->diffuse_color);
	glUniform4fv(loc_material4.specular_color, 1, material_parameters->specular_color);
	glUniform1f(loc_material4.specular_exponent, material_parameters->specular_exponent);
	glUniform4fv(loc_material4.emissive_color, 1, material_parameters->emissive_color);
}

void bind_texture(GLuint tex, int glTextureId, GLuint texture_name) {
	glActiveTexture(GL_TEXTURE0 + glTextureId);
	glBindTexture(GL_TEXTURE_2D, texture_name);
	glUniform1i(tex, glTextureId);
}

/*********************************  START: camera *********************************/
typedef enum {
	CAMERA_1,
	CAMERA_2,
	CAMERA_3,
	CAMERA_4,
	CAMERA_5,
	CAMERA_6,
	NUM_CAMERAS
} CAMERA_INDEX;

typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move, rotation_axis;
} Camera;

Camera camera_info[NUM_CAMERAS];
Camera current_camera;

using glm::mat4;
void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	memcpy(&current_camera, pCamera, sizeof(Camera));
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void initialize_camera(void) {
	//CAMERA_1 : original view
	Camera* pCamera = &camera_info[CAMERA_1];
	for (int k = 0; k < 3; k++)
	{
		pCamera->pos[k] = scene.camera.e[k];
		pCamera->uaxis[k] = scene.camera.u[k];
		pCamera->vaxis[k] = scene.camera.v[k];
		pCamera->naxis[k] = scene.camera.n[k];
	}

	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_2 : bronze statue view
	pCamera = &camera_info[CAMERA_2];
	pCamera->pos[0] = -593.047974f; pCamera->pos[1] = -3758.460938f; pCamera->pos[2] = 474.587830f;
	pCamera->uaxis[0] = 0.864306f; pCamera->uaxis[1] = -0.502877f; pCamera->uaxis[2] = 0.009328f;
	pCamera->vaxis[0] = 0.036087f; pCamera->vaxis[1] = 0.080500f; pCamera->vaxis[2] = 0.996094f;
	pCamera->naxis[0] = -0.501662f; pCamera->naxis[1] = -0.860599f; pCamera->naxis[2] = 0.087724f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_3 : bronze statue view
	pCamera = &camera_info[CAMERA_3];
	pCamera->pos[0] = -1.463161f; pCamera->pos[1] = 1720.545166f; pCamera->pos[2] = 683.703491f;
	pCamera->uaxis[0] = -0.999413f; pCamera->uaxis[1] = -0.032568f; pCamera->uaxis[2] = -0.010066f;
	pCamera->vaxis[0] = -0.011190f; pCamera->vaxis[1] = -0.034529f; pCamera->vaxis[2] = 0.999328f;
	pCamera->naxis[0] = -0.032200f; pCamera->naxis[1] = 0.998855f; pCamera->naxis[2] = -0.034872f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_4 : top view
	pCamera = &camera_info[CAMERA_4];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 18300.0f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 1.0f; pCamera->vaxis[2] = 0.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 1.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_5 : front view
	pCamera = &camera_info[CAMERA_5];
	pCamera->pos[0] = 0.0f; pCamera->pos[1] = 11700.0f; pCamera->pos[2] = 0.0f;
	pCamera->uaxis[0] = 1.0f; pCamera->uaxis[1] = 0.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 0.0f; pCamera->naxis[1] = 1.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	//CAMERA_6 : side view
	pCamera = &camera_info[CAMERA_6];
	pCamera->pos[0] = 14600.0f; pCamera->pos[1] = 0.0f; pCamera->pos[2] = 0.0f;
	pCamera->uaxis[0] = 0.0f; pCamera->uaxis[1] = 1.0f; pCamera->uaxis[2] = 0.0f;
	pCamera->vaxis[0] = 0.0f; pCamera->vaxis[1] = 0.0f; pCamera->vaxis[2] = 1.0f;
	pCamera->naxis[0] = 1.0f; pCamera->naxis[1] = 0.0f; pCamera->naxis[2] = 0.0f;
	pCamera->move = 0;
	pCamera->fovy = TO_RADIAN * scene.camera.fovy, pCamera->aspect_ratio = scene.camera.aspect, pCamera->near_c = 0.1f; pCamera->far_c = 50000.0f;

	set_current_camera(CAMERA_1);
}
/*********************************  END: camera *********************************/

/******************************  START: shader setup ****************************/
// Begin of Callback function definitions
void prepare_shader_program(void) {
	char string[256];

	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram_simple);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_primitive_alpha = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_alpha");

	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_material.diffuseTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");
	loc_material.normalTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_normal_texture");
	loc_material.emissiveTex = glGetUniformLocation(h_ShaderProgram_TXPS, "u_emissive_texture");

	loc_flag_diffuse_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_diffuse_texture_mapping");
	loc_flag_normal_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_normal_texture_mapping");
	loc_flag_emissive_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_emissive_texture_mapping");
	loc_flag_normal_based_directX = glGetUniformLocation(h_ShaderProgram_TXPS, "u_normal_based_directX");

	ShaderInfo shader_info_TXPHI[3] = {
		{ GL_VERTEX_SHADER, "Shaders/exPhong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/exPhong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_TXPHI = LoadShaders(shader_info_TXPHI);
	loc_ModelViewProjectionMatrix_TXPHI = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPHI = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPHI = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color2 = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_global_ambient_color");
	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light2[%d].light_on", i);
		loc_light2[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].position", i);
		loc_light2[i].position = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].ambient_color", i);
		loc_light2[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].diffuse_color", i);
		loc_light2[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].specular_color", i);
		loc_light2[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].spot_direction", i);
		loc_light2[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].spot_exponent", i);
		loc_light2[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].spot_cutoff_angle", i);
		loc_light2[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
		sprintf(string, "u_light2[%d].light_attenuation_factors", i);
		loc_light2[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPHI, string);
	}

	loc_material2.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_material.ambient_color");
	loc_material2.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_material.diffuse_color");
	loc_material2.specular_color = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_material.specular_color");
	loc_material2.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_material.emissive_color");
	loc_material2.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_material.specular_exponent");

	loc_diffuse_texture = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPHI, "u_flag_texture_mapping");

	ShaderInfo shader_info_PS[3] = {
	{ GL_VERTEX_SHADER, "Shaders/Phong_sc.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong_sc.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color3 = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");

	loc_material3.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material3.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material3.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material3.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material3.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");

	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_PS, "screen_effect");
	loc_screen_frequency = glGetUniformLocation(h_ShaderProgram_PS, "screen_frequency");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_PS, "screen_width");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_blind_effect");

	ShaderInfo shader_info_GS[3] = {
	{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color4 = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light4[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light4[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light4[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light4[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light4[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light4[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light4[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light4[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light4[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}

	loc_material4.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material4.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material4.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material4.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material4.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

	ShaderInfo shader_info_skybox[3] = {
		{ GL_VERTEX_SHADER, "Shaders/skybox.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/skybox.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_skybox = LoadShaders(shader_info_skybox);
	loc_cubemap_skybox = glGetUniformLocation(h_ShaderProgram_skybox, "u_skymap");

	loc_ModelViewProjectionMatrix_SKY = glGetUniformLocation(h_ShaderProgram_skybox, "u_ModelViewProjectionMatrix");

}


/*******************************  END: shder setup ******************************/

/****************************  START: geometry setup ****************************/
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION	0
#define INDEX_NORMAL			1
#define INDEX_TEX_COORD			2

bool b_draw_grid = false;

//axes
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded axes into graphics memory.\n");
}

void draw_axes(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(8000.0f, 8000.0f, 8000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	glBindVertexArray(axes_VAO);
	glUniform1f(loc_primitive_alpha, 1.0f);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform1f(loc_primitive_alpha, 1.0f);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform1f(loc_primitive_alpha, 1.0f);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//grid
#define GRID_LENGTH			(100)
#define NUM_GRID_VETICES	((2 * GRID_LENGTH + 1) * 4)
GLuint grid_VBO, grid_VAO;
GLfloat grid_vertices[NUM_GRID_VETICES][3];
GLfloat grid_color[3] = { 0.5f, 0.5f, 0.5f };

void prepare_grid(void) {

	//set grid vertices
	int vertex_idx = 0;
	for (int x_idx = -GRID_LENGTH; x_idx <= GRID_LENGTH; x_idx++)
	{
		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = -GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	for (int y_idx = -GRID_LENGTH; y_idx <= GRID_LENGTH; y_idx++)
	{
		grid_vertices[vertex_idx][0] = -GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &grid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), &grid_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &grid_VAO);
	glBindVertexArray(grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	fprintf(stdout, " * Loaded grid into graphics memory.\n");
}

void draw_grid(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(1.0f);
	glBindVertexArray(grid_VAO);
	glUniform1f(loc_primitive_alpha, 1.0f);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_LINES, 0, NUM_GRID_VETICES);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//sun_temple
GLuint* sun_temple_VBO;
GLuint* sun_temple_VAO;
int* sun_temple_n_triangles;
int* sun_temple_vertex_offset;
GLfloat** sun_temple_vertices;
GLuint* sun_temple_texture_names;

bool* flag_texture_mapping;

bool readTexImage2D_from_file(const char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (tx_pixmap == NULL)
		return false;
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		//fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);

	return true;
}

void prepare_sun_temple(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO malloc
	sun_temple_VBO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);
	sun_temple_VAO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);

	sun_temple_n_triangles = (int*)malloc(sizeof(int) * scene.n_materials);
	sun_temple_vertex_offset = (int*)malloc(sizeof(int) * scene.n_materials);

	flag_texture_mapping = (bool*)malloc(sizeof(bool) * scene.n_textures);

	// vertices
	sun_temple_vertices = (GLfloat**)malloc(sizeof(GLfloat*) * scene.n_materials);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		MATERIAL* pMaterial = &(scene.material_list[materialIdx]);
		GEOMETRY_TRIANGULAR_MESH* tm = &(pMaterial->geometry.tm);

		// vertex
		sun_temple_vertices[materialIdx] = (GLfloat*)malloc(sizeof(GLfloat) * 8 * tm->n_triangle * 3);

		int vertexIdx = 0;
		for (int triIdx = 0; triIdx < tm->n_triangle; triIdx++) {
			TRIANGLE tri = tm->triangle_list[triIdx];
			for (int triVertex = 0; triVertex < 3; triVertex++) {
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].x;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].y;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].z;

				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].x;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].y;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].z;

				sun_temple_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].u;
				sun_temple_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].v;
			}
		}

		// # of triangles
		sun_temple_n_triangles[materialIdx] = tm->n_triangle;

		if (materialIdx == 0)
			sun_temple_vertex_offset[materialIdx] = 0;
		else
			sun_temple_vertex_offset[materialIdx] = sun_temple_vertex_offset[materialIdx - 1] + 3 * sun_temple_n_triangles[materialIdx - 1];

		glGenBuffers(1, &sun_temple_VBO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, sun_temple_VBO[materialIdx]);
		glBufferData(GL_ARRAY_BUFFER, sun_temple_n_triangles[materialIdx] * 3 * n_bytes_per_vertex,
			sun_temple_vertices[materialIdx], GL_STATIC_DRAW);

		// As the geometry data exists now in graphics memory, ...
		free(sun_temple_vertices[materialIdx]);

		// Initialize vertex array object.
		glGenVertexArrays(1, &sun_temple_VAO[materialIdx]);
		glBindVertexArray(sun_temple_VAO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, sun_temple_VBO[materialIdx]);
		glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_TEX_COORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if ((materialIdx > 0) && (materialIdx % 100 == 0))
			fprintf(stdout, " * Loaded %d sun temple materials into graphics memory.\n", materialIdx / 100 * 100);
	}
	fprintf(stdout, " * Loaded %d sun temple materials into graphics memory.\n", scene.n_materials);

	// textures
	sun_temple_texture_names = (GLuint*)malloc(sizeof(GLuint) * scene.n_textures);
	glGenTextures(scene.n_textures, sun_temple_texture_names);

	for (int texId = 0; texId < scene.n_textures; texId++) {
		glBindTexture(GL_TEXTURE_2D, sun_temple_texture_names[texId]);

		bool bReturn = readTexImage2D_from_file(scene.texture_file_name[texId]);

		if (bReturn) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			flag_texture_mapping[texId] = true;
		}
		else {
			flag_texture_mapping[texId] = false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	fprintf(stdout, " * Loaded sun temple textures into graphics memory.\n");

	free(sun_temple_vertices);
}

void bindTexture(GLuint tex, int glTextureId, int texId) {
	if (INVALID_TEX_ID != texId) {
		glActiveTexture(GL_TEXTURE0 + glTextureId);
		glBindTexture(GL_TEXTURE_2D, sun_temple_texture_names[texId]);
		glUniform1i(tex, glTextureId);
	}
}

void draw_sun_temple(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		// set material
		glUniform4fv(loc_material.ambient_color, 1, scene.material_list[materialIdx].shading.ph.ka);
		glUniform4fv(loc_material.diffuse_color, 1, scene.material_list[materialIdx].shading.ph.kd);
		glUniform4fv(loc_material.specular_color, 1, scene.material_list[materialIdx].shading.ph.ks);
		glUniform1f(loc_material.specular_exponent, scene.material_list[materialIdx].shading.ph.spec_exp);
		glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);

//		printf("%d\n", loc_material.ambient_color);

		int diffuseTexId = scene.material_list[materialIdx].diffuseTexId;
		int normalTexId = scene.material_list[materialIdx].normalMapTexId;
		int emissiveTexId = scene.material_list[materialIdx].emissiveTexId;

		bindTexture(loc_material.diffuseTex, TEXTURE_INDEX_DIFFUSE, diffuseTexId);
		glUniform1i(loc_flag_diffuse_texture_mapping, flag_texture_mapping[diffuseTexId]);
		bindTexture(loc_material.normalTex, TEXTURE_INDEX_NORMAL, normalTexId);
		glUniform1i(loc_flag_normal_texture_mapping, flag_texture_mapping[normalTexId]);
		glUniform1i(loc_flag_normal_based_directX, 1); // only for sun temple
		bindTexture(loc_material.emissiveTex, TEXTURE_INDEX_EMISSIVE, emissiveTexId);
		glUniform1i(loc_flag_emissive_texture_mapping, flag_texture_mapping[emissiveTexId]);

		glEnable(GL_TEXTURE_2D);

		glBindVertexArray(sun_temple_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * sun_temple_n_triangles[materialIdx]);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

// skybox
GLuint skybox_VBO, skybox_VAO;
GLuint skybox_texture_name;

GLfloat cube_vertices[72][3] = {
	// vertices enumerated clockwise
	  // 6*2*3 * 2 (POS & NORM)

	// position
	-1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f, //right
	 1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f, //left
	 1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //top
	 1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f, //bottom
	 1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f, //back
	-1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f, //front
	 1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,

	 // normal
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,
	 0.0f, 0.0f, -1.0f,      0.0f, 0.0f, -1.0f,     0.0f, 0.0f, -1.0f,

	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,
	-1.0f, 0.0f,  0.0f,     -1.0f, 0.0f,  0.0f,    -1.0f, 0.0f,  0.0f,

	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,
	 1.0f, 0.0f,  0.0f,      1.0f, 0.0f,  0.0f,     1.0f, 0.0f,  0.0f,

	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,

	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 0.0f,

	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f,
	 0.0f, -1.0f, 0.0f,      0.0f, -1.0f, 0.0f,     0.0f, -1.0f, 0.0f
};

void readTexImage2DForCubeMap(const char* filename, GLenum texture_target) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);

	width = FreeImage_GetWidth(tx_pixmap);
	height = FreeImage_GetHeight(tx_pixmap);
	FreeImage_FlipVertical(tx_pixmap);
	data = FreeImage_GetBits(tx_pixmap);

	glTexImage2D(texture_target, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap);
}

void prepare_skybox(void) { // Draw skybox.
	glGenVertexArrays(1, &skybox_VAO);
	glGenBuffers(1, &skybox_VBO);

	glBindVertexArray(skybox_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), &cube_vertices[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &skybox_texture_name);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	readTexImage2DForCubeMap("Scene/Cubemap/px.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/nx.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	readTexImage2DForCubeMap("Scene/Cubemap/py.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/ny.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	readTexImage2DForCubeMap("Scene/Cubemap/pz.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	readTexImage2DForCubeMap("Scene/Cubemap/nz.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	fprintf(stdout, " * Loaded cube map textures into graphics memory.\n\n");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void draw_skybox(void) {
	glUseProgram(h_ShaderProgram_skybox);

	glUniform1i(loc_cubemap_skybox, TEXTURE_INDEX_SKYMAP);

	ModelViewMatrix = ViewMatrix * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20000.0f, 20000.0f, 20000.0f));
	//ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(20000.0f, 20000.0f, 20000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_SKY, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(skybox_VAO);
	glActiveTexture(GL_TEXTURE0 + TEXTURE_INDEX_SKYMAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture_name);

	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 2 * 3);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glUseProgram(0);
}

// for animation
unsigned int timestamp_scene = 0; // the global clock in the scene
int flag_animation, flag_polygon_fill;
int cur_frame_spider = 0;

// cow object
GLuint cow_VBO, cow_VAO;
int cow_n_triangles;
GLfloat* cow_vertices;

Material_Parameters material_cow;

void prepare_cow(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, cow_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/cow_vnt.geom");
	cow_n_triangles = read_geometry_vnt(&cow_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	cow_n_total_triangles += cow_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &cow_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glBufferData(GL_ARRAY_BUFFER, cow_n_total_triangles * 3 * n_bytes_per_vertex, cow_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(cow_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &cow_VAO);
	glBindVertexArray(cow_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_cow.ambient_color[0] = 0.5f;
	material_cow.ambient_color[1] = 0.5f;
	material_cow.ambient_color[2] = 0.5f;
	material_cow.ambient_color[3] = 1.0f;

	material_cow.diffuse_color[0] = 0.7f;
	material_cow.diffuse_color[1] = 0.5f;
	material_cow.diffuse_color[2] = 0.2f;
	material_cow.diffuse_color[3] = 1.0f;

	material_cow.specular_color[0] = 0.4f;
	material_cow.specular_color[1] = 0.4f;
	material_cow.specular_color[2] = 0.2f;
	material_cow.specular_color[3] = 1.0f;

	material_cow.specular_exponent = 5.334717f;

	material_cow.emissive_color[0] = 0.000000f;
	material_cow.emissive_color[1] = 0.000000f;
	material_cow.emissive_color[2] = 0.000000f;
	material_cow.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_COW]);

	My_glTexImage2D_from_file("Data/static_objects/Cow.png");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_cow(void) {
	set_material(&material_cow);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_COW]);
	glFrontFace(GL_CW);
	glBindVertexArray(cow_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * cow_n_triangles);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// ironman object
GLuint ironman_VBO, ironman_VAO;
int ironman_n_triangles;
GLfloat* ironman_vertices;

Material_Parameters material_ironman;

void prepare_ironman(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, ironman_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/ironman_vnt.geom");
	ironman_n_triangles = read_geometry_vnt(&ironman_vertices, n_bytes_per_triangle, filename);
	ironman_n_triangles = read_geometry_vnt(&ironman_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ironman_n_total_triangles += ironman_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &ironman_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glBufferData(GL_ARRAY_BUFFER, ironman_n_total_triangles * 3 * n_bytes_per_vertex, ironman_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ironman_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ironman_VAO);
	glBindVertexArray(ironman_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_ironman.ambient_color[0] = 0.4f;
	material_ironman.ambient_color[1] = 0.4f;
	material_ironman.ambient_color[2] = 0.4f;
	material_ironman.ambient_color[3] = 1.0f;

	material_ironman.diffuse_color[0] = 0.900000f;
	material_ironman.diffuse_color[1] = 0.200000f;
	material_ironman.diffuse_color[2] = 0.200000f;
	material_ironman.diffuse_color[3] = 1.0f;

	material_ironman.specular_color[0] = 1.0f;
	material_ironman.specular_color[1] = 1.0f;
	material_ironman.specular_color[2] = 1.0f;
	material_ironman.specular_color[3] = 1.0f;

	material_ironman.specular_exponent = 52.334717f;

	material_ironman.emissive_color[0] = 0.000000f;
	material_ironman.emissive_color[1] = 0.000000f;
	material_ironman.emissive_color[2] = 0.000000f;
	material_ironman.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_IRONMAN]);

	My_glTexImage2D_from_file("Shaders/sea.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_ironman(void) {
	set_material(&material_ironman);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_IRONMAN]);
	glFrontFace(GL_CW);
	glBindVertexArray(ironman_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ironman_n_triangles);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// spider object
#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

Material_Parameters material_spider;

void prepare_spider(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/dynamic_objects/spider/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry_vnt(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_spider.ambient_color[0] = 0.5f;
	material_spider.ambient_color[1] = 0.5f;
	material_spider.ambient_color[2] = 0.5f;
	material_spider.ambient_color[3] = 1.0f;

	material_spider.diffuse_color[0] = 0.9f;
	material_spider.diffuse_color[1] = 0.5f;
	material_spider.diffuse_color[2] = 0.1f;
	material_spider.diffuse_color[3] = 1.0f;

	material_spider.specular_color[0] = 0.5f;
	material_spider.specular_color[1] = 0.5f;
	material_spider.specular_color[2] = 0.5f;
	material_spider.specular_color[3] = 1.0f;

	material_spider.specular_exponent = 11.334717f;

	material_spider.emissive_color[0] = 0.000000f;
	material_spider.emissive_color[1] = 0.000000f;
	material_spider.emissive_color[2] = 0.000000f;
	material_spider.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_SPIDER]);

	My_glTexImage2D_from_file("Shaders/xthesky.jpg");

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_spider(void) {
	set_material(&material_spider);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_SPIDER]);
	glFrontFace(GL_CW);
	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// bus object
GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;

Material_Parameters material_bus;

void prepare_bus(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/static_objects/bus_vnt.geom");
	bus_n_triangles = read_geometry_vnt(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;

	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_bus.ambient_color[0] = 0.4f;
	material_bus.ambient_color[1] = 0.4f;
	material_bus.ambient_color[2] = 0.4f;
	material_bus.ambient_color[3] = 1.0f;

	material_bus.diffuse_color[0] = 0.7f;
	material_bus.diffuse_color[1] = 0.5f;
	material_bus.diffuse_color[2] = 0.2f;
	material_bus.diffuse_color[3] = 1.0f;

	/*
		material_bus.diffuse_color[0] = 0.96862f;
	material_bus.diffuse_color[1] = 0.90980f;
	material_bus.diffuse_color[2] = 0.79607f;
	material_bus.diffuse_color[3] = 1.0f;
	*/

	material_bus.specular_color[0] = 0.5f;
	material_bus.specular_color[1] = 0.5f;
	material_bus.specular_color[2] = 0.5f;
	material_bus.specular_color[3] = 1.0f;

	material_bus.specular_exponent = 50.334717f;

	material_bus.emissive_color[0] = 0.000000f;
	material_bus.emissive_color[1] = 0.000000f;
	material_bus.emissive_color[2] = 0.000000f;
	material_bus.emissive_color[3] = 1.0f;

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, texture_names[TEXTURE_BUS]);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_bus(void) {
	set_material(&material_bus);
	bind_texture(loc_diffuse_texture, TEXTURE_ID_DIFFUSE, texture_names[TEXTURE_BUS]);
	glFrontFace(GL_CW);
	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// cuboid
GLuint cuboid_VBO, cuboid_VAO;
GLfloat cuboid_vertices[8][3] = {
	{ -1.0f, -0.5f, -0.5f }, { 1.0f, -0.5f, -0.5f }, 
	{ 1.0f,  0.5f, -0.5f },	{ -1.0f,  0.5f, -0.5f },
	{ -1.0f, -0.5f,  0.5f }, { 1.0f, -0.5f,  0.5f },
	{ 1.0f,  0.5f,  0.5f },	{ -1.0f,  0.5f,  0.5f }
};
GLushort cuboid_indices[36] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	0, 1, 5, 5, 4, 0,
	2, 3, 7, 7, 6, 2,
	0, 3, 7, 7, 4, 0,
	1, 2, 6, 6, 5, 1
};
GLfloat cuboid_color[3] = { 0.0f, 0.0f, 0.0f };

void prepare_cuboid(void) {
	glGenBuffers(1, &cuboid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, cuboid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cuboid_vertices), &cuboid_vertices[0][0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &cuboid_VAO);
	glBindVertexArray(cuboid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cuboid_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded cube into graphics memory.\n");
}

void draw_cuboid(void) {
	glUseProgram(h_ShaderProgram_simple);
	glFrontFace(GL_CCW);

	glLineWidth(2.0f);
	glBindVertexArray(cuboid_VAO);
	glUniform1f(loc_primitive_alpha, cuboid_alpha);
	glUniform3fv(loc_primitive_color, 1, cuboid_color);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, cuboid_indices);
	glBindVertexArray(0);
	glLineWidth(1.0f);

	glUseProgram(0);
}

void back_to_front(void) {
	glUseProgram(h_ShaderProgram_simple);

	if (flag_blend) {
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1i(loc_u_flag_blending, 1);
	}
	else glUniform1i(loc_u_flag_blending, 0);

	glEnable(GL_CULL_FACE);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 4000.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, (timestamp_scene % 360) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glCullFace(GL_FRONT);
	draw_cuboid();
	glCullFace(GL_BACK);
	draw_cuboid();
	glDisable(GL_CULL_FACE);

	glUseProgram(0);
}

/*****************************  END: geometry setup *****************************/

/********************************  START: light *********************************/
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

void initialize_lights(void) { // follow OpenGL conventions for initialization
	for (int i = 0; i < scene.n_lights; i++) {
		light[i].light_on = 1;

		if (LIGHT_DIRECTIONAL == scene.light_list[i].type) {
			light[i].position[0] = scene.light_list[i].pos[0];
			light[i].position[1] = scene.light_list[i].pos[1];
			light[i].position[2] = scene.light_list[i].pos[2];
			light[i].position[3] = 0.0f;

			light[i].ambient_color[0] = 0.0f;
			light[i].ambient_color[1] = 0.0f;
			light[i].ambient_color[2] = 0.0f;
			light[i].ambient_color[3] = 1.0f;

			light[i].diffuse_color[0] = scene.light_list[i].color[0];
			light[i].diffuse_color[1] = scene.light_list[i].color[1];
			light[i].diffuse_color[2] = scene.light_list[i].color[2];
			light[i].diffuse_color[3] = 1.0f;

			light[i].specular_color[0] = 0.5f; 
			light[i].specular_color[1] = 0.5f;
			light[i].specular_color[2] = 0.5f; 
			light[i].specular_color[3] = 1.0f;
		}
		else if (LIGHT_POINT == scene.light_list[i].type) {
			light[i].position[0] = scene.light_list[i].pos[0];
			light[i].position[1] = scene.light_list[i].pos[1];
			light[i].position[2] = scene.light_list[i].pos[2];
			light[i].position[3] = scene.light_list[i].pos[3];

			light[i].ambient_color[0] = 0.0f;
			light[i].ambient_color[1] = 0.0f;
			light[i].ambient_color[2] = 0.0f;
			light[i].ambient_color[3] = 1.0f;

			light[i].diffuse_color[0] = scene.light_list[i].color[0];
			light[i].diffuse_color[1] = scene.light_list[i].color[1];
			light[i].diffuse_color[2] = scene.light_list[i].color[2];
			light[i].diffuse_color[3] = 1.0f;

			light[i].specular_color[0] = 0.8f; 
			light[i].specular_color[1] = 0.8f;
			light[i].specular_color[2] = 0.8f; 
			light[i].specular_color[3] = 1.0f;

			light[i].light_attenuation_factors[0] = 1.0f;
			light[i].light_attenuation_factors[1] = 0.01;
			light[i].light_attenuation_factors[2] = 0.0f;
			light[i].light_attenuation_factors[3] = 1.0f;

			light[i].spot_cutoff_angle = 180.0f;
		}
		else {
			// for spot light, volume light, ...
		}
	}
}

void set_lights(void) {
	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.3f, 0.3f, 0.3f, 1.0f);

	glm::vec4 light_position_EC;
	glm::vec3 light_direction_EC;
	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, light[i].light_on);

		light_position_EC = ViewMatrix * glm::vec4(light[i].position[0], light[i].position[1], light[i].position[2], light[i].position[3]);
		glUniform4fv(loc_light[i].position, 1, &light_position_EC[0]);

		glUniform4fv(loc_light[i].ambient_color, 1, light[i].ambient_color);
		glUniform4fv(loc_light[i].diffuse_color, 1, light[i].diffuse_color);
		glUniform4fv(loc_light[i].specular_color, 1, light[i].specular_color);

		if (0.0f != light[i].position[3])
		{
			light_direction_EC = glm::transpose(glm::inverse(glm::mat3(ViewMatrix))) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1], light[i].spot_direction[2]);
			glUniform3fv(loc_light[i].spot_direction, 1, &light_direction_EC[0]);
			glUniform1f(loc_light[i].spot_exponent, light[i].spot_exponent);
			glUniform1f(loc_light[i].spot_cutoff_angle, light[i].spot_cutoff_angle);
			glUniform4fv(loc_light[i].light_attenuation_factors, 1, light[i].light_attenuation_factors);
		}
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPHI);

	glUniform4f(loc_global_ambient_color2, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light2[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light2[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light2[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light2[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light2[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light2[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light2[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light2[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light2[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light2[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light2[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material2.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material2.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material2.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material2.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material2.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);

	glUseProgram(h_ShaderProgram_PS);

	glUniform4f(loc_global_ambient_color3, 0.2f, 0.2f, 0.2f, 1.0f);

	glUniform4f(loc_material3.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material3.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material3.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material3.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material3.specular_exponent, 0.0f); // [0.0, 128.0]

	glUniform1i(loc_screen_effect, 0);
	glUniform1f(loc_screen_frequency, 2.0f);
	glUniform1f(loc_screen_width, 0.125f);

	glUniform1i(loc_blind_effect, 0);

	glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color4, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light4[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light4[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light4[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light4[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light4[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light4[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light4[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light4[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light4[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light4[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light4[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material4.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material4.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material4.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material4.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material4.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light2[0].light_on = 1;
	light2[0].position[0] = 0.0f; light2[0].position[1] = 100.0f; 	// point light position in EC
	light2[0].position[2] = 0.0f; light2[0].position[3] = 1.0f;

	light2[0].ambient_color[0] = 0.13f; light2[0].ambient_color[1] = 0.13f;
	light2[0].ambient_color[2] = 0.13f; light2[0].ambient_color[3] = 1.0f;

	light2[0].diffuse_color[0] = 0.5f; light2[0].diffuse_color[1] = 0.5f;
	light2[0].diffuse_color[2] = 0.5f; light2[0].diffuse_color[3] = 1.5f;

	light2[0].specular_color[0] = 0.8f; light2[0].specular_color[1] = 0.8f;
	light2[0].specular_color[2] = 0.8f; light2[0].specular_color[3] = 1.0f;

	light2[1].light_on = 1;
	light2[1].position[0] = 0.0f; light2[1].position[1] = 0.0f; // spot light position in WC
	light2[1].position[2] = 0.0f; light2[1].position[3] = 1.0f;

	light2[1].ambient_color[0] = 0.152f; light2[1].ambient_color[1] = 0.0f;
	light2[1].ambient_color[2] = 0.0f; light2[1].ambient_color[3] = 1.0f;

	light2[1].diffuse_color[0] = 0.572f; light2[1].diffuse_color[1] = 0.0f;
	light2[1].diffuse_color[2] = 0.0f; light2[1].diffuse_color[3] = 1.0f;

	light2[1].specular_color[0] = 0.772f; light2[1].specular_color[1] = 0.0f;
	light2[1].specular_color[2] = 0.0f; light2[1].specular_color[3] = 1.0f;

	light2[1].spot_direction[0] = 0.0f; light2[1].spot_direction[1] = 0.0f; // spot light direction in WC
	light2[1].spot_direction[2] = -1.0f;
	light2[1].spot_cutoff_angle = 20.0f;
	light2[1].spot_exponent = 8.0f;

	light2[2].light_on = 1;
	light2[2].position[0] = 0.0f; light2[2].position[1] = -2500.0f; // spot light position in WC
	light2[2].position[2] = 2300.0f; light2[2].position[3] = 1.0f;

	light2[2].ambient_color[0] = 0.0f; light2[2].ambient_color[1] = 0.0f;
	light2[2].ambient_color[2] = 0.152f; light2[2].ambient_color[3] = 1.0f;

	light2[2].diffuse_color[0] = 0.0f; light2[2].diffuse_color[1] = 0.0f;
	light2[2].diffuse_color[2] = 0.572f; light2[2].diffuse_color[3] = 1.0f;

	light2[2].specular_color[0] = 0.0f; light2[2].specular_color[1] = 0.0f;
	light2[2].specular_color[2] = 0.772f; light2[2].specular_color[3] = 1.0f;

	light2[2].spot_direction[0] = 0.0f; light2[2].spot_direction[1] = 0.0f; // spot light direction in WC
	light2[2].spot_direction[2] = -1.0f;
	light2[2].spot_cutoff_angle = 20.0f;
	light2[2].spot_exponent = 8.0f;

	light2[3].light_on = 1;
	light2[3].position[0] = 0.0f; light2[3].position[1] = 0.0f; 	// point light position in EC
	light2[3].position[2] = 100.0f; light2[3].position[3] = 1.0f;

	light2[3].ambient_color[0] = 0.0f; light2[3].ambient_color[1] = 0.13f;
	light2[3].ambient_color[2] = 0.0f; light2[3].ambient_color[3] = 1.0f;

	light2[3].diffuse_color[0] = 0.0f; light2[3].diffuse_color[1] = 0.5f;
	light2[3].diffuse_color[2] = 0.0f; light2[3].diffuse_color[3] = 1.5f;

	light2[3].specular_color[0] = 0.0f; light2[3].specular_color[1] = 0.8f;
	light2[3].specular_color[2] = 0.0f; light2[3].specular_color[3] = 1.0f;

	glUseProgram(h_ShaderProgram_TXPHI);
	glUniform1i(loc_light2[0].light_on, light2[0].light_on);
	glUniform4fv(loc_light2[0].position, 1, light2[0].position);
	glUniform4fv(loc_light2[0].ambient_color, 1, light2[0].ambient_color);
	glUniform4fv(loc_light2[0].diffuse_color, 1, light2[0].diffuse_color);
	glUniform4fv(loc_light2[0].specular_color, 1, light2[0].specular_color);

	glUniform1i(loc_light2[1].light_on, light2[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light2[1].position[0], light2[1].position[1],
		light2[1].position[2], light2[1].position[3]);
	glUniform4fv(loc_light2[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light2[1].ambient_color, 1, light2[1].ambient_color);
	glUniform4fv(loc_light2[1].diffuse_color, 1, light2[1].diffuse_color);
	glUniform4fv(loc_light2[1].specular_color, 1, light2[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light2[1].spot_direction[0], light2[1].spot_direction[1],
		light2[1].spot_direction[2]);
	glUniform3fv(loc_light2[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light2[1].spot_cutoff_angle, light2[1].spot_cutoff_angle);
	glUniform1f(loc_light2[1].spot_exponent, light2[1].spot_exponent);

	glUniform1i(loc_light2[2].light_on, light2[2].light_on);
	glUniform4fv(loc_light2[2].position, 1, light2[2].position);
	glUniform4fv(loc_light2[2].ambient_color, 1, light2[2].ambient_color);
	glUniform4fv(loc_light2[2].diffuse_color, 1, light2[2].diffuse_color);
	glUniform4fv(loc_light2[2].specular_color, 1, light2[2].specular_color);
	glUniform3fv(loc_light2[2].spot_direction, 1, light2[2].spot_direction);
	glUniform1f(loc_light2[2].spot_cutoff_angle, light2[2].spot_cutoff_angle);
	glUniform1f(loc_light2[2].spot_exponent, light2[2].spot_exponent);

	glUniform1i(loc_light2[3].light_on, light2[3].light_on);
	glUniform4fv(loc_light2[3].position, 1, light2[3].position);
	glUniform4fv(loc_light2[3].ambient_color, 1, light2[3].ambient_color);
	glUniform4fv(loc_light2[3].diffuse_color, 1, light2[3].diffuse_color);
	glUniform4fv(loc_light2[3].specular_color, 1, light2[3].specular_color);

	glUseProgram(0);

	light4[0].light_on = 1;
	light4[0].position[0] = 0.0f; light4[0].position[1] = 100.0f; 	// point light position in EC
	light4[0].position[2] = 0.0f; light4[0].position[3] = 1.0f;

	light4[0].ambient_color[0] = 0.13f; light4[0].ambient_color[1] = 0.13f;
	light4[0].ambient_color[2] = 0.13f; light4[0].ambient_color[3] = 1.0f;

	light4[0].diffuse_color[0] = 0.5f; light4[0].diffuse_color[1] = 0.5f;
	light4[0].diffuse_color[2] = 0.5f; light4[0].diffuse_color[3] = 1.5f;

	light4[0].specular_color[0] = 0.8f; light4[0].specular_color[1] = 0.8f;
	light4[0].specular_color[2] = 0.8f; light4[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light4[1].light_on = 1;
	light4[1].position[0] = -200.0f; light4[1].position[1] = 500.0f; // spot light position in WC
	light4[1].position[2] = -200.0f; light4[1].position[3] = 1.0f;

	light4[1].ambient_color[0] = 0.2f; light4[1].ambient_color[1] = 0.2f;
	light4[1].ambient_color[2] = 0.2f; light4[1].ambient_color[3] = 1.0f;

	light4[1].diffuse_color[0] = 0.82f; light4[1].diffuse_color[1] = 0.82f;
	light4[1].diffuse_color[2] = 0.82f; light4[1].diffuse_color[3] = 1.0f;

	light4[1].specular_color[0] = 0.82f; light4[1].specular_color[1] = 0.82f;
	light4[1].specular_color[2] = 0.82f; light4[1].specular_color[3] = 1.0f;

	light4[1].spot_direction[0] = 0.0f; light4[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light4[1].spot_direction[2] = 0.0f;
	light4[1].spot_cutoff_angle = 20.0f;
	light4[1].spot_exponent = 20.0f;

	glUseProgram(h_ShaderProgram_GS);
	glUniform1i(loc_light4[0].light_on, light4[0].light_on);
	glUniform4fv(loc_light4[0].position, 1, light4[0].position);
	glUniform4fv(loc_light4[0].ambient_color, 1, light4[0].ambient_color);
	glUniform4fv(loc_light4[0].diffuse_color, 1, light4[0].diffuse_color);
	glUniform4fv(loc_light4[0].specular_color, 1, light4[0].specular_color);

	glUseProgram(0);
}


/*********************************  END: light **********************************/

void translateCamera(int sign, char axis) {
	switch (axis) {
	case 'u':
		current_camera.pos[0] += CAM_TSPEED * sign * (current_camera.uaxis[0]);
		current_camera.pos[1] += CAM_TSPEED * sign * (current_camera.uaxis[1]);
		current_camera.pos[2] += CAM_TSPEED * sign * (current_camera.uaxis[2]);
		break;
	case 'v':
		current_camera.pos[0] += CAM_TSPEED * sign * (current_camera.vaxis[0]);
		current_camera.pos[1] += CAM_TSPEED * sign * (current_camera.vaxis[1]);
		current_camera.pos[2] += CAM_TSPEED * sign * (current_camera.vaxis[2]);
		break;
	case 'n':
		current_camera.pos[0] += CAM_TSPEED * sign * (-current_camera.naxis[0]);
		current_camera.pos[1] += CAM_TSPEED * sign * (-current_camera.naxis[1]);
		current_camera.pos[2] += CAM_TSPEED * sign * (-current_camera.naxis[2]);
		break;
	}

	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void rotateCamera(int angle, char axis) {
	glm::mat3 rotationMatrix;
	glm::vec3 direction;

	switch (axis) {
	case 'u':
		rotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_RSPEED * TO_RADIAN * angle, glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2])));
		direction = rotationMatrix * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
		current_camera.vaxis[0] = direction.x, current_camera.vaxis[1] = direction.y, current_camera.vaxis[2] = direction.z;
		direction = rotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
		current_camera.naxis[0] = direction.x, current_camera.naxis[1] = direction.y, current_camera.naxis[2] = direction.z;
		break;
	case 'v':
		rotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_RSPEED * TO_RADIAN * angle, glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2])));
		direction = rotationMatrix * glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
		current_camera.naxis[0] = direction.x, current_camera.naxis[1] = direction.y, current_camera.naxis[2] = direction.z;
		direction = rotationMatrix * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
		current_camera.uaxis[0] = direction.x, current_camera.uaxis[1] = direction.y, current_camera.uaxis[2] = direction.z;
		break;
	case 'n':
		rotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_RSPEED * TO_RADIAN * angle, glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2])));
		direction = rotationMatrix * glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
		current_camera.uaxis[0] = direction.x, current_camera.uaxis[1] = direction.y, current_camera.uaxis[2] = direction.z;
		direction = rotationMatrix * glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
		current_camera.vaxis[0] = direction.x, current_camera.vaxis[1] = direction.y, current_camera.vaxis[2] = direction.z;
		break;
	}

	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void moveSpider(void) {
	unsigned int timestampSpider = timestamp_scene % 1170;

	if (timestampSpider <= 360) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 230.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, ((timestampSpider + 90) % 360) * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(500.0f, 0.0f, 0.0f));
	}

	else if (timestampSpider > 1030) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, (signed)(timestampSpider - 1030) * 10 - 1900.0f, 230.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 975) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3((signed)(timestampSpider - 975) * 10 - 550.0f, -1900.0f, 230.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 950) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-550.0f, (signed)(timestampSpider - 950) * 10 - 2150.0f, (signed)(timestampSpider - 950) * 10 - 20.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 820) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-550.0f, (signed)(timestampSpider - 820) * 10 - 3450.0f, -20.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 710) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-(signed)(timestampSpider - 710) * 10 + 550, -3450.0f, -20.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 580) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(550, -(signed)(timestampSpider - 580) * 10 - 2150.0f, -20.0f));
	}

	else if (timestampSpider > 555) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(550, -(signed)(timestampSpider - 555) * 10 - 1900.0f, -(signed)(timestampSpider - 555) * 10 + 230.0f));
	}

	else if (timestampSpider > 500) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3((signed)(timestampSpider - 500) * 10, -1900.0f, 230.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * (-TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	else if (timestampSpider > 360) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -(signed)(timestampSpider - 360) * 10 - 500.0f, 230.0f));
	}



	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0, 50.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glm::vec4 position_EC = ModelViewMatrix * glm::vec4(light2[3].position[0], light2[3].position[1],
		light2[3].position[2], light2[3].position[3]);
	glUniform4fv(loc_light2[3].position, 1, &position_EC[0]);

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPHI, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPHI, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPHI, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_spider();
}

// rectangle object
GLuint rectangle_VBO, rectangle_VAO;

/*
GLfloat rectangle_vertices[12][3] = {  // vertices enumerated counterclockwise
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
*/


GLfloat rectangle_vertices[12][3] = {  // vertices enumerated counterclockwise
	{ -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f }
};

Material_Parameters material_floor;
Material_Parameters material_screen;

void prepare_rectangle(void) { // Draw coordinate axes.
	// Initialize vertex buffer object.
	glGenBuffers(1, &rectangle_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &rectangle_VAO);
	glBindVertexArray(rectangle_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	material_floor.ambient_color[0] = 0.0f;
	material_floor.ambient_color[1] = 0.05f;
	material_floor.ambient_color[2] = 0.0f;
	material_floor.ambient_color[3] = 1.0f;

	material_floor.diffuse_color[0] = 0.4f;
	material_floor.diffuse_color[1] = 0.5f;
	material_floor.diffuse_color[2] = 0.4f;
	material_floor.diffuse_color[3] = 1.0f;

	material_floor.specular_color[0] = 0.04f;
	material_floor.specular_color[1] = 0.7f;
	material_floor.specular_color[2] = 0.04f;
	material_floor.specular_color[3] = 1.0f;

	material_floor.specular_exponent = 2.5f;

	material_floor.emissive_color[0] = 0.0f;
	material_floor.emissive_color[1] = 0.0f;
	material_floor.emissive_color[2] = 0.0f;
	material_floor.emissive_color[3] = 1.0f;

	material_screen.ambient_color[0] = 0.1745f;
	material_screen.ambient_color[1] = 0.0117f;
	material_screen.ambient_color[2] = 0.0117f;
	material_screen.ambient_color[3] = 1.0f;

	material_screen.diffuse_color[0] = 0.6142f;
	material_screen.diffuse_color[1] = 0.0413f;
	material_screen.diffuse_color[2] = 0.0413;
	material_screen.diffuse_color[3] = 1.0f;

	material_screen.specular_color[0] = 0.7278f;
	material_screen.specular_color[1] = 0.6269f;
	material_screen.specular_color[2] = 0.6269f;
	material_screen.specular_color[3] = 1.0f;

	material_screen.specular_exponent = 20.5f;

	material_screen.emissive_color[0] = 0.0f;
	material_screen.emissive_color[1] = 0.0f;
	material_screen.emissive_color[2] = 0.0f;
	material_screen.emissive_color[3] = 1.0f;
}

void set_material_floor(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(loc_material.ambient_color, 1, material_floor.ambient_color);
	glUniform4fv(loc_material.diffuse_color, 1, material_floor.diffuse_color);
	glUniform4fv(loc_material.specular_color, 1, material_floor.specular_color);
	glUniform1f(loc_material.specular_exponent, material_floor.specular_exponent);
	glUniform4fv(loc_material.emissive_color, 1, material_floor.emissive_color);
}

void draw_floor(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void set_material_screen(void) {
	// assume ShaderProgram_PS is used
	glUniform4fv(loc_material3.ambient_color, 1, material_screen.ambient_color);
	glUniform4fv(loc_material3.diffuse_color, 1, material_screen.diffuse_color);
	glUniform4fv(loc_material3.specular_color, 1, material_screen.specular_color);
	glUniform1f(loc_material3.specular_exponent, material_screen.specular_exponent);
	glUniform4fv(loc_material3.emissive_color, 1, material_screen.emissive_color);
}

void draw_screen(void) {
	glFrontFace(GL_CCW);

	glBindVertexArray(rectangle_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void initialize_screen(void) {
	flag_draw_screen = flag_screen_effect = 0;
	screen_frequency = 2.0f;
	screen_width = 0.125f;
}

/********************  START: callback function definitions *********************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	set_lights();

	draw_grid();
	draw_axes();
	draw_skybox();
	draw_sun_temple();


	if (flag_ps) glUseProgram(h_ShaderProgram_TXPHI);
	else glUseProgram(h_ShaderProgram_GS);

	glUniform1i(loc_flag_texture_mapping, false);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, -2500.0f, 1800.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (flag_ps) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPHI, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPHI, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPHI, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}

	draw_cow();

	glUseProgram(h_ShaderProgram_TXPHI);

	glUniform1i(loc_flag_texture_mapping, true);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 3000.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPHI, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPHI, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPHI, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_ironman();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 3000.0f, 5000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPHI, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPHI, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPHI, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_bus();

	moveSpider();

	glUseProgram(0);

	if (flag_draw_screen) {
		glUseProgram(h_ShaderProgram_PS);

		set_material_screen();
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 4500.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (timestamp_scene % 360) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(250.0f, 250.0f, 250.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

		glUniform1i(loc_screen_effect, flag_screen_effect);
		draw_screen();
		glUniform1i(loc_screen_effect, 0);
		glUseProgram(0);
	}

	if (flag_blend) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		back_to_front();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutSwapBuffers();

}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	if (flag_bus_color) {
		color += 0.002f;
		if (color > 1.0f)
			color = 0.0f;
		material_bus.diffuse_color[0] = color;
		material_bus.diffuse_color[1] = color;
		material_bus.diffuse_color[2] = color;
	}
	if (flag_random_color) {
		glUseProgram(h_ShaderProgram_TXPHI);
		light2[2].ambient_color[0] = rand() / 32767.0f;
		light2[2].ambient_color[1] = rand() / 32767.0f;
		light2[2].ambient_color[2] = rand() / 32767.0f;
		glUniform4fv(loc_light2[2].ambient_color, 1, light2[2].ambient_color);
		glUseProgram(0);
	}
	glutPostRedisplay();
	glutTimerFunc(25, timer_scene, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		glutPostRedisplay();
		break;
	case '1':
		set_current_camera(CAMERA_1);
		glutPostRedisplay();
		break;
	case '2':
		set_current_camera(CAMERA_2);
		glutPostRedisplay();
		break;
	case '3':
		set_current_camera(CAMERA_3);
		glutPostRedisplay();
		break;
	case '4':
		set_current_camera(CAMERA_4);
		glutPostRedisplay();
		break;
	case '5':
		set_current_camera(CAMERA_5);
		glutPostRedisplay();
		break;
	case '6':
		set_current_camera(CAMERA_6);
		glutPostRedisplay();
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'q':
		flag_ps = !flag_ps;
		glutPostRedisplay();
		break;
	case 'w':
		glUseProgram(h_ShaderProgram_TXPHI);
		light2[2].light_on = 1 - light2[2].light_on;
		glUniform1i(loc_light2[2].light_on, light2[2].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'e':
		glUseProgram(h_ShaderProgram_TXPHI);
		light2[1].light_on = 1 - light2[1].light_on;
		glUniform1i(loc_light2[1].light_on, light2[1].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'r':
		glUseProgram(h_ShaderProgram_TXPHI);
		light2[3].light_on = 1 - light2[3].light_on;
		glUniform1i(loc_light2[3].light_on, light2[3].light_on);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
		flag_blend = !flag_blend;
		glutPostRedisplay();
		break;
	case 'a':
		if (flag_blend) {
			cuboid_alpha += 0.1f;
			if (cuboid_alpha > 1.0f)
				cuboid_alpha = 1.0f;
			glutPostRedisplay();
		}
		break;
	case 's':
		if (flag_blend) {
			cuboid_alpha -= 0.1f;
			if (cuboid_alpha < 0.0f)
				cuboid_alpha = 0.0f;
			glutPostRedisplay();
		}
		break;
	case 'y':
		flag_draw_screen = 1 - flag_draw_screen;
		glutPostRedisplay();
		break;
	case 'u':
		if (flag_draw_screen) {
			flag_screen_effect = 1 - flag_screen_effect;
			glutPostRedisplay();
		}
		break;
#define SCEEN_MAX_FREQUENCY 50.0f
	case 'g':
		if (flag_draw_screen) {
			screen_frequency += 1.0f;
			if (screen_frequency > SCEEN_MAX_FREQUENCY)
				screen_frequency = SCEEN_MAX_FREQUENCY;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_frequency, screen_frequency);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
#define SCEEN_MIN_FREQUENCY 2.0f
	case 'h':
		if (flag_draw_screen) {
			screen_frequency -= 1.0f;
			if (screen_frequency < SCEEN_MIN_FREQUENCY)
				screen_frequency = SCEEN_MIN_FREQUENCY;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_frequency, screen_frequency);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
	case 'i':
		flag_bus_color = !flag_bus_color;
		if (flag_bus_color) {
			color = 0.0f;
			material_bus.diffuse_color[0] = color;
			material_bus.diffuse_color[1] = color;
			material_bus.diffuse_color[2] = color;
		}
		else {
			material_bus.diffuse_color[0] = 0.7f;
			material_bus.diffuse_color[1] = 0.5f;
			material_bus.diffuse_color[2] = 0.2f;
		}
		glutPostRedisplay();
		break;
	case 'o':
		glUseProgram(h_ShaderProgram_TXPHI);
		flag_random_color = !flag_random_color;
		if (flag_random_color) {
			light2[2].ambient_color[0] = rand() / 32767.0f;
			light2[2].ambient_color[1] = rand() / 32767.0f;
			light2[2].ambient_color[2] = rand() / 32767.0f;
		}
		else {
			light2[2].ambient_color[0] = 0.0f; 
			light2[2].ambient_color[1] = 0.0f;
			light2[2].ambient_color[2] = 0.152f;
		}
		glUniform4fv(loc_light2[2].ambient_color, 1, light2[2].ambient_color);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		translateCamera(-1, 'u');
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		translateCamera(+1, 'u');
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		translateCamera(-1, 'n');
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		translateCamera(+1, 'n');
		glutPostRedisplay();
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			current_camera.move = 1;
			prevX = x, prevY = y;
		}
		else if (state == GLUT_UP)
			current_camera.move = 0;
	}
}

void motion(int x, int y) {
	if (!current_camera.move) return;

	rotateCamera(x - prevX, 'v');
	rotateCamera(y - prevY, 'u');

	prevX = x, prevY = y;

	glutPostRedisplay();
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &grid_VAO);
	glDeleteBuffers(1, &grid_VBO);

	glDeleteVertexArrays(scene.n_materials, sun_temple_VAO);
	glDeleteBuffers(scene.n_materials, sun_temple_VBO);

	glDeleteVertexArrays(1, &skybox_VAO);
	glDeleteBuffers(1, &skybox_VBO);

	glDeleteTextures(scene.n_textures, sun_temple_texture_names);

	free(sun_temple_n_triangles);
	free(sun_temple_vertex_offset);

	free(sun_temple_VAO);
	free(sun_temple_VBO);

	free(sun_temple_texture_names);
	free(flag_texture_mapping);

	glDeleteVertexArrays(1, &cow_VAO);
	glDeleteBuffers(1, &cow_VBO);

	glDeleteVertexArrays(1, &ironman_VAO);
	glDeleteBuffers(1, &ironman_VBO);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void initialize_flags(void) {
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPHI);
	glUniform1i(loc_flag_texture_mapping, true);
	glUseProgram(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	initialize_lights();
	initialize_lights_and_material();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_grid();
	prepare_sun_temple();
	prepare_skybox();
	prepare_cow();
	prepare_ironman();
	prepare_spider();
	prepare_bus();
	prepare_cuboid();
	set_up_scene_lights();
	prepare_rectangle();
	initialize_screen();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "********************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "********************************************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "********************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n********************************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 9
void drawScene(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Sun Temple Scene";
	char messages[N_MESSAGE_LINES][256] = { 
		"    - Keys used:",
		"		'f' : draw x, y, z axes and grid",
		"		'1' : set the camera for bronze statue view",
		"		'2' : set the camera for bronze statue view",
		"		'3' : set the camera for tree view",	
		"		'4' : set the camera for top view",
		"		'5' : set the camera for front view",
		"		'6' : set the camera for side view",
		"		'ESC' : program close",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(20, 20);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
