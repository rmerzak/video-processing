#include <iostream>
#include <stdio.h>
#include <GLFW/glfw3.h>
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
    glfwMakeContextCurrent(window);
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
    printf("Hello, World!\n");
    return 0;
}
