# DirectX Model Viewer

A simple model viewer made with DirectX 11 and Dear ImGui (v1.77) developed with the purpose of learning both of them.

It builds with Visual Studio 2019 (v142) and the Windows SDK v10.0, although any version of those that supports DirectX 11 should work.

Right now it can read meshes from an OBJ file, render them with a basic shader or in wireframe mode, and show its vertex normals.
You can also move the camera around the mesh in a Blender-like fashion.
It uses quaternions to do these rotations.

However, it is still not finished, although the difficult part is basically done, which is integrating Dear ImGui, importing OBJ files and rendering the meshes.
The things left to do are add more shaders to have different visualization options and materials.
I'm also planning on adding some gizmos and options to toggle them and the grid.

## Usage

Usage is simple, run the program and go the 'File' menu in the top menu bar and select 'Open File...', then choose the desired OBJ file and click 'OK'.

You can use the left mouse button to move the camera around the mesh and the mouse wheel to zoom in/out.
Because it rotates around the origin, it is recommended that the mesh you visualize is also centered at the origin.

The 'View' menu provides different visualization options. At the moment, you can toggle between visualizing the mesh in wireframe or solid mode, and toggle the visualization of vertex normals.

## Images

These are some example models viewed with this software. Keep in mind the shader used is intentionally ugly for debugging purposes and the wireframe mode uses the same shader as the solid mode.
There will be more visualization options once I finish it.

![diamond](/images/dragon.PNG)

A high polygon dragon

![diamond](/images/dragon_normals.PNG)

The normals of the previos dragon being visualized

![diamond](/images/dragon_wireframe.PNG)

The dragon visualized in wireframe

![file_chooser](/images/file_chooser.PNG)

The file chooser that pops up when you click 'Open File...'
