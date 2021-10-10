# PBR Model Viewer

A simple PBR model viewer made with DirectX 11 and Dear ImGui developed with the purpose of learning PBR and DirectX 11.

I usually build it with Visual Studio 2019, although any version that supports DirectX 11 should work. Just configure the correct SDK and retarget the solution inside visual studio if you need to.

When you load a mesh it uses a temporary shader until you load any PBR map (albedo, normal, metalness or roughness). At that point it changes into the proper PBR shader.
It uses Disney's BRDF, which should be the same as Unreal Engine's. However, I haven't implemented IBL and other effects yet, so it shouldn't look exactly the same.

## Usage

Run the program and go the 'File' menu in the top menu bar and select 'Open Mesh File...', then choose the desired mesh file and click 'OK'.

You can use the left mouse button to move the camera around the mesh and the mouse wheel to zoom in/out.
Because it rotates around the origin, it is recommended that the mesh you visualize is also centered at the origin.

You can also load a cubemap going to 'File' > 'Load Cubemap Folder' and selecting a folder which contains the cubemap. It expects a different image for each cubemap face with a specific name. In the solution folder you can find an example cubemap folder ready to use.

After you load a mesh, a window with several button will appear that allow you to load the different PBR maps.

The 'View' menu provides different visualization options. At the moment, you can toggle between visualizing the mesh in wireframe or solid mode, and toggle the cubemap on/off.

## Images

These are some example models viewed with this software.

![cerberus](/images/cerberus.png)

![rust](/images/rust.png)
