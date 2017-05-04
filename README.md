# graphics_pipeline-clipping-rasterization

This project is created for Computer Graphics course at AUA

For now it initializes a triangle in world coordinates, (see main())
does the MODEL-VIEWING-PERSPECTIVE transformation
creates a polygon and clips it according to 6 frustum planes
afterwards interpolates colors of each vertex and rasterizes the final polygon

Make sure yau have installed GLM and X11

to compile use g++ -I/PATH_TO_THE_HEADER -o poly poly.cpp -L/usr/X11/lib -lX11

Press 'c' to draw the initial polygon

You can test rasterization by clicking on the screen and defining points
press 'c' again to draw a polygon with random colors with those points

