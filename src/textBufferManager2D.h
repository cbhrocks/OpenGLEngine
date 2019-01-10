#ifndef PATHFINDER_TEXTBUFFERMANAGER2D_H
#define PATHFINDER_TEXTBUFFERMANAGER2D_H

class TextBufferManager2D {
public:
    GLuint texture;
    GLuint framebuffer;
    GLuint depthbuffer;
    size_t width;
    size_t height;

    TextBufferManager2D(){}

    TextBufferManager2D(size_t xSize, size_t ySize) {
        this->width = xSize;
        this->height = ySize;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);

        glGenRenderbuffers(1, &depthbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, xSize, ySize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

        checkGLError("Initial textbuffermanager");
    }

};


#endif //PATHFINDER_TEXTBUFFERMANAGER2D_H
