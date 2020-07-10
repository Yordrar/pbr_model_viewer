# DirectX Model Viewer

A simple model viewer made with DirectX 11 and Dear ImGui (v1.77) with the purpose of learning both of them.

It builds with Visual Studio 2019 (v142) and the Windows SDK v10.0, although any version of those that supports DirectX 11 should work.

It is still not finished, although the difficult part is basically done, which is integrating Dear ImGui, importing OBJ files and rendering the meshes.
The things left to do are add more shaders to have different visualization options and materials, and  use XInput to rotate the camera around th mesh using the mouse.

Usage is simple, run the program and go the 'File' menu in the top menu bar and select 'Open File...', then choose the desired OBJ file and click 'OK'.

## Images

These are some example models viewed with this software. Keep in mind the shader used is intentionally ugly for debugging purposes, there will be more visualization options once I finish it.

![diamond](/images/diamond.PNG)

![file_chooser](/images/file_chooser.PNG)
