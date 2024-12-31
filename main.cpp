#include <iostream>
#include "rendering.hpp"

// Include necessary headers for Windows and EGL
#ifdef USING_ANGLE
#include <windows.h>
#include <EGL/egl.h>
#endif
#include "glad/gles2.h"
// GLFW error callback function
static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    std::cerr << "  (Error code: 0x" << std::hex << error << ")" << std::dec << std::endl;
}

int main() {
    // Set the error callback before glfwInit()
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

#ifdef USING_ANGLE
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    std::cout << "Using ANGLE for rendering" << std::endl;
#endif

    // Print GLFW version
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "GLFW Version: " << major << "." << minor << "." << rev << std::endl;

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Red Circle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        int errorCode = glfwGetError(nullptr);
        std::cerr << "  GLFW Error Code: 0x" << std::hex << errorCode << std::dec << std::endl;

        switch (errorCode) {
            case GLFW_NOT_INITIALIZED:
                std::cerr << "  GLFW was not initialized." << std::endl;
                break;
            case GLFW_INVALID_ENUM:
                std::cerr << "  Invalid window hint provided." << std::endl;
                break;
            case GLFW_API_UNAVAILABLE:
                std::cerr << "  Requested OpenGL ES API is unavailable." << std::endl;
#ifdef USING_ANGLE
                std::cerr << "  Make sure ANGLE libraries are correctly linked and DLLs are accessible." << std::endl;
#endif
                break;
            case GLFW_VERSION_UNAVAILABLE:
                std::cerr << "  Requested OpenGL ES version is unavailable." << std::endl;
                break;
            case GLFW_PLATFORM_ERROR: {
                std::cerr << "  A platform-specific error occurred." << std::endl;
#ifdef _WIN32
                // Now LPSTR, DWORD, etc. should be defined
                LPSTR messageBuffer = nullptr;
                DWORD errorMessageID = ::GetLastError();
                if (errorMessageID != 0) {
                    FormatMessageA(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

                    if (messageBuffer) {
                        std::cerr << "  Windows Error: " << messageBuffer << std::endl;
                        LocalFree(messageBuffer);
                    }
                }
#endif
                break;
            }
            default:
                std::cerr << "  Unknown GLFW error." << std::endl;
                break;
        }

        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

#ifdef USING_ANGLE
    // Now PFNEGLGETPROCADDRESSPROC, EGLDisplay, eglGetDisplay, etc. should be defined
    typedef void* (*PFNEGLGETPROCADDRESSPROC)(const char*);
    PFNEGLGETPROCADDRESSPROC my_eglGetProcAddress = nullptr;

    std::cout << "Attempting to get EGL display..." << std::endl;
    EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    std::cout << "eglGetDisplay returned: " << eglDisplay << std::endl;

    if (eglDisplay == EGL_NO_DISPLAY) {
        std::cerr << "Failed to get EGL display." << std::endl;
    } else {
        std::cout << "Attempting to initialize EGL..." << std::endl;
        EGLBoolean eglInitialized = eglInitialize(eglDisplay, nullptr, nullptr);
        std::cout << "eglInitialize returned: " << (eglInitialized == EGL_TRUE ? "true" : "false") << std::endl;

        if (eglInitialized) {
            std::cout << "Attempting to query EGL version..." << std::endl;
            const char* eglVersion = eglQueryString(eglDisplay, EGL_VERSION);
            std::cout << "eglQueryString (EGL_VERSION) returned: " << (eglVersion ? eglVersion : "nullptr") << std::endl;

            std::cout << "Attempting to query EGL vendor..." << std::endl;
            const char* eglVendor = eglQueryString(eglDisplay, EGL_VENDOR);
            std::cout << "eglQueryString (EGL_VENDOR) returned: " << (eglVendor ? eglVendor : "nullptr") << std::endl;

            // Now HMODULE, LoadLibraryA, etc. should be defined
            HMODULE libEGL = LoadLibraryA("libEGL.dll");
            if (libEGL) {
                std::cout << "Successfully loaded libEGL.dll" << std::endl;
                my_eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC)GetProcAddress(libEGL, "eglGetProcAddress");
                if (my_eglGetProcAddress != nullptr) {
                    std::cout << "my_eglGetProcAddress found" << std::endl;
                }
                if (!my_eglGetProcAddress) {
                    std::cerr << "Failed to get eglGetProcAddress from libEGL.dll" << std::endl;
                }
            } else {
                std::cerr << "Failed to load libEGL.dll" << std::endl;
            }
        } else {
            std::cerr << "Failed to initialize EGL." << std::endl;
        }
    }
    // Now gladLoadGLES2 should be defined
    if (my_eglGetProcAddress) {
        gladLoadGLES2((GLADloadfunc)my_eglGetProcAddress);
    } else {
        std::cerr << "Cannot load GL functions because my_eglGetProcAddress is null." << std::endl;
    }
#endif

    // Initialize rendering (shaders, buffers, etc.)
    if (!initializeRendering()) {
        std::cerr << "Failed to initialize rendering." << std::endl;
        glfwTerminate();
        return -1;
    }

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        renderFrame();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    cleanupRendering();
    glfwTerminate();

    return 0;
}
