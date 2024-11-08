#include <iostream>
#include <stdio.h>
#include <GLFW/glfw3.h>
bool load_frames(const char* filename, int *width, int *height, unsigned char** data);
int main(int argc, const char** argv) {
    GLFWwindow* window;
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return 1;
    }
    window = glfwCreateWindow(640, 480, "Video Processing", NULL, NULL);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    int frame_width, frame_height;
    unsigned char* frame_data;
    if(!load_frames("../video/studie.mp4", &frame_width, &frame_height, &frame_data)) {
        printf("Failed to load frames\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    unsigned char* data = new unsigned char[100 * 100 * 3];
    for(int y = 0;y < 100;++y) {
        for(int x = 0;x < 100;++x) {
            if (y % 2 == 0) {
                data[y * 100 * 3 + x * 3 + 0] = 0xff;
                data[y * 100 * 3 + x * 3 + 1] = 0x00;
                data[y * 100 * 3 + x * 3 + 2] = 0x00;
            } else {
                data[y * 100 * 3 + x * 3 + 0] = 0x00;
                data[y * 100 * 3 + x * 3 + 1] = 0x00;
                data[y * 100 * 3 + x * 3 + 2] = 0x00;
            }
        }
    }
    GLuint text_handle;
    glGenTextures(1, &text_handle);
    glBindTexture(GL_TEXTURE_2D, text_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
        // set up orthographic projection
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho( 0,width, height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        // render whatever you like
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, text_handle);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(200, 200);
            glTexCoord2f(1, 0); glVertex2f(200 + frame_width, 200);
            glTexCoord2f(1, 1); glVertex2f(200 + frame_width, 200 + frame_height);
            glTexCoord2f(0, 1); glVertex2f(200, 200 + frame_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
    printf("Hello, World!\n");
    return 0;
}
