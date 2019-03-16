#ifndef __GLHELPER
#define __GLHELPER

#include <iostream>

static void checkGLError(char const * ident = "")
{
	GLenum errCode;
	
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL Error (%s): ", ident);
		switch (errCode)
		{
		case GL_INVALID_ENUM:
			fprintf(stderr, "GL_INVALID_ENUM\n");
			break;
		case GL_INVALID_VALUE:
			fprintf(stderr, "GL_INVALID_VALUE\n");
			break;
		case GL_INVALID_OPERATION:
			fprintf(stderr, "GL_INVALID_OPERATION\n");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION\n");
			break;
		case GL_OUT_OF_MEMORY:
			fprintf(stderr, "GL_OUT_OF_MEMORY\n");
			break;
		}
    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Frame buffer setup failed : ");
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
            fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
        if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
            fprintf(stderr, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
        if(status == GL_FRAMEBUFFER_UNSUPPORTED)
            fprintf(stderr, "GL_FRAMEBUFFER_UNSUPPORTED\n");
    }
}

#endif

