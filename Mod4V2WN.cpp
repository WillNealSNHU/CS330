#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"     // Image loading Utility functions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

using namespace std;

namespace
{
	const double M_PI = 3.14159265358979323846f; 
	const double M_PI_2 = 1.571428571428571; // just using half of M_PI
}

enum Camera_Movement {
	FORWARD,
	BACKWARD, // think I fixed the camera movement from the previous assignment
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// My camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices
class Camera
{
public:

	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// euler Angles
	float Yaw;
	float Pitch;

	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with the vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// constructor with my scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// its the view matrix along with the lookAt
	glm::mat4 GetViewMatrix() const
	{
		return glm::lookAt(Position, Position + Front, Up);
		//https://gamedev.stackexchange.com/questions/138910/creating-a-movable-camera-using-glmlookat
	}

	//  pretty much the same from the tutorials
	void ProcessInput(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
		if (direction == UP)
			Position += UP * velocity;
		if (direction == DOWN)
			Position -= UP * velocity;
	}

	// input received from the mouse 
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// I want to make sure that when pitch is out of bounds the screen doesn't get flipped, this exact if statement can be found in several places
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// calculates the front vector from the Camera's euler angles
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));  
		Up = glm::normalize(glm::cross(Right, Front));
	}
};



class Meshes
{
	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nVertices;	// Number of vertices for the mesh
		GLuint nIndices;    // Number of indices for the mesh
		// this class is pretty much the same in every openGL project I have seen.
	};

public:
	GLMesh gBoxMesh;
	GLMesh gConeMesh;
	GLMesh gCylinderMesh;
	GLMesh gTaperedCylinderMesh;
	GLMesh gPlaneMesh;
	GLMesh gPrismMesh;
	GLMesh gSphereMesh;
	GLMesh gPyramid3Mesh;
	GLMesh gPyramid4Mesh;
	GLMesh gTorusMesh;
	//like before I wanted this meshes built out so I have them whenever I build out my vertex and index data. 
	// I believe by building out some of the create/destroys and unfinished meshes I was giving myself a lot of problems
	// I think now with some more time and experience under my belt I have found better ways of handling this

public:
	void CreateMeshes();
	void DestroyMeshes();

private:
	void UCreatePlaneMesh(GLMesh& mesh);
	void UCreateBoxMesh(GLMesh& mesh);
	void UCreateCylinderMesh(GLMesh& mesh);
	void UCreatePyramid4Mesh(GLMesh& mesh);
	void UCreateSphereMesh(GLMesh& mesh);
	void UDestroyMesh(GLMesh& mesh);
};


void Meshes::CreateMeshes()
{
	UCreatePlaneMesh(gPlaneMesh);
	UCreateBoxMesh(gBoxMesh);
	UCreateCylinderMesh(gCylinderMesh);
	UCreatePyramid4Mesh(gPyramid4Mesh);
	UCreateSphereMesh(gSphereMesh);
}

void Meshes::DestroyMeshes()
{
	UDestroyMesh(gBoxMesh);
	UDestroyMesh(gConeMesh);
	UDestroyMesh(gCylinderMesh);
	UDestroyMesh(gPlaneMesh);
	UDestroyMesh(gPyramid3Mesh);
	UDestroyMesh(gPyramid4Mesh);
	UDestroyMesh(gPrismMesh);
	UDestroyMesh(gSphereMesh);
	UDestroyMesh(gTorusMesh);
}

void Meshes::UCreatePlaneMesh(GLMesh& mesh)
{
	// Vertex data
	GLfloat verts[] = {
		// Vertex Positions		// Normals			// Texture coords	// Index
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,			//0
		1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,			//1
		1.0f,  0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,			//2
		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,			//3
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	// Generate the VAO for the mesh
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);	// activate the VAO

	// Create VBOs for the mesh
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends data to the GPU

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Meshes::UCreatePyramid4Mesh(GLMesh& mesh)
{
	// Vertex data
	GLfloat verts[] = {
		// Vertex Positions		// Normals			// Texture coords
		//bottom side
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,     //front bottom left
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,		//back bottom left
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f,		//back bottom right	
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,     //front bottom left
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,     //front bottom left
		0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 1.0f,     //front bottom right
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,	1.0f, 0.0f,		//back bottom right	
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,	0.0f, 1.0f,     //front bottom left

		//back side

		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 1.0f,		//top point	
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,	0.0f, 0.0f,		//back bottom right	
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,		//back bottom left
		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, -1.0f,	0.5f, 1.0f,		//top point	

		//left side

		0.0f, 0.5f, 0.0f,		-1.0f, 0.0f, 0.0f,	0.5f, 1.0f,		//top point	
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,		//back bottom left	
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,     //front bottom left
		0.0f, 0.5f, 0.0f,		-1.0f, 0.0f, 0.0f,	0.5f, 1.0f,		//top point	

		//right side

		0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.5f, 1.0f,		//top point	
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	0.0f, 0.0f,     //front bottom right
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,		//back bottom right	
		0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.5f, 1.0f,		//top point	

		//front side

		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 1.0f,		//top point			
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,     //front bottom left	
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,     //front bottom right
		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.5f, 1.0f,		//top point
	};

	const GLuint floatsPerVertex = 3;	// Number of coordinates per vertex
	const GLuint floatsPerColor = 3;	// Number of values per vertex color
	const GLuint floatsPerUV = 2;		// Number of texture coordinate values

	// Calculate total defined vertices
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor + floatsPerUV));

	glGenVertexArrays(1, &mesh.vao);			// Creates 1 VAO
	glGenBuffers(1, mesh.vbos);					// Creates 1 VBO
	glBindVertexArray(mesh.vao);				// Activates the VAO
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);	// Activates the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV);

	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
	glEnableVertexAttribArray(2);
}

void Meshes::UCreateBoxMesh(GLMesh& mesh)
{
	// Position and Color data
	GLfloat verts[] = {
		//Positions				//Normals
		// ------------------------------------------------------

		//Back Face				//Negative Z Normal  Texture Coords.
		0.5f, 0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  0.0f, 1.0f,   //0
		// still trying to build this out
		0.5f, -0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  0.0f, 0.0f,   //1
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,  1.0f, 0.0f,   //2
		-0.5f, 0.5f, -0.5f,		0.0f,  0.0f, -1.0f,  1.0f, 1.0f,   //3

		//Bottom Face			//Negative Y Normal
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  //4
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  //5
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  //6
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f,  0.0f,  1.0f, 1.0f, //7

		//Left Face				//Negative X Normal
		-0.5f, 0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,      //8
		-0.5f, -0.5f,  -0.5f,	1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //9
		-0.5f,  -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //10
		-0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //11

		//Right Face			//Positive X Normal
		0.5f,  0.5f,  0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  //12
		0.5f,  -0.5f, 0.5f,		1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //13
		0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //14
		0.5f, 0.5f, -0.5f,		1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //15

		//Top Face				//Positive Y Normal
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //16
		-0.5f,  0.5f, 0.5f,		0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //17
		0.5f,  0.5f,  0.5f,		0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //18
		0.5f,  0.5f,  -0.5f,	0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //19

		//Front Face			//Positive Z Normal
		-0.5f, 0.5f,  0.5f,	    0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //20
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  0.0f, 0.0f, //21
		0.5f,  -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  1.0f, 0.0f, //22
		0.5f,  0.5f,  0.5f,		0.0f,  0.0f,  1.0f,  1.0f, 1.0f, //23
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2,
		4,5,6,
		4,7,6,
		8,9,10,
		8,11,10,
		12,13,14,
		12,15,14,
		16,17,18,
		16,19,18,
		20,21,22,
		20,23,22
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

	glGenVertexArrays(1, &mesh.vao); 
	glBindVertexArray(mesh.vao);

	// Making 2 buffers here one for the indices and the other for vertex
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// https://www.reddit.com/r/opengl/comments/xpplbw/cylinder_modern_opengl/

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Meshes::UCreateCylinderMesh(GLMesh& mesh)
{
	GLfloat verts[] = {
		// cylinder bottom		// normals			// texture coords
		1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f,1.0f,
		.98f, 0.0f, -0.17f,		0.0f, -1.0f, 0.0f,	0.41f, 0.983f,
		.94f, 0.0f, -0.34f,		0.0f, -1.0f, 0.0f,	0.33f, 0.96f,
		.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.92f,
		.77f, 0.0f, -0.64f,		0.0f, -1.0f, 0.0f,	0.17f, 0.87f,
		.64f, 0.0f, -0.77f,		0.0f, -1.0f, 0.0f,	0.13f, 0.83f,
		.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.77f,
		.34f, 0.0f, -0.94f,		0.0f, -1.0f, 0.0f,	0.04f, 0.68f,
		.17f, 0.0f, -0.98f,		0.0f, -1.0f, 0.0f,	0.017f, 0.6f,
		0.0f, 0.0f, -1.0f,		0.0f, -1.0f, 0.0f,	0.0f,0.5f,
		-.17f, 0.0f, -0.98f,	0.0f, -1.0f, 0.0f,	0.017f, 0.41f,
		-.34f, 0.0f, -0.94f,	0.0f, -1.0f, 0.0f,	0.04f, 0.33f,
		-.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.25f,
		-.64f, 0.0f, -0.77f,	0.0f, -1.0f, 0.0f,	0.13f, 0.17f,
		-.77f, 0.0f, -0.64f,	0.0f, -1.0f, 0.0f,	0.17f, 0.13f,
		-.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.08f,
		-.94f, 0.0f, -0.34f,	0.0f, -1.0f, 0.0f,	0.33f, 0.04f,
		-.98f, 0.0f, -0.17f,	0.0f, -1.0f, 0.0f,	0.41f, 0.017f,
		-1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f, 0.0f,
		-.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.017f,
		-.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.04f,
		-.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.08f,
		-.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.13f,
		-.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.17f,
		-.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.25f,
		-.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.33f,
		-.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.41f,
		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 0.5f,
		.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.6f,
		.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.68f,
		.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.77f,
		.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.83f,
		.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.87f,
		.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.92f,
		.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.96f,
		.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.983f,

		// cylinder top			// normals			// texture coords
		1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f,1.0f,
		// still trying to build this out
		.98f, 1.0f, -0.17f,		0.0f, 1.0f, 0.0f,	0.41f, 0.983f,
		.94f, 1.0f, -0.34f,		0.0f, 1.0f, 0.0f,	0.33f, 0.96f,
		.87f, 1.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.25f, 0.92f,
		.77f, 1.0f, -0.64f,		0.0f, 1.0f, 0.0f,	0.17f, 0.87f,
		.64f, 1.0f, -0.77f,		0.0f, 1.0f, 0.0f,	0.13f, 0.83f,
		.5f, 1.0f, -0.87f,		0.0f, 1.0f, 0.0f,	0.08f, 0.77f,
		.34f, 1.0f, -0.94f,		0.0f, 1.0f, 0.0f,	0.04f, 0.68f,
		.17f, 1.0f, -0.98f,		0.0f, 1.0f, 0.0f,	0.017f, 0.6f,
		0.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f,0.5f,
		-.17f, 1.0f, -0.98f,	0.0f, 1.0f, 0.0f,	0.017f, 0.41f,
		-.34f, 1.0f, -0.94f,	0.0f, 1.0f, 0.0f,	0.04f, 0.33f,
		-.5f, 1.0f, -0.87f,		0.0f, 1.0f, 0.0f,	0.08f, 0.25f,
		-.64f, 1.0f, -0.77f,	0.0f, 1.0f, 0.0f,	0.13f, 0.17f,
		-.77f, 1.0f, -0.64f,	0.0f, 1.0f, 0.0f,	0.17f, 0.13f,
		-.87f, 1.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.25f, 0.08f,
		-.94f, 1.0f, -0.34f,	0.0f, 1.0f, 0.0f,	0.33f, 0.04f,
		-.98f, 1.0f, -0.17f,	0.0f, 1.0f, 0.0f,	0.41f, 0.017f,
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f, 0.0f,
		-.98f, 1.0f, 0.17f,		0.0f, 1.0f, 0.0f,	0.6f, 0.017f,
		-.94f, 1.0f, 0.34f,		0.0f, 1.0f, 0.0f,	0.68f, 0.04f,
		-.87f, 1.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.77f, 0.08f,
		-.77f, 1.0f, 0.64f,		0.0f, 1.0f, 0.0f,	0.83f, 0.13f,
		-.64f, 1.0f, 0.77f,		0.0f, 1.0f, 0.0f,	0.87f, 0.17f,
		-.5f, 1.0f, 0.87f,		0.0f, 1.0f, 0.0f,	0.92f, 0.25f,
		-.34f, 1.0f, 0.94f,		0.0f, 1.0f, 0.0f,	0.96f, 0.33f,
		-.17f, 1.0f, 0.98f,		0.0f, 1.0f, 0.0f,	0.983f, 0.41f,
		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.5f,
		.17f, 1.0f, 0.98f,		0.0f, 1.0f, 0.0f,	0.983f, 0.6f,
		.34f, 1.0f, 0.94f,		0.0f, 1.0f, 0.0f,	0.96f, 0.68f,
		.5f, 1.0f, 0.87f,		0.0f, 1.0f, 0.0f,	0.92f, 0.77f,
		.64f, 1.0f, 0.77f,		0.0f, 1.0f, 0.0f,	0.87f, 0.83f,
		.77f, 1.0f, 0.64f,		0.0f, 1.0f, 0.0f,	0.83f, 0.87f,
		.87f, 1.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.77f, 0.92f,
		.94f, 1.0f, 0.34f,		0.0f, 1.0f, 0.0f,	0.68f, 0.96f,
		.98f, 1.0f, 0.17f,		0.0f, 1.0f, 0.0f,	0.6f, 0.983f,

		// cylinder body		// normals				// texture coords
		1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0,1.0,
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0,0.0,
		.98f, 0.0f, -0.17f,		1.0f, 0.0f, 0.0f,		0.0277,0.0,
		1.0f, 1.0f, 0.0f,		0.92f, 0.0f, -0.08f,	0.0,1.0,
		.98f, 1.0f, -0.17f,		0.92f, 0.0f, -0.08f,	0.0277,1.0,
		.98f, 0.0f, -0.17f,		0.92f, 0.0f, -0.08f,	0.0277,0.0,
		.94f, 0.0f, -0.34f,		0.83f, 0.0f, -0.17f,	0.0554,0.0,
		.98f, 1.0f, -0.17f,		0.83f, 0.0f, -0.17f,	0.0277,1.0,
		.94f, 1.0f, -0.34f,		0.83f, 0.0f, -0.17f,	0.0554,1.0,
		.94f, 0.0f, -0.34f,		0.75f, 0.0f, -0.25f,	0.0554,0.0,
		.87f, 0.0f, -0.5f,		0.75f, 0.0f, -0.25f,	0.0831,0.0,
		.94f, 1.0f, -0.34f,		0.75f, 0.0f, -0.25f,	0.0554,1.0,
		.87f, 1.0f, -0.5f,		0.67f, 0.0f, -0.33f,	0.0831,1.0,
		.87f, 0.0f, -0.5f,		0.67f, 0.0f, -0.33f,	0.0831,0.0,
		.77f, 0.0f, -0.64f,		0.67f, 0.0f, -0.33f,	0.1108,0.0,
		.87f, 1.0f, -0.5f,		0.58f, 0.0f, -0.42f,	0.0831,1.0,
		.77f, 1.0f, -0.64f,		0.58f, 0.0f, -0.42f,	0.1108,1.0,
		.77f, 0.0f, -0.64f,		0.58f, 0.0f, -0.42f,	0.1108,0.0,
		.64f, 0.0f, -0.77f,		0.5f, 0.0f, -0.5f,		0.1385,0.0,
		.77f, 1.0f, -0.64f,		0.5f, 0.0f, -0.5f,		0.1108,1.0,
		.64f, 1.0f, -0.77f,		0.5f, 0.0f, -0.5f,		0.1385,1.0,
		.64f, 0.0f, -0.77f,		0.42f, 0.0f, -0.58f,	0.1385,0.0,
		.5f, 0.0f, -0.87f,		0.42f, 0.0f, -0.58f,	0.1662,0.0,
		.64f, 1.0f, -0.77f,		0.42f, 0.0f, -0.58f,	0.1385, 1.0,
		.5f, 1.0f, -0.87f,		0.33f, 0.0f, -0.67f,	0.1662, 1.0,
		.5f, 0.0f, -0.87f,		0.33f, 0.0f, -0.67f,	0.1662, 0.0,
		.34f, 0.0f, -0.94f,		0.33f, 0.0f, -0.67f,	0.1939, 0.0,
		.5f, 1.0f, -0.87f,		0.25f, 0.0f, -0.75f,	0.1662, 1.0,
		.34f, 1.0f, -0.94f,		0.25f, 0.0f, -0.75f,	0.1939, 1.0,
		.34f, 0.0f, -0.94f,		0.25f, 0.0f, -0.75f,	0.1939, 0.0,
		.17f, 0.0f, -0.98f,		0.17f, 0.0f, -0.83f,	0.2216, 0.0,
		.34f, 1.0f, -0.94f,		0.17f, 0.0f, -0.83f,	0.1939, 1.0,
		.17f, 1.0f, -0.98f,		0.17f, 0.0f, -0.83f,	0.2216, 1.0,
		.17f, 0.0f, -0.98f,		0.08f, 0.0f, -0.92f,	0.2216, 0.0,
		0.0f, 0.0f, -1.0f,		0.08f, 0.0f, -0.92f,	0.2493, 0.0,
		.17f, 1.0f, -0.98f,		0.08f, 0.0f, -0.92f,	0.2216, 1.0,
		0.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.2493, 1.0,
		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.2493, 0.0,
		-.17f, 0.0f, -0.98f,	0.0f, 0.0f, -1.0f,		0.277, 0.0,
		0.0f, 1.0f, -1.0f,		0.08f, 0.0f, -1.08f,	0.2493, 1.0,
		-.17f, 1.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 1.0,
		-.17f, 0.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 0.0,
		-.34f, 0.0f, -0.94f,	-0.08f, 0.0f, -0.92f,	0.3047, 0.0,
		-.17f, 1.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 1.0,
		-.34f, 1.0f, -0.94f,	-0.17f, 0.0f, -0.83f,	0.3047, 1.0,
		-.34f, 0.0f, -0.94f,	-0.17f, 0.0f, -0.83f,	0.3047, 0.0,
		-.5f, 0.0f, -0.87f,		-0.17f, 0.0f, -0.83f,	0.3324, 0.0,
		-.34f, 1.0f, -0.94f,	-0.25f, 0.0f, -0.75f,	0.3047, 1.0,
		-.5f, 1.0f, -0.87f,		-0.25f, 0.0f, -0.75f,	0.3324, 1.0,
		-.5f, 0.0f, -0.87f,		-0.25f, 0.0f, -0.75f,	0.3324, 0.0,
		-.64f, 0.0f, -0.77f,	-0.33f, 0.0f, -0.67f,	0.3601, 0.0,
		-.5f, 1.0f, -0.87f,		-0.33f, 0.0f, -0.67f,	0.3324, 1.0,
		-.64f, 1.0f, -0.77f,	-0.33f, 0.0f, -0.67f,	0.3601, 1.0,
		-.64f, 0.0f, -0.77f,	-0.42f, 0.0f, -0.58f,	0.3601, 0.0,
		-.77f, 0.0f, -0.64f,	-0.42f, 0.0f, -0.58f,	0.3878, 0.0,
		-.64f, 1.0f, -0.77f,	-0.42f, 0.0f, -0.58f,	0.3601, 1.0,
		-.77f, 1.0f, -0.64f,	-0.5f, 0.0f, -0.5f,		0.3878, 1.0,
		-.77f, 0.0f, -0.64f,	-0.5f, 0.0f, -0.5f,		0.3878, 0.0,
		-.87f, 0.0f, -0.5f,		-0.5f, 0.0f, -0.5f,		0.4155, 0.0,
		-.77f, 1.0f, -0.64f,	-0.58f, 0.0f, -0.42f,	0.3878, 1.0,
		-.87f, 1.0f, -0.5f,		-0.58f, 0.0f, -0.42f,	0.4155, 1.0,
		-.87f, 0.0f, -0.5f,		-0.58f, 0.0f, -0.42f,	0.4155, 0.0,
		-.94f, 0.0f, -0.34f,	-0.67f, 0.0f, -0.33f,	0.4432, 0.0,
		-.87f, 1.0f, -0.5f,		-0.67f, 0.0f, -0.33f,	0.4155, 1.0,
		-.94f, 1.0f, -0.34f,	-0.67f, 0.0f, -0.33f,	0.4432, 1.0,
		-.94f, 0.0f, -0.34f,	-0.75f, 0.0f, -0.25f,	0.4432, 0.0,
		-.98f, 0.0f, -0.17f,	-0.75f, 0.0f, -0.25f,	0.4709, 0.0,
		-.94f, 1.0f, -0.34f,	-0.75f, 0.0f, -0.25f,	0.4432, 1.0,
		-.98f, 1.0f, -0.17f,	-0.83f, 0.0f, -0.17f,	0.4709, 1.0,
		-.98f, 0.0f, -0.17f,	-0.83f, 0.0f, -0.17f,	0.4709, 0.0,
		-1.0f, 0.0f, 0.0f,		-0.83f, 0.0f, -0.17f,	0.4986, 0.0,
		-.98f, 1.0f, -0.17f,	-0.92f, 0.0f, -0.08f,	0.4709, 1.0,
		-1.0f, 1.0f, 0.0f,		-0.92f, 0.0f, -0.08f,	0.4986, 1.0,
		-1.0f, 0.0f, 0.0f,		-0.92f, 0.0f, -0.08f,	0.4986, 0.0,
		-.98f, 0.0f, 0.17f,		-1.0f, 0.0f, 0.0f,		0.5263, 0.0,
		-1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.4986, 1.0,
		-.98f, 1.0f, 0.17f,		-1.0f, 0.0f, 0.0f,		0.5263, 1.0,
		-.98f, 0.0f, 0.17f,		-0.92f, 0.0f, 0.08f,	0.5263, 0.0,
		-.94f, 0.0f, 0.34f,		-0.92f, 0.0f, 0.08f,	0.554, 0.0,
		-.98f, 1.0f, 0.17f,		-0.92f, 0.0f, 0.08f,	0.5263, 1.0,
		-.94f, 1.0f, 0.34f,		-0.83f, 0.0f, 0.17f,	0.554, 1.0,
		-.94f, 0.0f, 0.34f,		-0.83f, 0.0f, 0.17f,	0.554, 0.0,
		-.87f, 0.0f, 0.5f,		-0.83f, 0.0f, 0.17f,	0.5817, 0.0,
		-.94f, 1.0f, 0.34f,		-0.75f, 0.0f, 0.25f,	0.554, 1.0,
		-.87f, 1.0f, 0.5f,		-0.75f, 0.0f, 0.25f,	0.5817, 1.0,
		-.87f, 0.0f, 0.5f,		-0.75f, 0.0f, 0.25f,	0.5817, 0.0,
		-.77f, 0.0f, 0.64f,		-0.67f, 0.0f, 0.33f,	0.6094, 0.0,
		-.87f, 1.0f, 0.5f,		-0.67f, 0.0f, 0.33f,	0.5817, 1.0,
		-.77f, 1.0f, 0.64f,		-0.67f, 0.0f, 0.33f,	0.6094, 1.0,
		-.77f, 0.0f, 0.64f,		-0.58f, 0.0f, 0.42f,	0.6094, 0.0,
		-.64f, 0.0f, 0.77f,		-0.58f, 0.0f, 0.42f,	0.6371, 0.0,
		-.77f, 1.0f, 0.64f,		-0.58f, 0.0f, 0.42f,	0.6094, 1.0,
		-.64f, 1.0f, 0.77f,		-0.5f, 0.0f, 0.5f,		0.6371, 1.0,
		-.64f, 0.0f, 0.77f,		-0.5f, 0.0f, 0.5f,		0.6371, 0.0,
		-.5f, 0.0f, 0.87f,		-0.5f, 0.0f, 0.5f,		0.6648, 0.0,
		-.64f, 1.0f, 0.77f,		-0.42f, 0.0f, 0.58f,	0.6371, 1.0,
		-.5f, 1.0f, 0.87f,		-0.42f, 0.0f, 0.58f,	0.6648, 1.0,
		-.5f, 0.0f, 0.87f,		-0.42f, 0.0f, 0.58f,	0.6648, 0.0,
		-.34f, 0.0f, 0.94f,		-0.33f, 0.0f, 0.67f,	0.6925, 0.0,
		-.5f, 1.0f, 0.87f,		-0.33f, 0.0f, 0.67f,	0.6648, 1.0,
		-.34f, 1.0f, 0.94f,		-0.33f, 0.0f, 0.67f,	0.6925, 1.0,
		-.34f, 0.0f, 0.94f,		-0.25f, 0.0f, 0.75f,	0.6925, 0.0,
		-.17f, 0.0f, 0.98f,		-0.25f, 0.0f, 0.75f,	0.7202, 0.0,
		-.34f, 1.0f, 0.94f,		-0.25f, 0.0f, 0.75f,	0.6925, 1.0,
		-.17f, 1.0f, 0.98f,		-0.17f, 0.0f, 0.83f,	0.7202, 1.0,
		-.17f, 0.0f, 0.98f,		-0.17f, 0.0f, 0.83f,	0.7202, 0.0,
		0.0f, 0.0f, 1.0f,		-0.17f, 0.0f, 0.83f,	0.7479, 0.0,
		-.17f, 1.0f, 0.98f,		-0.08f, 0.0f, 0.92f,	0.7202, 1.0,
		0.0f, 1.0f, 1.0f,		-0.08f, 0.0f, 0.92f,	0.7479, 1.0,
		0.0f, 0.0f, 1.0f,		-0.08f, 0.0f, 0.92f,	0.7479, 0.0,
		.17f, 0.0f, 0.98f,		-0.0f, 0.0f, 1.0f,		0.7756, 0.0,
		0.0f, 1.0f, 1.0f,		-0.0f, 0.0f, 1.0f,		0.7479, 1.0,
		.17f, 1.0f, 0.98f,		-0.0f, 0.0f, 1.0f,		0.7756, 1.0,
		.17f, 0.0f, 0.98f,		0.08f, 0.0f, 0.92f,		0.7756, 0.0,
		.34f, 0.0f, 0.94f,		0.08f, 0.0f, 0.92f,		0.8033, 0.0,
		.17f, 1.0f, 0.98f,		0.08f, 0.0f, 0.92f,		0.7756, 1.0,
		.34f, 1.0f, 0.94f,		0.17f, 0.0f, 0.83f,		0.8033, 1.0,
		.34f, 0.0f, 0.94f,		0.17f, 0.0f, 0.83f,		0.8033, 0.0,
		.5f, 0.0f, 0.87f,		0.17f, 0.0f, 0.83f,		0.831, 0.0,
		.34f, 1.0f, 0.94f,		0.25f, 0.0f, 0.75f,		0.8033, 1.0,
		.5f, 1.0f, 0.87f,		0.25f, 0.0f, 0.75f,		0.831, 1.0,
		.5f, 0.0f, 0.87f,		0.25f, 0.0f, 0.75f,		0.831, 0.0,
		.64f, 0.0f, 0.77f,		0.33f, 0.0f, 0.67f,		0.8587, 0.0,
		.5f, 1.0f, 0.87f,		0.33f, 0.0f, 0.67f,		0.831, 1.0,
		.64f, 1.0f, 0.77f,		0.33f, 0.0f, 0.67f,		0.8587, 1.0,
		.64f, 0.0f, 0.77f,		0.42f, 0.0f, 0.58f,		0.8587, 0.0,
		.77f, 0.0f, 0.64f,		0.42f, 0.0f, 0.58f,		0.8864, 0.0,
		.64f, 1.0f, 0.77f,		0.42f, 0.0f, 0.58f,		0.8587, 1.0,
		.77f, 1.0f, 0.64f,		0.5f, 0.0f, 0.5f,		0.8864, 1.0,
		.77f, 0.0f, 0.64f,		0.5f, 0.0f, 0.5f,		0.8864, 0.0,
		.87f, 0.0f, 0.5f,		0.5f, 0.0f, 0.5f,		0.9141, 0.0,
		.77f, 1.0f, 0.64f,		0.58f, 0.0f, 0.42f,		0.8864, 1.0,
		.87f, 1.0f, 0.5f,		0.58f, 0.0f, 0.42f,		0.9141, 1.0,
		.87f, 0.0f, 0.5f,		0.58f, 0.0f, 0.42f,		0.9141, 0.0,
		.94f, 0.0f, 0.34f,		0.67f, 0.0f, 0.33f,		0.9418, 0.0,
		.87f, 1.0f, 0.5f,		0.67f, 0.0f, 0.33f,		0.9141, 1.0,
		.94f, 1.0f, 0.34f,		0.67f, 0.0f, 0.33f,		0.9418, 1.0,
		.94f, 0.0f, 0.34f,		0.75f, 0.0f, 0.25f,		0.9418, 0.0,
		.98f, 0.0f, 0.17f,		0.75f, 0.0f, 0.25f,		0.9695, 0.0,
		.94f, 1.0f, 0.34f,		0.75f, 0.0f, 0.25f,		0.9418, 0.0,
		.98f, 1.0f, 0.17f,		0.83f, 0.0f, 0.17f,		0.9695, 1.0,
		.98f, 0.0f, 0.17f,		0.83f, 0.0f, 0.17f,		0.9695, 0.0,
		1.0f, 0.0f, 0.0f,		0.83f, 0.0f, 0.17f,		1.0, 0.0,
		.98f, 1.0f, 0.17f,		0.92f, 0.0f, 0.08f,		0.9695, 1.0,
		1.0f, 1.0f, 0.0f,		0.92f, 0.0f, 0.08f,		1.0, 1.0,
		1.0f, 0.0f, 0.0f,		0.92f, 0.0f, 0.08f,		1.0, 0.0
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); 
	glBindVertexArray(mesh.vao);

	// Create VBO
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); 


	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);


	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Meshes::UCreateSphereMesh(GLMesh& mesh)
{
	GLfloat verts[] = {
		// vertex data					// index
		// top center point
		0.0f, 1.0f, 0.0f,				//0

		// ring 1
		0.0f, 0.9808f, 0.1951f,			//1
		0.0747f, 0.9808f, 0.1802f,		//2
		0.1379f, 0.9808f, 0.1379f,		//3
		0.1802f, 0.9808f, 0.0747f,		//4
		0.1951f, 0.9808, 0.0f,			//5
		0.1802f, 0.9808f, -0.0747f,		//6
		0.1379f, 0.9808f, -0.1379f,		//7
		0.0747f, 0.9808f, -0.1802f,		//8
		0.0f, 0.9808f, -0.1951f,		//9
		-0.0747f, 0.9808f, -0.1802f,	//10
		-0.1379f, 0.9808f, -0.1379f,	//11
		-0.1802f, 0.9808f, -0.0747f,	//12
		-0.1951f, 0.9808, 0.0f,			//13
		-0.1802f, 0.9808f, 0.0747f,		//14
		-0.1379f, 0.9808f, 0.1379f,		//15
		-0.0747f, 0.9808f, 0.1802f,		//16

		// ring 2
		0.0f, 0.9239f, 0.3827f,			//17
		0.1464f, 0.9239f, 0.3536f,		//18
		0.2706f, 0.9239f, 0.2706f,		//19
		0.3536f, 0.9239f, 0.1464f,		//20
		0.3827f, 0.9239f, 0.0f,			//21
		0.3536f, 0.9239f, -0.1464f,		//22
		0.2706f, 0.9239f, -0.2706f,		//23
		0.1464f, 0.9239f, -0.3536f,		//24
		0.0f, 0.9239f, -0.3827f,		//25
		-0.1464f, 0.9239f, -0.3536f,	//26
		-0.2706f, 0.9239f, -0.2706f,	//27
		-0.3536f, 0.9239f, -0.1464f,	//28
		-0.3827f, 0.9239f, 0.0f,		//29
		-0.3536f, 0.9239f, 0.1464f,		//30
		-0.2706f, 0.9239f, 0.2706f,		//31
		-0.1464f, 0.9239f, 0.3536f,		//32

		// ring 3
		0.0f, 0.8315f, 0.5556f,			//33
		0.2126f, 0.8315f, 0.5133f,		//34
		0.3928f, 0.8315f, 0.3928f,		//35
		0.5133f, 0.8315f, 0.2126f,		//36
		0.5556f, 0.8315f, 0.0f,			//37
		0.5133f, 0.8315f, -0.2126f,		//38
		0.3928f, 0.8315f, -0.3928f,		//39
		0.2126f, 0.8315f, -0.5133f,		//40
		0.0f, 0.8315f, -0.5556f,		//41
		-0.2126f, 0.8315f, -0.5133f,	//42
		-0.3928f, 0.8315f, -0.3928f,	//43
		-0.5133f, 0.8315f, -0.2126f,	//44
		-0.5556f, 0.8315f, 0.0f,		//45
		-0.5133f, 0.8315f, 0.2126f,		//46
		-0.3928f, 0.8315f, 0.3928f,		//47
		-0.2126f, 0.8315f, 0.5133f,		//48

		// ring 4
		0.0f, 0.7071f, 0.7071f,			//49
		0.2706f, 0.7071f, 0.6533f,		//50
		0.5f, 0.7071f, 0.5f,			//51
		0.6533f, 0.7071f, 0.2706f,		//52
		0.7071f, 0.7071f, 0.0f,			//53
		0.6533f, 0.7071f, -0.2706f,		//54
		0.5f, 0.7071f, -0.5f,			//55
		0.2706f, 0.7071f, -0.6533f,		//56
		0.0f, 0.7071f, -0.7071f,		//57
		-0.2706f, 0.7071f, -0.6533f,	//58
		-0.5f, 0.7071f, -0.5f,			//59
		-0.6533f, 0.7071f, -0.2706f,	//60
		-0.7071f, 0.7071f, 0.0f,		//61
		-0.6533f, 0.7071f, 0.2706f,		//62
		-0.5f, 0.7071f, 0.5f,			//63
		-0.2706f, 0.7071f, 0.6533f,		//64

		// ring 5
		0.0f, 0.5556f, 0.8315f,			//65
		0.3182f, 0.5556f, 0.7682f,		//66
		0.5879f, 0.5556f, 0.5879f,		//67
		0.7682f, 0.5556f, 0.3182f,		//68
		0.8315f, 0.5556f, 0.0f,			//69
		0.7682f, 0.5556f, -0.3182f,		//70
		0.5879f, 0.5556f, -0.5879f,		//71
		0.3182f, 0.5556f, -0.7682f,		//72
		0.0f, 0.5556f, -0.8315f,		//73
		-0.3182f, 0.5556f, -0.7682f,	//74
		-0.5879f, 0.5556f, -0.5879f,	//75
		-0.7682f, 0.5556f, -0.3182f,	//76
		-0.8315f, 0.5556f, 0.0f,		//77
		-0.7682f, 0.5556f, 0.3182f,		//78
		-0.5879f, 0.5556f, 0.5879f,		//79
		-0.3182f, 0.5556f, 0.7682f,		//80

		//ring 6
		0.0f, 0.3827f, 0.9239f,			//81
		0.3536f, 0.3827f, 0.8536f,		//82
		0.6533f, 0.3827f, 0.6533f,		//83
		0.8536f, 0.3827f, 0.3536f,		//84
		0.9239f, 0.3827f, 0.0f,			//85
		0.8536f, 0.3827f, -0.3536f,		//86
		0.6533f, 0.3827f, -0.6533f,		//87
		0.3536f, 0.3827f, -0.8536f,		//88
		0.0f, 0.3827f, -0.9239f,		//89
		-0.3536f, 0.3827f, -0.8536f,	//90
		-0.6533f, 0.3827f, -0.6533f,	//91
		-0.8536f, 0.3827f, -0.3536f,	//92
		-0.9239f, 0.3827f, 0.0f,		//93
		-0.8536f, 0.3827f, 0.3536f,		//94
		-0.6533f, 0.3827f, 0.6533f,		//95

		-0.3536f, 0.3827f, 0.8536f,		//96

		// ring 7
		0.0f, 0.1951f, 0.9808f,			//97
		0.3753f, 0.1915f, 0.9061f,		//98
		0.6935f, 0.1915f, 0.6935f,		//99
		0.9061f, 0.1915f, 0.3753f,		//100
		0.9808f, 0.1915f, 0.0f,			//101
		0.9061f, 0.1915f, -0.3753f,		//102
		0.6935f, 0.1915f, -0.6935f,		//103
		0.3753f, 0.1915f, -0.9061f,		//104
		0.0f, 0.1915f, -0.9808f,		//105
		-0.3753f, 0.1915f, -0.9061f,	//106
		-0.6935f, 0.1915f, -0.6935f,	//107
		-0.9061f, 0.1915f, -0.3753f,	//108
		-0.9808f, 0.1915f, 0.0f,		//109
		-0.9061f, 0.1915f, 0.3753f,		//110
		-0.6935f, 0.1915f, 0.6935f,		//111
		-0.3753f, 0.1915f, 0.9061f,		//112

		// ring 8
		0.0f, 0.0f, 1.0f,				//113
		0.3827f, 0.0f, 0.9239f,			//114
		0.7071f, 0.0f, 0.7071f,			//115
		0.9239f, 0.0f, 0.3827f,			//116
		1.0f, 0.0f, 0.0f,				//117
		0.9239f, 0.0f, -0.3827f,		//118
		0.7071f, 0.0f, -0.7071f,		//119
		0.3827f, 0.0f, -0.9239f,		//120
		0.0f, 0.0f, -1.0f,				//121
		-0.3827f, 0.0f, -0.9239f,		//122
		-0.7071f, 0.0f, -0.7071f,		//123
		-0.9239f, 0.0f, -0.3827f,		//124
		-1.0f, 0.0f, 0.0f,				//125
		-0.9239f, 0.0f, 0.3827f,		//126
		-0.7071, 0.0, 0.7071f,			//127
		-0.3827f, 0.0f, 0.9239f,		//128

		// ring 9
		0.0f, -0.1915f, 0.9808f,		//129
		0.3753f, -0.1915f, 0.9061f,		//130
		0.6935f, -0.1915f, 0.6935f,		//131
		0.9061f, -0.1915f, 0.3753f,		//132
		0.9808f, -0.1915f, 0.0f,		//133
		0.9061f, -0.1915f, -0.3753f,	//134
		0.6935f, -0.1915f, -0.6935f,	//135
		0.3753f, -0.1915f, -0.9061f,	//136
		0.0f, -0.1915f, -0.9808f,		//137
		-0.3753f, -0.1915f, -0.9061f,	//138
		-0.6935f, -0.1915f, -0.6935f,	//139
		-0.9061f, -0.1915f, -0.3753f,	//140
		-0.9808f, -0.1915f, 0.0f,		//141
		-0.9061f, -0.1915f, 0.3753f,	//142
		-0.6935f, -0.1915f, 0.6935f,	//143
		-0.3753f, -0.1915f, 0.9061f,	//144

		// ring 10
		0.0f, -0.3827f, 0.9239f,		//145
		0.3536f, -0.3827f, 0.8536f,		//146
		0.6533f, -0.3827f, 0.6533f,		//147
		0.8536f, -0.3827f, 0.3536f,		//148
		0.9239f, -0.3827f, 0.0f,		//149
		0.8536f, -0.3827f, -0.3536f,	//150
		0.6533f, -0.3827f, -0.6533f,	//151
		0.3536f, -0.3827f, -0.8536f,	//152
		0.0f, -0.3827f, -0.9239f,		//153
		-0.3536f, -0.3827f, -0.8536f,	//154
		-0.6533f, -0.3827f, -0.6533f,	//155
		-0.8536f, -0.3827f, -0.3536f,	//156
		-0.9239f, -0.3827f, 0.0f,		//157
		-0.8536f, -0.3827f, 0.3536f,	//158
		-0.6533f, -0.3827f, 0.6533f,	//159
		-0.3536f, -0.3827f, 0.8536f,	//160

		// ring 11
		0.0f, -0.5556f, 0.8315f,		//161
		0.3182f, -0.5556f, 0.7682f,		//162
		0.5879f, -0.5556f, 0.5879f,		//163
		0.7682f, -0.5556f, 0.3182f,		//164
		0.8315f, -0.5556f, 0.0f,		//165
		0.7682f, -0.5556f, -0.3182f,	//166
		0.5879f, -0.5556f, -0.5879f,	//167
		0.3182f, -0.5556f, -0.7682f,	//168
		0.0f, -0.5556f, -0.8315f,		//169
		-0.3182f, -0.5556f, -0.7682f,	//170
		-0.5879f, 0.5556f, -0.5879f,	//171
		-0.7682f, -0.5556f, -0.3182f,	//172
		-0.8315f, -0.5556f, 0.0f,		//173
		-0.7682f, -0.5556f, 0.3182f,	//174
		-0.5879f, -0.5556f, 0.5879f,	//175
		-0.3182f, -0.5556f, 0.7682f,	//176

		// ring 12
		0.0f, -0.7071f, 0.7071f,		//177
		0.2706f, -0.7071f, 0.6533f,		//178
		0.5f, -0.7071f, 0.5f,			//179
		0.6533f, -0.7071f, 0.2706f,		//180
		0.7071f, -0.7071f, 0.0f,		//181
		0.6533f, -0.7071f, -0.2706f,	//182
		0.5f, -0.7071f, -0.5f,			//183
		0.2706f, -0.7071f, -0.6533f,	//184
		0.0f, -0.7071f, -0.7071f,		//185
		-0.2706f, -0.7071f, -0.6533f,	//186
		-0.5f, -0.7071f, -0.5f,			//187
		-0.6533f, -0.7071f, -0.2706f,	//188
		-0.7071f, -0.7071f, 0.0f,		//189
		-0.6533f, -0.7071f, 0.2706f,	//190
		-0.5f, -0.7071f, 0.5f,			//191
		-0.2706f, -0.7071f, 0.6533f,	//192

		// ring 13
		0.0f, -0.8315f, 0.5556f,		//193
		0.2126f, -0.8315f, 0.5133f,		//194
		0.3928f, -0.8315f, 0.3928f,		//195
		0.5133f, -0.8315f, 0.2126f,		//196
		0.5556f, -0.8315f, 0.0f,		//197
		0.5133f, -0.8315f, -0.2126f,	//198
		0.3928f, -0.8315f, -0.3928f,	//199
		0.2126f, -0.8315f, -0.5133f,	//200
		0.0f, -0.8315f, -0.5556f,		//201
		-0.2126f, -0.8315f, -0.5133f,	//202
		-0.3928f, -0.8315f, -0.3928f,	//203
		-0.5133f, -0.8315f, -0.2126f,	//204
		-0.5556f, -0.8315f, 0.0f,		//205
		-0.5133f, -0.8315f, 0.2126f,	//206
		-0.3928f, -0.8315f, 0.3928f,	//207
		-0.2126f, -0.8315f, 0.5133f,	//208

		// ring 14
		0.0f, -0.9239f, 0.3827f,		//209
		0.1464f, -0.9239f, 0.3536f,		//210
		0.2706f, -0.9239f, 0.2706f,		//211
		0.3536f, -0.9239f, 0.1464f,		//212
		0.3827f, -0.9239f, 0.0f,		//213
		0.3536f, -0.9239f, -0.1464f,	//214
		0.2706f, -0.9239f, -0.2706f,	//215
		0.1464f, -0.9239f, -0.3536f,	//216
		0.0f, -0.9239f, -0.3827f,		//217
		-0.1464f, -0.9239f, -0.3536f,	//218
		-0.2706f, -0.9239f, -0.2706f,	//219
		-0.3536f, -0.9239f, -0.1464f,	//220
		-0.3827f, -0.9239f, 0.0f,		//221
		-0.3536f, -0.9239f, 0.1464f,	//222
		-0.2706f, -0.9239f, 0.2706f,	//223
		-0.1464f, -0.9239f, 0.3536f,	//224

		// ring 15
		0.0f, -0.9808f, 0.1951f,		//225
		0.0747f, -0.9808f, 0.1802f,		//226
		0.1379f, -0.9808f, 0.1379f,		//227
		0.1802f, -0.9808f, 0.0747f,		//228
		0.1951f, -0.9808, 0.0f,			//229
		0.1802f, -0.9808f, -0.0747f,	//230
		0.1379f, -0.9808f, -0.1379f,	//231
		0.0747f, -0.9808f, -0.1802f,	//232
		0.0f, -0.9808f, -0.1951f,		//233
		-0.0747f, -0.9808f, -0.1802f,	//234
		-0.1379f, -0.9808f, -0.1379f,	//235
		-0.1802f, -0.9808f, -0.0747f,	//236
		-0.1951f, -0.9808, 0.0f,		//237
		-0.1802f, -0.9808f, 0.0747f,	//238
		-0.1379f, -0.9808f, 0.1379f,	//239
		-0.0747f, -0.9808f, 0.1802f,	//240

		// bottom center point
			0.0f, -1.0f, 0.0f,				//241
	};

	// index data
	GLuint indices[] = {
		//ring 1 - top
		0,1,2,
		0,2,3,
		0,3,4,
		0,4,5,
		0,5,6,
		0,6,7,
		0,7,8,
		0,8,9,
		0,9,10,
		0,10,11,
		0,11,12,
		0,12,13,
		0,13,14,
		0,14,15,
		0,15,16,
		0,16,1,

		// as a side note the rings were the easiest for me to grasp. When I saw the pattern
		// forming as I went down 1-17-18 then 1-2-18, then 2-18-19 and I could physically see
		// how things were building off each other it made building this out easier.
		// ring 1 to ring 2
		1,17,18,
		1,2,18,
		2,18,19,
		2,3,19,
		3,19,20,
		3,4,20,
		4,20,21,
		4,5,21,
		5,21,22,
		5,6,22,
		6,22,23,
		6,7,23,
		7,23,24,
		7,8,24,
		8,24,25,
		8,9,25,
		9,25,26,
		9,10,26,
		10,26,27,
		10,11,27,
		11,27,28,
		11,12,28,
		12,28,29,
		12,13,29,
		13,29,30,
		13,14,30,
		14,30,31,
		14,15,31,
		15,31,32,
		15,16,32,
		16,32,17,
		16,1,17,

		// ring 2 to ring 3
		17,33,34,
		17,18,34,
		18,34,35,
		18,19,35,
		19,35,36,
		19,20,36,
		20,36,37,
		20,21,37,
		21,37,38,
		21,22,38,
		22,38,39,
		22,23,39,
		23,39,40,
		23,24,40,
		24,40,41,
		24,25,41,
		25,41,42,
		25,26,42,
		26,42,43,
		26,27,43,
		27,43,44,
		27,28,44,
		28,44,45,
		28,29,45,
		29,45,46,
		29,30,46,
		30,46,47,
		30,31,47,
		31,47,48,
		31,32,48,
		32,48,33,
		32,17,33,

		// ring 3 to ring 4
		33,49,50,
		33,34,50,
		34,50,51,
		34,35,51,
		35,51,52,
		35,36,52,
		36,52,53,
		36,37,53,
		37,53,54,
		37,38,54,
		38,54,55,
		38,39,55,
		39,55,56,
		39,40,56,
		40,56,57,
		40,41,57,
		41,57,58,
		41,42,58,
		42,58,59,
		42,43,59,
		43,59,60,
		43,44,60,
		44,60,61,
		44,45,61,
		45,61,62,
		45,46,62,
		46,62,63,
		46,47,63,
		47,63,64,
		47,48,64,
		48,64,49,
		48,33,49,

		// ring 4 to ring 5
		49,65,66,
		49,50,66,
		50,66,67,
		50,51,67,
		51,67,68,
		51,52,68,
		52,68,69,
		52,53,69,
		53,69,70,
		53,54,70,
		54,70,71,
		54,55,71,
		55,71,72,
		55,56,72,
		56,72,73,
		56,57,73,
		57,73,74,
		57,58,74,
		58,74,75,
		58,59,75,
		59,75,76,
		59,60,76,
		60,76,77,
		60,61,77,
		61,77,78,
		61,62,78,
		62,78,79,
		62,63,79,
		63,79,80,
		63,64,80,
		64,80,65,
		64,49,65,

		// ring 5 to ring 6
		65,81,82,
		65,66,82,
		66,82,83,
		66,67,83,
		67,83,84,
		67,68,84,
		68,84,85,
		68,69,85,
		69,85,86,
		69,70,86,
		70,86,87,
		70,71,87,
		71,87,88,
		71,72,88,
		72,88,89,
		72,73,89,
		73,89,90,
		73,74,90,
		74,90,91,
		74,75,91,
		75,91,92,
		75,76,92,
		76,92,93,
		76,77,93,
		77,93,94,
		77,78,94,
		78,94,95,
		78,79,95,
		79,95,96,
		79,80,96,
		80,96,81,
		80,65,81,

		// ring 6 to ring 7
		81,97,98,
		81,82,98,
		82,98,99,
		82,83,99,
		83,99,100,
		83,84,100,
		84,100,101,
		84,85,101,
		85,101,102,
		85,86,102,
		86,102,103,
		86,87,103,
		87,103,104,
		87,88,104,
		88,104,105,
		88,89,105,
		89,105,106,
		89,90,106,
		90,106,107,
		90,91,107,
		91,107,108,
		91,92,108,
		92,108,109,
		92,93,109,
		93,109,110,
		93,94,110,
		94,110,111,
		94,95,111,
		95,111,112,
		95,96,112,
		96,112,97,
		96,81,97,

		// ring 7 to ring 8
		97,113,114,
		97,98,114,
		98,114,115,
		98,99,115,
		99,115,116,
		99,100,116,
		100,116,117,
		100,101,117,
		101,117,118,
		101,102,118,
		102,118,119,
		102,103,119,
		103,119,120,
		103,104,120,
		104,120,121,
		104,105,121,
		105,121,122,
		105,106,122,
		106,122,123,
		106,107,123,
		107,123,124,
		107,108,124,
		108,124,125,
		108,109,125,
		109,125,126,
		109,110,126,
		110,126,127,
		110,111,127,
		111,127,128,
		111,112,128,
		112,128,113,
		112,97,113,

		// ring 8 to ring 9
		113,129,130,
		113,114,130,
		114,130,131,
		114,115,131,
		115,131,132,
		115,116,132,
		116,132,133,
		116,117,133,
		117,133,134,
		117,118,134,
		118,134,135,
		118,119,135,
		119,135,136,
		119,120,136,
		120,136,137,
		120,121,137,
		121,137,138,
		121,122,138,
		122,138,139,
		122,123,139,
		123,139,140,
		123,124,140,
		124,140,141,
		124,125,141,
		125,141,142,
		125,126,142,
		126,142,143,
		126,127,143,
		127,143,144,
		127,128,144,
		128,144,129,
		128,113,129,

		// ring 9 to ring 10
		129,145,146,
		129,130,146,
		130,146,147,
		130,131,147,
		131,147,148,
		131,132,148,
		132,148,149,
		132,133,149,
		133,149,150,
		133,134,150,
		134,150,151,
		134,135,151,
		135,151,152,
		135,136,152,
		136,152,153,
		136,137,153,
		137,153,154,
		137,138,154,
		138,154,155,
		138,139,155,
		139,155,156,
		139,140,156,
		140,156,157,
		140,141,157,
		141,157,158,
		141,142,158,
		142,158,159,
		142,143,159,
		143,159,160,
		143,144,160,
		144,160,145,
		144,129,145,

		// ring 10 to ring 11
		145,161,162,
		145,146,162,
		146,162,163,
		146,147,163,
		147,163,164,
		147,148,164,
		148,164,165,
		148,149,165,
		149,165,166,
		149,150,166,
		150,166,167,
		150,151,167,
		151,167,168,
		151,152,168,
		152,168,169,
		152,153,169,
		153,169,170,
		153,154,170,
		154,170,171,
		154,155,171,
		155,171,172,
		155,156,172,
		156,172,173,
		156,157,173,
		157,173,174,
		157,158,174,
		158,174,175,
		158,159,175,
		159,175,176,
		159,160,176,
		160,176,161,
		160,145,161,

		// ring 11 to ring 12
		161,177,178,
		161,162,178,
		162,178,179,
		162,163,179,
		163,179,180,
		163,164,180,
		164,180,181,
		164,165,181,
		165,181,182,
		165,166,182,
		166,182,183,
		166,167,183,
		167,183,184,
		167,168,184,
		168,184,185,
		168,169,185,
		169,185,186,
		169,170,186,
		170,186,187,
		170,171,187,
		171,187,188,
		171,172,188,
		172,188,189,
		172,173,189,
		173,189,190,
		173,174,190,
		174,190,191,
		174,175,191,
		175,191,192,
		175,176,192,
		176,192,177,
		176, 161,177,

		// ring 12 to ring 13
		177,193,194,
		177,178,194,
		178,194,195,
		178,179,195,
		179,195,196,
		179,180,196,
		180,196,197,
		180,181,197,
		181,197,198,
		181,182,198,
		182,198,199,
		182,183,199,
		183,199,200,
		183,184,200,
		184,200,201,
		184,185,201,
		185,201,202,
		185,186,202,
		186,202,203,
		186,187,203,
		187,203,204,
		187,188,204,
		188,204,205,
		188,189,205,
		189,205,206,
		189,190,206,
		190,206,207,
		190,191,207,
		191,207,208,
		191,192,208,
		192,208,193,
		192,177,193,

		// ring 13 to ring 14
		193,209,210,
		193,194,210,
		194,210,211,
		194,195,211,
		195,211,212,
		195,196,212,
		196,212,213,
		196,197,213,
		197,213,214,
		197,198,214,
		198,214,215,
		198,199,215,
		199,215,216,
		199,200,216,
		200,216,217,
		200,201,217,
		201,217,218,
		201,202,218,
		202,218,219,
		202,203,219,
		203,219,220,
		203,204,220,
		204,220,221,
		204,205,221,
		205,221,222,
		205,206,222,
		206,222,223,
		206,207,223,
		207,223,224,
		207,208,224,
		208,224,209,
		208,193,209,

		// ring 14 to ring 15
		209,225,226,
		209,210,226,
		210,226,227,
		210,211,227,
		211,227,228,
		211,212,228,
		212,228,229,
		212,213,229,
		213,229,230,
		213,214,230,
		214,230,231,
		214,215,231,
		215,231,232,
		215,216,232,
		216,232,233,
		216,217,233,
		217,233,234,
		217,218,234,
		218,234,235,
		218,219,235,
		219,235,236,
		219,220,236,
		220,236,237,
		220,221,237,
		221,237,238,
		221,222,238,
		222,238,239,
		222,223,239,
		223,239,240,
		223,224,240,
		224,240,225,
		224,209,225,

		// ring 15 - bottom
		225,226,241,
		226,227,241,
		227,228,241,
		228,229,241,
		229,239,241,
		230,231,241,
		231,232,241,
		232,233,241,
		233,234,241,
		234,235,241,
		235,236,241,
		236,237,241,
		237,238,241,
		238,239,241,
		239,240,241,
		240,225,241
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex));
	mesh.nIndices = sizeof(indices) / (sizeof(indices[0]));

	glm::vec3 normal;
	glm::vec3 vert;
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	float u, v;
	std::vector<GLfloat> combined_values;

	// combine interleaved vertices, normals, abd my texture coords
	// https://www.reddit.com/r/opengl/comments/u26dkd/colors_properly_render_with_glmvec3_but_not_any/ 
	// seeing others mistakes helped a lot
	for (int i = 0; i < sizeof(verts) / (sizeof(verts[0])); i += 3)
	{
		vert = glm::vec3(verts[i], verts[i + 1], verts[i + 2]);
		normal = normalize(vert - center);
		u = atan2(normal.x, normal.z) / (2 * M_PI) + 0.5;
		v = normal.y * 0.5 + 0.5;
		combined_values.push_back(vert.x);
		combined_values.push_back(vert.y);
		combined_values.push_back(vert.z);
		combined_values.push_back(normal.x);
		combined_values.push_back(normal.y);
		combined_values.push_back(normal.z);
		combined_values.push_back(u);
		combined_values.push_back(v);
	}

	// this block of code can pretty much be copy and pasted at the end of each ucreate. You will always need to build out your vao and vbo. Youll always need 
	// to activate the buffers, send vertex data to the GPU and create you attribute pointers. Very little will change here
	// and I actually found it easier to just copy/paste this then attempt to remember and misplace or forget something

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); 
	glBindVertexArray(mesh.vao);

	// Create VBOs
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * combined_values.size(), combined_values.data(), GL_STATIC_DRAW); 


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);


	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void Meshes::UDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}



#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif


namespace
{
	const char* const WINDOW_TITLE = "Project Work V2 10/17";
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	float gCameraSpeed = 2.5f;
	bool gFirstMouse = true;
	//timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	// was able to pull some of this directly from 3-5 milestone
	float gLastFrame = 0.0f;
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nIndices;    // Number of indices of the mesh
	};
	/// Main GLFW Window
	GLFWwindow* gWindow = nullptr;
	// Triangle mesh data
	GLMesh gMesh;
	// Texture id
	GLuint gTextureIdDesk;
	GLuint gTextureIdMug;
	GLuint gTextureIdBotCap;
	GLuint gTextureIdPenBod;
	GLuint gTextureIdBottl;
	GLuint gTextureIdCon;

	Meshes meshes;
	//Shader Program
	GLuint gProgramId;
	//Camera
	Camera gCamera(glm::vec3(0.0f, 1.0f, 8.0f));

	glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 gCameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
}

bool UInitialize(int argc, char* argv[], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
// my favorite part. the part where we destroy it all

const GLchar* vertexShaderSource = GLSL(440,
	// https://stackoverflow.com/questions/43029936/how-to-write-basic-shader-in-glsl
	layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;

	gl_Position = projection * view * vec4(FragPos, 1.0);
}
); // https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/6.multiple_lights.vs

// this was essentially a mixture of multiple rescources, though my last resource fixed a lot of the issues I was having

/* Fragment Shader Source Code */
const GLchar* fragmentShaderSource = GLSL(440,
	out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[5];
uniform Material material;

uniform bool hasTexture;
uniform bool hasTextureTransparency;
uniform vec3 meshColor;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);


	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < 5; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	if (hasTextureTransparency)
		FragColor = vec4(result, texture(material.diffuse, TexCoords).a);
	else
		FragColor = vec4(result, 1.0);

}
// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results

	vec3 texColor;

	if (hasTexture)
		texColor = vec3(texture(material.diffuse, TexCoords));
	else
		texColor = meshColor;

	vec3 ambient = light.ambient * texColor;
	vec3 diffuse = light.diffuse * diff * texColor;
	vec3 specular = light.specular * spec * vec3(0.5, 0.5, 0.5);
	return (ambient + diffuse + specular) * light.intensity;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 texColor;

	if (hasTexture)
		texColor = vec3(texture(material.diffuse, TexCoords));
	else
		texColor = meshColor;

	// combine results
	vec3 ambient = light.ambient * texColor;
	vec3 diffuse = light.diffuse * diff * texColor;
	vec3 specular = light.specular * spec;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular) * light.intensity;
}
);

int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	glfwSetCursorPosCallback(gWindow, UMousePositionCallback);

	// Set the mouse scroll callback
	glfwSetScrollCallback(gWindow, UMouseScrollCallback);
	glfwSetInputMode(gWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
	meshes.CreateMeshes();

	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;


	// Load textures
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);


	// Load and bind desk texture
	if (!UCreateTexture("Textures/table-wood.jpg", gTextureIdDesk)) {
		std::cerr << "Failed to load desk texture" << std::endl;
		// Handle error gracefully
	}
	// Load and bind mug texture
	if (!UCreateTexture("Textures/cracked-white.jpg", gTextureIdMug)) {
		std::cerr << "Failed to load mug texture" << std::endl;
		// Handle error gracefully
	}
	// Load and bind botcap texture
	if (!UCreateTexture("Textures/black-pin.jpg", gTextureIdBotCap)) {
		std::cerr << "Failed to load botcap texture" << std::endl;
		// Handle error gracefully
	}
	// Load and bind penbod texture
	if (!UCreateTexture("Textures/pink-dot.jpg", gTextureIdPenBod)) {
		std::cerr << "Failed to load gTextureIdPenBod texture" << std::endl;
		// Handle error gracefully
	}
	// Load and bund bottle texture
	if (!UCreateTexture("Textures/sup-reme.jpg", gTextureIdBottl)) {
		std::cerr << "failed to get the bottle texture bruh" << std::endl;
	}
	// Load and bund container texture
	if (!UCreateTexture("Textures/aspire-logo.jpg", gTextureIdCon)) {
		std::cerr << "failed to get the container texture" << std::endl;
	}

	
	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);





	while (!glfwWindowShouldClose(gWindow))
	{
		// per-frame timing
	   // --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;
		// input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}


	//destroying textures
	meshes.DestroyMeshes();
	UDestroyTexture(gTextureIdDesk);
	UDestroyTexture(gTextureIdMug);
	UDestroyTexture(gTextureIdBotCap);
	UDestroyTexture(gTextureIdPenBod);
	UDestroyTexture(gTextureIdBottl);
	UDestroyTexture(gTextureIdCon);

	UDestroyShaderProgram(gProgramId);

	glfwTerminate();
	return EXIT_SUCCESS;
}


bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);


	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
	if (*window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return false;
	}

	if (glfwGetCurrentContext() == nullptr)
	{
		std::cerr << "Failed to create OpenGL context" << std::endl;
		glfwTerminate();
		return false;
	}

	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	return true;

}



void UProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// WASD
	float cameraOffset = gCameraSpeed * gDeltaTime;

	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessInput(FORWARD, cameraOffset);
	if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessInput(BACKWARD, cameraOffset);
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessInput(LEFT, cameraOffset);
	if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessInput(RIGHT, cameraOffset);
	if (glfwGetKey(gWindow, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessInput(UP, cameraOffset);
	if (glfwGetKey(gWindow, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessInput(DOWN, cameraOffset);
	// not much change from what I had previously built
}



void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; 

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);

	// https://stackoverflow.com/questions/66823783/toggle-between-ortho-and-perspective-views-in-opengl
}

void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.0)
		gCameraSpeed *= 2.1f;
	else if (yoffset < 0.0)
		gCameraSpeed /= 1.1f;
}

void URender()
{
	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 translation;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera/view transformation
	view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);

	// Creates an perspective projection
	view = gCamera.GetViewMatrix();
	projection = glm::perspective(glm::radians(60.0f), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	// Set the shader to be used

	glUseProgram(gProgramId);

	glUniform3fv(glGetUniformLocation(gProgramId, "viewPos"), 1, glm::value_ptr(gCameraPos));

	glUniform1i(glGetUniformLocation(gProgramId, "material.diffuse"), 0);
	glUniform1f(glGetUniformLocation(gProgramId, "material.shininess"), 32.0f);


	// directional light
	// this is literally from the first assignment - https://learnopengl.com/Lighting/Light-casters
	// directional - point came from a couple sources - https://www.reddit.com/r/opengl/comments/321c5r/gluniform3fv_vec3_myarray_and_confusion/
	// https://glm.g-truc.net/0.9.2/api/a00001.html
	// https://learnopengl.com/code_viewer.php?code=lighting%2Fmultiple_lights - just needed to slightly tweak based off the code found here

	glUniform3fv(glGetUniformLocation(gProgramId, "dirLight.direction"), 1, glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "dirLight.ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "dirLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.4f, 0.4f, 0.4f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "dirLight.specular"), 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
	glUniform1f(glGetUniformLocation(gProgramId, "dirLight.intensity"), 1.0f);



	// point light 1
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[0].position"), 1, glm::value_ptr(glm::vec3(0.0f, 3.0f, 0.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[0].ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[0].diffuse"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[0].specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[0].linear"), 0.09);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[0].quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[0].intensity"), 1.0f);

	// point light 2
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[1].position"), 1, glm::value_ptr(glm::vec3(-8.0f, 3.0f, -8.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[1].ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[1].diffuse"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[1].specular"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.0f)));
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[1].linear"), 0.09);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[1].quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[1].intensity"), 1.0f);

	// point light 3
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[2].position"), 1, glm::value_ptr(glm::vec3(8.0f, 3.0f, -8.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[2].ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[2].diffuse"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.8f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[2].specular"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.8f)));
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[2].linear"), 0.09);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[2].quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[2].intensity"), 1.0f);

	// point light 4
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[3].position"), 1, glm::value_ptr(glm::vec3(-8.0f, 3.0f, 8.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[3].ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[3].diffuse"), 1, glm::value_ptr(glm::vec3(0.0f, 0.8f, 0.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[3].specular"), 1, glm::value_ptr(glm::vec3(0.0f, 0.8f, 0.0f)));
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[3].linear"), 0.09);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[3].quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[3].intensity"), 1.0f);

	// point light 5
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[4].position"), 1, glm::value_ptr(glm::vec3(8.0f, 3.0f, 8.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[4].ambient"), 1, glm::value_ptr(glm::vec3(0.05f, 0.05f, 0.05f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[4].diffuse"), 1, glm::value_ptr(glm::vec3(0.8f, 0.0f, 0.0f)));
	glUniform3fv(glGetUniformLocation(gProgramId, "pointLights[4].specular"), 1, glm::value_ptr(glm::vec3(0.8f, 0.0f, 0.0f)));
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[4].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[4].linear"), 0.09);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[4].quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(gProgramId, "pointLights[4].intensity"), 1.0f);


	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniform1i(glGetUniformLocation(gProgramId, "hasTextureTransparency"), 0);

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//desk
	glBindVertexArray(meshes.gPlaneMesh.vao);
	scale = glm::scale(glm::vec3(8.0f, 8.0f, 8.0f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	model = scale * translation;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdDesk);
	glUniform1i(glGetUniformLocation(gProgramId, "hasTexture"), 1);
	glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

	//gTextureIdMug
	//mug
	glBindVertexArray(meshes.gCylinderMesh.vao);
	scale = glm::scale(glm::vec3(0.45f, 1.2f, 0.45f));
	translation = glm::translate(glm::vec3(-3.0f, 0.0f, 3.0f));
	model = translation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdMug);
	glUniform1i(glGetUniformLocation(gProgramId, "hasTexture"), 1);
	glUniform3fv(glGetUniformLocation(gProgramId, "meshColor"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	//gTextureIdPenTop
	//pen top
	glBindVertexArray(meshes.gSphereMesh.vao);
	scale = glm::scale(glm::vec3(0.04f, 0.04f, 0.04f));
	translation = glm::translate(glm::vec3(-1.52f, 0.04f, 2.07f));
	model = translation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdMug);
	glUniform1i(glGetUniformLocation(gProgramId, "hasTexture"), 1);
	glUniform3fv(glGetUniformLocation(gProgramId, "meshColor"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);

	//gTextureIdBotCap
	//bottle cap
	glBindVertexArray(meshes.gPyramid4Mesh.vao);
	translation = glm::translate(glm::vec3(0.0f, 0.9f, 2.0f));
	scale = glm::scale(glm::vec3(0.22f, 0.22f, 0.22f));
	model = translation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdBotCap);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid4Mesh.nVertices);	glBindVertexArray(0);


	//Pen body
	//gTextureIdPenBod
	glBindVertexArray(meshes.gCylinderMesh.vao);
	scale = glm::scale(glm::vec3(0.03f, 1.5f, 0.03f));
	rotation = glm::rotate(glm::radians(-70.0f), glm::vec3(0.0, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0, 0.0f, 1.0f));
	translation = glm::translate(glm::vec3(-1.0f, 0.03f, 3.5f));
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdPenBod);
	glUniform1i(glGetUniformLocation(gProgramId, "hasTexture"), 1);
	glUniform3fv(glGetUniformLocation(gProgramId, "meshColor"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	//gTextureIdBottl
	//bottle
	glBindVertexArray(meshes.gCylinderMesh.vao);
	scale = glm::scale(glm::vec3(0.15f, 0.8f, 0.15f));
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 2.0f));
	model = translation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdBottl);
	glUniform1i(glGetUniformLocation(gProgramId, "hasTexture"), 1);
	glUniform3fv(glGetUniformLocation(gProgramId, "meshColor"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	//container
	glBindVertexArray(meshes.gBoxMesh.vao);
	translation = glm::translate(glm::vec3(1.0f, 0.15f, 3.0f));
	scale = glm::scale(glm::vec3(1.0f, 0.15f, 0.6f));
	model = translation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindTexture(GL_TEXTURE_2D, gTextureIdCon);
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);


	glfwSwapBuffers(gWindow);
}

bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	int success = 0;
	char infoLog[512];

	programId = glCreateProgram();
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShaderId, 1, &vtxShaderSource, nullptr);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, nullptr);

	glCompileShader(vertexShaderId);
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(vertexShaderId); // Delete the shader object
		return false;
	}

	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(vertexShaderId);   // Delete the shader objects
		glDeleteShader(fragmentShaderId);
		return false;
	}

	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		glDeleteShader(vertexShaderId);   // Delete the shader objects
		glDeleteShader(fragmentShaderId);
		return false;
	}

	glDetachShader(programId, vertexShaderId);   // Detach the shader objects
	glDetachShader(programId, fragmentShaderId);
	glDeleteShader(vertexShaderId);   // Delete the shader objects
	glDeleteShader(fragmentShaderId);

	glUseProgram(programId);
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");


	return true;
}

void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}
/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{


		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 1);
		glBindTexture(GL_TEXTURE_2D, 2);

		return true;
	}

	// Error loading the image
	return false;
}
void UDestroyTexture(GLuint textureId)
{
	glDeleteTextures(1, &textureId);
}

// other non directly cited sources
// https://gamedev.stackexchange.com/questions/181782/how-can-i-change-the-camera-to-work-from-an-y-up-system-to-a-z-up
//  https://www.reddit.com/r/opengl/comments/xpplbw/cylinder_modern_opengl/
// https://learnopengl.com/
// https://www.reddit.com/r/GraphicsProgramming/comments/pq0z6k/just_started_learning_opengl_and_i_want_to_cry/ 
// https://www.youtube.com/watch?v=Q7vm264YNrM ( this guy's youtube videos were amazing )
// https://www.khronos.org/files/opengl45-quick-reference-card.pdf - Best cheat sheet I've found
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/ 
// Also should be noted I used SNHU tutorials 
// this is the V2 created on 10.17.23

