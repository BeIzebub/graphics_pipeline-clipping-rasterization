#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <cmath>
#include <iostream>     // std::cout
#include <algorithm>    // std::min
#include <defs.h>
#include <vector>

#include <unistd.h>

char WINDOW_NAME[] = "Graphics Window";
char ICON_NAME[] = "Icon";
Display *display;
int screen;
Window main_window;
GC gc;
unsigned long foreground, background;
int width = 800;
int height = 600;

Polygon *polygon = new Polygon;


void fillPolygon()
{
	EdgeTable *edgeTable= polygon->createEdgeTable();

	int scanLine = edgeTable->head->yMin;
	edge* e1;
	edge* e2;
	EdgeTable *activeList = new EdgeTable();

	while (!edgeTable->empty()) {
		// Remove edges from active list if y == ymax
		if (!activeList->empty()) {
			edgeTable->deleteEdgesByKey(scanLine);
			activeList->deleteEdgesByKey(scanLine);
		}

		// Add edges from ET to AL if y == ymin
		edge *current = edgeTable->head;
		while (current != NULL)
		{
			if (current->yMin == scanLine) {
				printf("inserting edge\n");
				activeList->insertEdgeByY(createEdge(current));
			}
			current = current->next;
		}

		// Sort by x & slope
		activeList->sort_list();

		// Fill polygon pixels
		current = activeList->head;
		printf("filling...\n");
		while (current != NULL)
		{
			e1 = current;
			e2 = current->next;

			int xdiff = e2->x - e1->x;
    			if(xdiff == 0)
        			break;

    			Color colordiff = e2->xColor - e1->xColor;
    			float factor = 0.0f;
    			float factorStep = 1.0f / (float)xdiff;
 			for(int x = e1->x; x < e2->x; x++) {
	
				XSetForeground(display, gc, (e1->xColor+(colordiff*factor)).ToUInt32());
				XDrawPoint(display, main_window, gc, x, scanLine);
        			factor += factorStep;
    			}
			
			if(e2->next == NULL)
				break;
			else
				current = e2->next;
		}

		// Increment scanline
		scanLine++;

		// Incremement the X variables based on the slope
		current = activeList->head;
		printf("incrementing xes\n");
		while (current != NULL)
		{
			if (current->dX != 0) 
			{
                		current->sum += current->dX;
                
                		while (current->sum >= current->dY) {
                   			 current->x += (current->sign);
                			 current->sum -= current->dY;
                		}	
            		}
			int y = scanLine - current->yMin;
			
			Color colorDiff = current->maxColor-current->minColor;
			current->xColor = current->minColor + (colorDiff*((float)y/current->dY));
			current = current->next;
		}
	}

	delete(polygon);
	delete(edgeTable);
	delete(activeList);
	polygon = new Polygon;
}


void connectX()
{
   display = XOpenDisplay(NULL);
   if (display == NULL) {fprintf(stderr, "Cannot connect to X\n");
                         exit(1);}
   screen = DefaultScreen(display);
   foreground = BlackPixel(display, screen);
   background = WhitePixel(display, screen);
}

Window openWindow(int x, int y, int width, int height,
                  int border_width, int argc, char** argv)
{
    Window new_window;
    XSizeHints size_hints;
    new_window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                 x, y, width, height, border_width, foreground,
                 background);
 size_hints.x = x;
 size_hints.y = y;
 size_hints.width = width;
 size_hints.height = height;
 size_hints.flags = PPosition | PSize;
    XSetStandardProperties(display, new_window, WINDOW_NAME, ICON_NAME,
                          None, argv, argc, &size_hints);
    XSelectInput(display, new_window, (ButtonPressMask | KeyPressMask |
                                      ExposureMask | PointerMotionMask));
    return (new_window);
}

GC getGC()
{
    GC gc;
    XGCValues gcValues;
    gc = XCreateGC(display, main_window, (unsigned long)0, &gcValues);
    XSetBackground(display, gc, background);
    XSetForeground(display, gc, foreground);
    return (gc);
}

void disconnectX()
{
    XCloseDisplay(display);
    exit(0);
}


void draw()
{
    XFlush(display);
}

void doButtonPressEvent(XButtonEvent *pEvent)
{
   int x;
   int y;
  
  
   x = pEvent->x;
   y = pEvent->y;
	srand( time(NULL) ); //Randomize seed initialization
	polygon->addVertex(x, y, 10, 1.0, Color(rand() % 2,rand() % 2,rand() % 2, 1.0));

   XDrawPoint(display, main_window,gc,x,y);
   printf("Mouse clicked at %d,%d\n", x,y);
   /* disconnectX();*/
   printf("Button pressed\n");
}

void doExposeEvent(XExposeEvent *pEvent)
{
   printf("Expose Event occurred\n");
   draw();
}
 
void doKeyPressEvent(XKeyEvent *pEvent)
{
    int key_buffer_size = 10;
    char key_buffer[9];
    XComposeStatus compose_status;
    KeySym key_sym;
    XLookupString(pEvent, key_buffer, key_buffer_size,
                  &key_sym, &compose_status);
    if (key_buffer[0] == 'q') {
	disconnectX();
    }
    else
    if(key_buffer[0] == 'c') {


	fillPolygon();
    }
    else printf("You pressed %c\n", key_buffer[0]);
}


int main (int argc, char** argv){
  XEvent event;
  connectX();
  main_window = openWindow(10,20,width,height,5, argc, argv);
  gc = getGC();
  XMapWindow(display, main_window);
  draw();
  XFlush(display);

	std::vector<glm::vec4> verts;
	
	glm::vec4 vert1 = glm::vec4(0  ,  0,  0,  1);
	glm::vec4 vert2 = glm::vec4(0  ,  1,  0,  1);
	glm::vec4 vert3 = glm::vec4(1  ,  1,  0,  1);
	//glm::vec4 vert4 = glm::vec4(1  ,  0.5,  0,  1);

	verts.push_back(vert1);
	verts.push_back(vert2);
	verts.push_back(vert3);
	//verts.push_back(vert4);

	std::vector<Color> colors;
	colors.push_back(Color(1.0, 0.0, 0.0, 1.0));
	colors.push_back(Color(0.0, 1.0, 0.0, 1.0));
	colors.push_back(Color(0.0, 0.0, 1.0, 1.0));

	glm::vec3 cam = glm::vec3(0,  0,  0);

	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)); 
	//pMat4 = glm::scale(pMat4, glm::vec3(2.0f, 2.0f, 2.0f)); 
	//pMat4 = glm::rotate(pMat4, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f)); 

	glm::mat4 lookAt = glm::lookAt (cam, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 3.0f / 4.0f, 0.5f, 10.0f);

	glm::mat4 MVP = projection * lookAt * trans;

	for(std::vector<glm::vec4>::size_type i = 0; i != verts.size(); i++)
	{
		verts[i] = MVP * verts[i];

		polygon->addVertex(verts[i].x, verts[i].y, verts[i].z, verts[i].w, colors[i]);
	}

	std::cout << "cilpping" << std::endl;
	polygon->clip();
	polygon->divideByW();
	polygon->viewPort();


  while (True){

	XNextEvent(display, &event);
		switch (event.type) {
		case Expose:
			doExposeEvent (&event.xexpose);
			break;
		case ButtonPress:
			printf("Button Pressed\n");
			doButtonPressEvent(&event.xbutton);
			break;		
		case KeyPress:
     			printf("Key pressed\n");
     			doKeyPressEvent(&event.xkey);
     			break; 
    	}
  }
	  	
}
