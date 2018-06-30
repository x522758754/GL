#include "app.h"
#include "shader.h"
#include <model.h>
#include <vector>
#include <map>


class faceCulling : public app
{
	Model m;
	Shader shader;
	GLuint texture;
	GLuint VAO;


	void start()
	{
		//m = Model("_models/Cyborg/Cyborg.obj");
		shader = Shader("_shaders/_034_faceCulling.vs", "_shaders/_034_faceCulling.fs");
		shader.use();
		shader.setInt("texture_diffuse1", 0);

		texture = Utils::loadTexture("_texs/window.png");

		/*
		Remember: to specify vertices in a counter-clockwise winding order you need to visualize the triangle
		as if you're in front of the triangle and from that point of view, is where you set their order.

		To define the order of a triangle on the right side of the cube for example, you'd imagine yourself looking
		straight at the right side of the cube, and then visualize the triangle and make sure their order is specified
		in a counter-clockwise order. This takes some practice, but try visualizing this yourself and see that this
		is correct.
		*/

		float cubeVertices[] = {
			// Back face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
											  // Front face
											  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
											  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
											  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
											  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
											  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
											  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
																				// Left face
																				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
																				-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
																				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
																				-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
																				-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
																				-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
																												  // Right face
																												  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
																												  0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
																												  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
																												  0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
																												  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
																												  0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
																																				   // Bottom face
																																				   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
																																				   0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
																																				   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
																																				   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
																																				   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
																																				   -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
																																													 // Top face
																																													 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
																																													 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
																																													 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
																																													 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
																																													 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
																																													 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(0 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);


		glm::mat4 projection;
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.0f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.0f);
		shader.setMat4("projection", projection);
	
		glm::mat4 view;
		view = cam->GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 model;
		shader.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


};