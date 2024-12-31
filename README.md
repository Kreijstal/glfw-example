# Red Circle

This project demonstrates a simple OpenGL ES application that renders a red circle on a black background. It uses GLFW for window and input management, GLAD for loading OpenGL ES functions, and ANGLE for OpenGL ES emulation on Windows. The project is built using CMake and is designed to be straightforward to set up using `pacman` (or `pacboy`) within the MSYS2 environment.

## Dependencies

You can install all the required dependencies using a single command within the MSYS2 environment (make sure you're in a MinGW-w64 shell, e.g., UCRT64 or CLANG64):

```bash
pacman -S mingw-w64-x86_64-{cmake,glfw,angleproject,python-glad,cc} git
```

## Building the Project

1. **Clone the repository:**

2. **Generate build files with CMake:**

    From the project's root directory (not inside the `build` directory), run: This project uses ANGLE by default.

    ```bash
    cmake -B build
    ```

    This will create a `build` subdirectory and generate the necessary build files (e.g., Makefiles) inside it.

3. **Build the project:**

    ```bash
    cmake --build build
    ```

    This will compile the code and create the `red_circle.exe` executable within the `build` directory.

## Running the Application

After building, navigate to the `build` directory and run the executable from your MSYS2 terminal:

```bash
./build/red_circle.exe
```

You should see a window with a red circle drawn in the center.

### Using GLFW

This project uses GLFW to handle window creation and input. Here are some basic GLFW features used:

*   **Window Creation:** `glfwCreateWindow` creates the application window.
*   **Input Handling:**
    *   `glfwPollEvents` processes events like keyboard and mouse input.
    *   `glfwWindowShouldClose` checks if the window should be closed (e.g., if the user pressed the close button).
*   **Context Management:** `glfwMakeContextCurrent` makes the OpenGL context current for rendering.
*   **Buffer Swapping:** `glfwSwapBuffers` swaps the front and back buffers to display the rendered frame.


## Notes

*   The shader code targets GLSL ES 1.00 for compatibility with a wide range of ANGLE versions.
*   You can modify the shader files (`.glsl`) and see the changes without recompiling the C++ code, provided you don't alter the input/output structure of the shaders (e.g., adding/removing attributes or uniforms).
*   The project includes basic error handling for GLFW and OpenGL. You might want to add more comprehensive error handling for production-ready applications.
