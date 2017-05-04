#include <stdint.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Color
{
	public:
		float R, G, B, A;

	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

	Color( const Color& other ) :
     	R( other.R ), G( other.G ), B( other.B ), A( other.A )
  	{}

	uint32_t ToUInt32() const
{
	uint32_t r = (uint32_t)(R * 255.0f);
	uint32_t g = (uint32_t)(G * 255.0f);
	uint32_t b = (uint32_t)(B * 255.0f);
	uint32_t a = (uint32_t)(A * 255.0f);

	return (a << 24) | (r << 16) | (g << 8) | b;

}

		Color operator + (const Color &c) const
{
	return Color(R + c.R, G + c.G, B + c.B, A + c.A);
}
		Color operator - (const Color &c) const
{
	return Color(R - c.R, G - c.G, B - c.B, A - c.A);
}

		Color operator * (float f) const
{
	return Color(R * f, G * f, B * f, A * f);
}

};


struct edge
{
	int yMin;
	int yMax;
	int x;
	int sign;
	int dX;
	int dY;
	int sum;

	Color maxColor;
	Color minColor;
	Color xColor;

	struct edge* next;
};

edge* createEdge(edge* from)
{
	edge* newEdge = new edge();
		newEdge->yMin = from->yMin;
		newEdge->yMax = from->yMax;
		newEdge->x = from->x;
		newEdge->dX = from->dX;
		newEdge->dY = from->dY;
		newEdge->sum = from->sum;
		newEdge->sign = from->sign;
		newEdge->maxColor = Color(from->maxColor);
		newEdge->minColor = Color(from->minColor);
		newEdge->xColor = Color(from->xColor);
}

edge* createEdge(int x1, int y1, int x2, int y2, const Color &color1, const Color &color2)
{
	edge* newEdge = new edge();
	if (y1 <= y2) {
		newEdge->yMin = y1;
		newEdge->yMax = y2;
		newEdge->x = x1;
		newEdge->dX = std::abs(x1 - x2);
		newEdge->dY = y2 - y1;
		newEdge->sum = 0;

		if (x1 < x2) {
			newEdge->sign = 1;
		}
		else
		{
			newEdge->sign = -1;
		}
		newEdge->minColor = color1;
		newEdge->maxColor = color2;
		newEdge->xColor = color1;
	}
	else
	{
		newEdge->yMin = y2;
		newEdge->yMax = y1;
		newEdge->x = x2;
		newEdge->dX = std::abs(x1 - x2);
		newEdge->dY = y1 - y2;
		newEdge->sum = 0;

		if (x1 < x2) {
			newEdge->sign = -1;
		}
		else
		{
			newEdge->sign = 1;
		}
		newEdge->minColor = color2;
		newEdge->maxColor = color1;
		newEdge->xColor = color2;
	}
	return newEdge;
}



class EdgeTable
{
public:
	edge *head;
	int listLength;

	EdgeTable() {
		listLength = 0;
		head = NULL;
	}

	bool removeEdge(edge* edgeToRemove)
	{
		if (edgeToRemove == head)
		{
			head = head->next;
		}
		else
		{
			edge *current = head;
			while (current->next != edgeToRemove)
			{
				current = current->next;
			}
			current->next = edgeToRemove->next;
			delete(edgeToRemove);
		}
	}


	void insertEdgeByY(edge * newEdge)
	{
		if (head == NULL || head->yMin >= newEdge->yMin)
		{

			newEdge->next = head;
			head = newEdge;
		}
		else
		{
			edge *current = head;
			while (current->next != NULL &&
				current->next->yMin < newEdge->yMin)
			{
				current = current->next;
			}

			newEdge->next = current->next;
			current->next = newEdge;
		}

		listLength++;
	}

	bool xCompare(edge* edge1, edge* edge2) {
		if (edge1->x < edge2->x) {
			return true;
		}
		else if (edge1->x > edge2->x) {
			return false;
		}
		else {
			return ((edge1->dX / edge1->dY) < (edge2->dX / edge2->dY));
		}
	}

void sort_list()
{
    edge* dumy_node = new edge();
    edge *cur_node = head;

    while (cur_node)
    {
        edge *insert_cur_pos =  dumy_node->next;
        edge *insert_pre_pos = NULL;

        while (insert_cur_pos)
        {
            if (xCompare(cur_node, insert_cur_pos))//insert_cur_pos->x > cur_node->x)
                break;

            insert_pre_pos = insert_cur_pos;
            insert_cur_pos = insert_cur_pos->next;
        }

        if (!insert_pre_pos)
            insert_pre_pos = dumy_node;

        edge *temp_node = cur_node->next;

        cur_node->next = insert_pre_pos->next;
        insert_pre_pos->next = cur_node;

        cur_node = temp_node;
    }

    head = dumy_node->next;
}

void deleteEdgesByKey(int key)
{
    // Store head node
    struct edge* temp = head, *prev;
 
    // If head node itself holds the key or multiple occurrences of key
    while (temp != NULL && temp->yMax == key)
    {
        head = temp->next;   // Changed head
        delete(temp);               // free old head
        temp = head;         // Change Temp
    }
 
    // Delete occurrences other than head
    while (temp != NULL)
    {
        // Search for the key to be deleted, keep track of the
        // previous node as we need to change 'prev->next'
        while (temp != NULL && temp->yMax != key)
        {
            prev = temp;
            temp = temp->next;
        }
 
        // If key was not present in linked list
        if (temp == NULL) return;
 
        // Unlink the node from linked list
        prev->next = temp->next;
 
        delete(temp);  // Free memory
 
        //Update Temp for next iteration of outer loop
        temp = prev->next;
    }
}

	bool empty()
	{
		return head == NULL;
	}
};

struct vertex
{
	
	glm::vec4 vec4;
	Color color;

	inline vertex(void) {}
	inline vertex(const float x, const float y, const float z, const float w)
	{
		vec4 = glm::vec4(x, y, z, w);
		color = Color(0,1,1);
	}

	inline vertex(const glm::vec4 v)
	{
		vec4 = v;
		color = Color(0,1,1);
	}

	inline vertex(const glm::vec4 v, const Color c)
	{
		vec4 = v;
		color = c;
	}

	inline vertex(const float x, const float y, const float z, const float w, const Color c)
	{
		vec4 = glm::vec4(x, y, z, w);
		color = c;
	}

	inline vertex operator + (const vertex& A) const
	{
		return vertex(vec4.x + A.vec4.x, vec4.y + A.vec4.y, vec4.z + A.vec4.z, vec4.w + A.vec4.w);
	}

	inline vertex operator + (const float A) const
	{
		return vertex(vec4.x + A, vec4.y + A, vec4.z + A, vec4.w + A);
	}

	inline float Dot(const vertex& A) const
	{
		return A.vec4.x*vec4.x + A.vec4.y*vec4.y + A.vec4.z*vec4.z + A.vec4.w*vec4.w;
	}
};

class Polygon {

public:
	struct vertex* vertices;
	int numberOfVertices;

	Polygon() {
		vertices = new vertex[10];
		numberOfVertices = 0;
	}

	void addVertex(float x, float y, float z, float w) {
		vertices[numberOfVertices] = vertex(x, y, z, w);
		numberOfVertices++;
	}

	void addVertex(const glm::vec4 v) {
		vertices[numberOfVertices] = vertex(v);
		numberOfVertices++;
	}

	void addVertex(float x, float y, float z, float w, Color c) {
		vertices[numberOfVertices] = vertex(x, y, z, w, c);
		numberOfVertices++;
	}

	void addVertex(const glm::vec4 v, Color c) {
		vertices[numberOfVertices] = vertex(v, c);
		numberOfVertices++;
	}

	EdgeTable* createEdgeTable()
	{
		EdgeTable *edgeTable = new EdgeTable;
	for(int i = 0; i < numberOfVertices; i++)
	{
		if(i+1 == numberOfVertices)
		{
     			edgeTable->insertEdgeByY(createEdge(vertices[i].vec4.x, vertices[i].vec4.y, 
						vertices[0].vec4.x, vertices[0].vec4.y, 
						vertices[i].color, 
						vertices[0].color));
		}
		else
		{
     			edgeTable->insertEdgeByY(createEdge(vertices[i].vec4.x, vertices[i].vec4.y, 
						vertices[i+1].vec4.x, vertices[i+1].vec4.y,  
						vertices[i].color, 
						vertices[i+1].color));
		}
	}
		return edgeTable;
	}

	int isOutside(vertex v)
	{
		if(v.vec4.x < -v.vec4.w) return 1;
		if(v.vec4.x > v.vec4.w) return 2;
		if(v.vec4.y < -v.vec4.w) return 3;
		if(v.vec4.y > v.vec4.w) return 4;
		if(v.vec4.z < -v.vec4.w) return 5;
		if(v.vec4.z > v.vec4.w) return 6;
		
		return 0;			
	}

	float calculateIntersectionRatio(vertex Vertex1, vertex Vertex2, int plane)
	{
		float t;
		 switch(plane)
			{
				case 1:
					t = (Vertex2.vec4.x + Vertex2.vec4.w) / (Vertex2.vec4.x - Vertex1.vec4.x + Vertex2.vec4.w - Vertex1.vec4.w);
					break;

				case 2:
					t = (Vertex2.vec4.x - Vertex2.vec4.w) / (Vertex2.vec4.x - Vertex1.vec4.x - Vertex2.vec4.w + Vertex1.vec4.w);

					break;

				case 3:
					t = (Vertex2.vec4.y + Vertex2.vec4.w) / (Vertex2.vec4.y - Vertex1.vec4.y + Vertex2.vec4.w - Vertex1.vec4.w);

					break;

				case 4:
					t = (Vertex2.vec4.y - Vertex2.vec4.w) / (Vertex2.vec4.y - Vertex1.vec4.y - Vertex2.vec4.w + Vertex1.vec4.w);

					break;

				case 5:
					t = (Vertex2.vec4.z + Vertex2.vec4.w) / (Vertex2.vec4.z - Vertex1.vec4.z + Vertex2.vec4.w - Vertex1.vec4.w);

					break;

				case 6:
					t = (Vertex2.vec4.z - Vertex2.vec4.w) / (Vertex2.vec4.z - Vertex1.vec4.z - Vertex2.vec4.w + Vertex1.vec4.w);

					break;
			}
		return t;
	}

	void clip()
	{
		struct vertex* newVertices = new vertex[20];
		int newCount = 0;

		for(int i = 0; i < numberOfVertices; i++)
		{
			vertex prev;
			vertex curr;
					
			if(i+1 == numberOfVertices)
			{
     				prev = vertices[i];
				curr = vertices[0];
			}
			else
			{
     				prev = vertices[i];
				curr = vertices[i+1];
			}
			
			std::cout << "checking" << std::endl;
			int outPrev = isOutside(prev);
			int outCurr = isOutside(curr);

			if (!outPrev && !outCurr)
			{
				std::cout << "both in" << std::endl;
				newVertices[newCount] = curr;
				newCount++;
			}
			else
			{
				if(!outPrev && outCurr)
				{
					float t = calculateIntersectionRatio(curr, prev, outCurr);
					std::cout << "ratio " << t << std::endl;

					vertex interVert = vertex(prev.vec4.x + (curr.vec4.x - prev.vec4.x) * t,
								prev.vec4.y + (curr.vec4.y - prev.vec4.y) * t,
								prev.vec4.z + (curr.vec4.z - prev.vec4.z) * t,
								prev.vec4.w + (curr.vec4.w - prev.vec4.w) * t,
								prev.color + (curr.color - prev.color) * t);
					newVertices[newCount] = interVert;
					newCount++;	
				}
				else
				{
					if(outPrev && !outCurr)
					{
						float t = calculateIntersectionRatio(curr, prev, outPrev);
						std::cout << "ratio " << t << std::endl;

						vertex interVert = vertex(prev.vec4.x - (prev.vec4.x - curr.vec4.x) * t,
									prev.vec4.y - (prev.vec4.y - curr.vec4.y) * t,
									prev.vec4.z - (prev.vec4.z - curr.vec4.z) * t,
									prev.vec4.w - (prev.vec4.w - curr.vec4.w) * t,
									prev.color - (prev.color - curr.color) * t);
						newVertices[newCount] = interVert;
						newCount ++;
						newVertices[newCount] = curr;
						newCount ++;
					}
					else
					{
						continue;
					}
				}

			}
		}

		delete(vertices);
		vertices = newVertices;
		numberOfVertices = newCount;
	}

	void divideByW()
	{
		for(int i = 0; i < numberOfVertices; i++)
		{
			vertices[i].vec4.x = vertices[i].vec4.x/vertices[i].vec4.w;
			vertices[i].vec4.y = vertices[i].vec4.y/vertices[i].vec4.w;
			vertices[i].vec4.z = vertices[i].vec4.z/vertices[i].vec4.w;
			vertices[i].vec4.w = vertices[i].vec4.w/vertices[i].vec4.w;
		}
	}

	void viewPort()
	{
		for(int i = 0; i < numberOfVertices; i++)
		{
			vertices[i].vec4.x = (int)((1.0+vertices[i].vec4.x)*800/2); // width
			vertices[i].vec4.y = (int)((1.0+vertices[i].vec4.y)*600/2); // width
			std::cout << "vertices " << vertices[i].vec4.x << ", " << vertices[i].vec4.y << std::endl;
		}
	} 
};

