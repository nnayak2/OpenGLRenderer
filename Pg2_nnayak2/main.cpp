#include "main.h"

//Define all key press handlers here. Declaration of extern in header
int light = 1;
int xx = 0, yy = 0, zz = 0, rot = 0;
float scale = 1.0f;
int activeTex = 0;

glm::vec3 eye, look, up;
float znear, zfar;
int pixwidth, pixheight;

//Define my light source here
GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat LightDiffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 5.0f, 0.0f, 1.0f };

struct lightSources
{
   glm::vec3 pos;
   glm::vec3 amb;
   glm::vec3 dif;
   glm::vec3 spec;
   //int shininess;
};

std::vector<lightSources> lights;

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		if (light) light = 0;
		else light = 1;
		break;

	case 'z':
		scale += 0.1f;
		break;
	case 'x':
		scale -= 0.1f;
		break;

	case '[':
		zz++;
		break;
	case ']':
		zz--;
		break;

	case 'q':
		rot++;
		break;
	case 'w':
		rot--;
		break;

   case ',':
      (activeTex - 1 < 0) ? 0 : activeTex--;
      break;
   case '.':
      (activeTex + 1 >= scene::getScene()->texNum) ? scene::getScene()->texNum - 1 : activeTex++;
      break;
	}
}

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		yy++;
		break;
	case GLUT_KEY_DOWN:
		yy--;
		break;
	case GLUT_KEY_LEFT:
		xx--;
		break;
	case GLUT_KEY_RIGHT:
		xx++;
		break;
	}
}

void draw()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	scene::getScene()->draw();
	glutSwapBuffers();
}

void redraw()
{
	glutPostRedisplay();
}

void initialiseGLUT(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(pixwidth, pixheight);
	glutCreateWindow("\t CSC561: Rasterization");

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

	//enable light
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);

	//glDisable(GL_BLEND);
   scene *scn = scene::getScene();
   scn->loadTextures();
   scn->setupLights();

   //Initialize the model view and projection matrices
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45, (float)1, 1.0, 5000.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(eye.x, eye.y, eye.z, look.x, look.y, look.z, up.x, up.y, up.z);
   //Some initial translation so cube is visible
   glTranslatef(0, 0, 5);

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialInput);
	glutDisplayFunc(draw);
	glutIdleFunc(redraw);
	glutMainLoop();
}

int main(int argc, char* argv[])
{
   //Read view and window text files for custom window
   std::ifstream fileIn;
   fileIn.open("./window.txt");
   if (!fileIn)
   {
      std::cout << "Could not find window.txt file, using assignment defaults." << std::endl;
      pixheight = 512;
      pixwidth = 512;
   }
   else
   {
      while (!fileIn.eof())
      {
         char line[50];
         fileIn.getline(line, 50);
         if (line[0] == 'p')
         {
            char *token = &line[2];
            _parseInt2(pixwidth, pixheight, token);
         }
      }
   }

   std::ifstream fileIn2;
   fileIn2.open("./view.txt");
   if (!fileIn2)
   {
      std::cout << "Could not find view.txt file, using assignment defaults." << std::endl;
      eye = glm::vec3(0, 0, -2);
      look = glm::vec3(0, 0, 1);
      up = glm::vec3(0, 1, 0);
      znear = 1;
      zfar = 3;
   }
   else
   {
      while (!fileIn2.eof())
      {
         char line[50];
         fileIn2.getline(line, 50);
         if (line[0] == 'e')
         {
            char *token = &line[2];
            _parseFloat3(eye.x, eye.y, eye.z, token);
         }
         if (line[0] == 'l')
         {
            char *token = &line[2];
            _parseFloat3(look.x, look.y, look.z, token);
         }
         if (line[0] == 'u')
         {
            char *token = &line[2];
            _parseFloat3(up.x, up.y, up.z, token);
         }
         if (line[0] == 'n')
         {
            char *token = &line[2];
            znear = _parseInt(token);
         }
         if (line[0] == 'f')
         {
            char *token = &line[2];
            zfar = _parseInt(token);
         }
      }
   }

   //Load the arbitrary lights
   std::ifstream fileIn3;
   fileIn3.open("./lights.txt");
   if (!fileIn3)
   {
      std::cout << "could not find lights.txt file, using defaults." << std::endl;
      //def light parameters
      lightSources temp;
      temp.pos = glm::vec3(0, 1, 2);
      temp.amb = glm::vec3(1, 1, 1);
      temp.dif = glm::vec3(1, 1, 1);
      temp.spec = glm::vec3(1.0, 1.0, 1.0);
      lights.push_back(temp);
   }
   else
   {
      while (!fileIn3.eof())
      {
         char line[50];
         fileIn3.getline(line, 50);
         lightSources temp;
         if (line[0] == 'p')
         {
            char *token = &line[2];
            _parseFloat3(temp.pos.x, temp.pos.y, temp.pos.z, token);

            for (int i = 0; i < 4; i++)
            {
               fileIn3.getline(line, 50);
               if (line[0] == 'a')
               {
                  char *token = &line[2];
                  _parseFloat3(temp.amb.x, temp.amb.y, temp.amb.z, token);
               }
               if (line[0] == 'd')
               {
                  char *token = &line[2];
                  _parseFloat3(temp.dif.x, temp.dif.y, temp.dif.z, token);
               }
               if (line[0] == 's')
               {
                  char *token = &line[2];
                  _parseFloat3(temp.spec.x, temp.spec.y, temp.spec.z, token);
               }
            }
            lights.push_back(temp);
         }

      }
   }

	//This class loads the obj file, and does the intersection calculations
	scene *scn = scene::getScene();

	std::string obj = "cube-textures.obj";

	//load the obj file
	scn->loadScene(const_cast<char*>(obj.c_str()));

	initialiseGLUT(argc, argv);
}