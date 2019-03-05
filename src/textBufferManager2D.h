#ifndef PATHFINDER_TEXTBUFFERMANAGER2D_H
#define PATHFINDER_TEXTBUFFERMANAGER2D_H

static const GLfloat quadVertexData[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

class TextBufferManager2D {
public:
	GLuint texture;
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
	GLuint VBO;
	size_t width;
	size_t height;
	Shader shader;

	TextBufferManager2D() {}

	TextBufferManager2D(size_t width, size_t height, Shader shader) :
		width(width), height(height), shader(shader)
	{
		cout << "Loaded OpenGL " << GLVersion.major << "." << GLVersion.minor << endl;

		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		createTexture();
		createRBO();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		checkGLError("Initial textbuffermanager");
	}

	void setShader(Shader shader) 
	{
		this->shader = shader;
	}

	void setDimensions(size_t width, size_t height)
	{
		this->width = width;
		this->height = height;
		this->createTexture();
	}

	void createTexture()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		checkGLError("create texture textbuffermanager");
	}

	void createRBO()
	{
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		checkGLError("create rbo textbuffermanager");
	}

	void createVAO()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

		glBindVertexArray(VAO);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//texture coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);

		checkGLError("create vao textbuffermanager");
	}

	void UploadUniforms()
	{
		this->shader.setInt("texId", 0);
	}

	void Draw()
	{
		this->shader.Use();
		this->UploadUniforms();
		glBindVertexArray(this->VAO);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		checkGLError("draw textbuffermanager");
	}
};


#endif //PATHFINDER_TEXTBUFFERMANAGER2D_H
