# 3DGraphicEngine
Simple 3D Graphic Engine that enables construction and manipulation of Bezier and BSpline patches and splines. 

Features:
- Interface that allows interaction with a 3D space (movement, rotation, scaling)
- Stereoscopic View
- Cursor position on the scene
- Points Objects (adding, moveing, deleting)
- Adaptive object's drawing
- Displaing control points
- Bezier Curves of 3rd degree C0-continuous (adding, editing, moving, deleting)
- Bezier Curves of 3rd degree C2-continuous (adding, editing, moving, deleting) represented in Bernstein and B-spline basis
- Bezier Curves of 3rd degree C2-continuous interpolating given set of points (adding, editing, moving, deleting) - algorithm with linear space and time complexity
- Patch that consist of Bicubic C0-continuous patches - either in rectangle or cylinder form (adding, moving, deleting)
- Patch that consist of Bicubic C2-continuous patches - either in rectangle or cylinder form (adding, moving, deleting)
- Gregory Patches - holes filling between 3 Bicubic Bezier patches, C2-continuous
- Finding Intersections between 2 patches - Newtoon algorithm
- Trimming (partially finished)
- Scene Saving/Loading
- Generating Milling Machine pathes for a 3D object - works only for a provided Mixer object!
