#include "GLUT.H"
#include <math.h>
#include <time.h>

const int GSIZE = 300;
const double PI = 3.14;
const int HEIGHT = 600;
const int WIDTH = 600;
const int NUM_PLANES = 3;
const int NUM_HELICOPTERS = 1;

double ground[GSIZE][GSIZE];

double dx=0, dy=0, dz=0;
double eyeX=2, eyeY=21, eyeZ=22;
double offset = 0;
double propalor = 0;
bool update = false;

// ego-motion
double sight_angle = PI;
double dir[3] = {0,0,-1};
double speed=0;
double angular_speed=0;
double pitch = 0;
struct AirMotion
	{
	// airplane motion
	double airsight_angle = PI;
	double airdir[3] = { 0,0,-1 };
	double airspeed = 0;
	double airangular_speed = 0;
	double airpitch = 0; // airplane pitch
	double airX = 0, airY = 15, airZ = 0;
};

AirMotion Planes[NUM_PLANES];
AirMotion Helicopter[NUM_HELICOPTERS];
// airplane motion
double airsight_angle = PI;
double airdir[3] = { 0,0,-1 };
double airspeed = 0;
double airangular_speed = 0;
double airpitch = 0; // airplane pitch
double airX = 0, airY = 15, airZ = 0;

// light parameters
float l0amb[4] = { 0.2,0.2,0.2,0 }; // ambient
float l0diff[4] = { 0.8,0.6,0.8,0 }; // diffuse
float l0spec[4] = { 0.8,0.8,0.8,0 }; // specular
float l0pos[4] = { 0,1,1,0 }; // position
// if l0pos[3] is 0 then the light is directional
// if l0pos[3] is 1 then the light is positional
float l1amb[4] = { 0.3,0.2,0.2,0 }; // ambient
float l1diff[4] = { 0.8,0.8,0.5,0 }; // diffuse
float l1spec[4] = { 0.8,0.8,0.8,0 }; // specular
float l1pos[4] = { 1,0,0,0 }; // position


// material parameters
// silver
float m0amb[4] = { 0.2,0.2,0.2,0 }; // ambient
float m0diff[4] = { 0.7,0.7,0.7,0 }; // ambient
float m0spec[4] = { 0.8,0.8,0.8,0 }; // ambient
// gold
float m1amb[4] = { 0.4,0.3,0.2,0 }; // ambient
float m1diff[4] = { 0.8,0.6,0.2,0 }; // ambient
float m1spec[4] = { 0.8,0.5,0.1,0 }; // ambient

void UpdateGround2();
void UpdateGround3();
void Smooth();

void init()
{
	int i, j;
	double dist;

	srand(time(0));

	for (i = 0; i < GSIZE; i++)
		for (j = 0; j < GSIZE; j++)
		{
/*			dist = sqrt(((double)GSIZE / 2 - i)*(GSIZE / 2 - i) + (GSIZE / 2 - j)*(GSIZE / 2 - j));
			ground[i][j] = 20 * cos(dist / 14.0 );
			*/
			ground[i][j] = 0;
		}
	// create random terrain
	for (i = 0; i < 2000; i++)
	{
		UpdateGround2();
		UpdateGround3();
	}

	Smooth();




	glClearColor(0.8,0.8,1,0); // sets baclground color 
	// allows for coser object to occlude the far object(s)
	glEnable(GL_DEPTH_TEST);
}

// completely random heights
void UpdateGround1() 
{
	int i, j;
	for (i = 0; i < GSIZE; i++)
		for (j = 0; j < GSIZE; j++)
			ground[i][j] += 0.1*(-1 + (rand() % 200) / 100.0);
}

void UpdateGround2()
{
	int i, j;
	int x1, y1, x2, y2;
	double a, b, delta = 0.1;

	x1 = rand() % GSIZE;
	y1 = rand() % GSIZE;
	x2 = rand() % GSIZE;
	y2 = rand() % GSIZE;

	if (x1 == x2) return;

	a = ((double)y2 - y1) / (x2 - x1);
	b = y1 - a*x1;

	if (rand() % 2 == 0) delta = -delta;

	for (i = 0; i < GSIZE; i++)
		for (j = 0; j < GSIZE; j++)
			if(i<a*j+b)
				ground[i][j] -= delta;	
			else
				ground[i][j] += delta;

}

void UpdateGround3()
{
	int x, y, num_pts = 1200;
	int i, j;
	double delta = 0.04;

	if (rand() % 2 == 0) delta = -delta;

	x = rand() % GSIZE;
	y = rand() % GSIZE;
	while (num_pts > 0)
	{
		num_pts--;
		ground[y][x] += delta;
		// choose a random direction
		switch (rand() % 4)
		{
		case 0: // go up
			y++;
			break;
		case 1: // go right
			x++;
			break;
		case 2: // go down
			y--;
			break;
		case 3 : // go left
			x--;
			break;
		}
		x = (x + GSIZE) % GSIZE;
		y = (y + GSIZE) % GSIZE;
	}
}

// low-pass filter
void Smooth()
{
	double tmp[GSIZE][GSIZE];
	int i, j;

	for (i = 1; i < GSIZE - 1; i++)
		for (j = 1; j < GSIZE - 1; j++)
			tmp[i][j] = (0.5*ground[i-1][j-1]+ ground[i - 1][j]+ 0.5*ground[i - 1][j + 1]+
				ground[i][j - 1] + 4*ground[i][j] + ground[i][j + 1]+
				0.5*ground[i + 1][j - 1] + ground[i + 1][j] + 0.5*ground[i + 1][j + 1]) / 10.0;

	for (i = 1; i < GSIZE - 1; i++)
		for (j = 1; j < GSIZE - 1; j++)
			ground[i][j] = tmp[i][j];

}

void SetColor(int i, int j)
{
	double h = ground[i][j]/7;

	// sand
	if(fabs(h)<0.1)
		glColor3d(0.8, 0.8, 0.6);	
	else
	if(fabs(h)<0.7)// grass
		glColor3d(0.9*fabs(h),0.8-0.7*fabs(h),0 );
	// snow
	else glColor3d(0.8*fabs(h), 0.8*fabs(h), fabs(h));
}


void DrawFloor()
{
	int i, j;

	glColor3d(0, 0, .4);

	for (i = 0; i < GSIZE-1; i++)
		for (j = 0; j < GSIZE-1; j++)
		{
			glBegin(GL_POLYGON);
			SetColor(i, j);
			glVertex3d(j-GSIZE/2, ground[i][j], i - GSIZE / 2);
			SetColor(i+1, j);
			glVertex3d(j - GSIZE / 2, ground[i+1][j], i+1 - GSIZE / 2);
			SetColor(i+1, j+1);
			glVertex3d(j+1 - GSIZE / 2, ground[i + 1][j+1], i + 1 - GSIZE / 2);
			SetColor(i, j+1);
			glVertex3d(j + 1 - GSIZE / 2, ground[i ][j + 1], i - GSIZE / 2);
			glEnd();
		}

	// water

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0, 0.2, 0.6,0.7);

	glBegin(GL_POLYGON);
		glVertex3d(-GSIZE / 2, 0, -GSIZE / 2);
		glVertex3d(GSIZE / 2, 0, -GSIZE / 2);
		glVertex3d(GSIZE / 2, 0, GSIZE / 2);
		glVertex3d(-GSIZE / 2, 0, GSIZE / 2);
	glEnd();
	glDisable(GL_BLEND);

}

void DrawAxes()
{
	glBegin(GL_LINES);
		// x : RED
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10, 0, 0);
		// y : GREEN
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10, 0);
		// Z : Magenta
		glColor3d(0.8, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10);
		glEnd();
	glLineWidth(1);

}

void DrawColorCube()
{
	// top
	glBegin(GL_POLYGON);//GL_LINE_LOOP);//
		glColor3d(1, 0, 0); // RED
		glVertex3d(1, 1, 1);
		glColor3d(1, 0, 1); // MAGENTA
		glVertex3d(1, 1, -1);
		glColor3d(0, 0, 1); // BLUE
		glVertex3d(-1, 1, -1);
		glColor3d(1, 1, 1); // WHITE
		glVertex3d(-1, 1, 1);
	glEnd();
	// bottom
	glBegin(GL_POLYGON);//GL_LINE_LOOP);//
		glColor3d(1, 1, 0); // YELLOW
		glVertex3d(1, -1, 1);
		glColor3d(0, 0, 0); // BLACK
		glVertex3d(1, -1, -1);
		glColor3d(0, 1, 1); // CYAN
		glVertex3d(-1, -1, -1);
		glColor3d(0, 1, 0); // GREEN
		glVertex3d(-1, -1, 1);
	glEnd();
	// right
	glBegin(GL_LINE_LOOP);//GL_POLYGON);
		glColor3d(1, 0, 0); // RED
		glVertex3d(1, 1, 1);
		glColor3d(1, 0, 1); // MAGENTA
		glVertex3d(1, 1, -1);
		glColor3d(0, 0, 0); // BLACK
		glVertex3d(1, -1, -1);
		glColor3d(1, 1, 0); // YELLOW
		glVertex3d(1, -1, 1);
	glEnd();
	// left
	glBegin(GL_LINE_LOOP);//GL_POLYGON);
		glColor3d(0, 1, 1); // CYAN
		glVertex3d(-1, -1, -1);
		glColor3d(0, 1, 0); // GREEN
		glVertex3d(-1, -1, 1);
		glColor3d(1, 1, 1); // WHITE
		glVertex3d(-1, 1, 1);
		glColor3d(0, 0, 1); // BLUE
		glVertex3d(-1, 1, -1);
	glEnd();
	// front
	glBegin(GL_LINE_LOOP);//GL_POLYGON);
	glColor3d(0, 1, 0); // GREEN
	glVertex3d(-1, -1, 1);
	glColor3d(1, 1, 1); // WHITE
	glVertex3d(-1, 1, 1);
	glColor3d(1, 0, 0); // RED
	glVertex3d(1, 1, 1);
	glColor3d(1, 1, 0); // YELLOW
	glVertex3d(1, -1, 1);
	glEnd();
	// rear
	glBegin(GL_LINE_LOOP);// GL_POLYGON);
		glColor3d(1, 0, 1); // MAGENTA
		glVertex3d(1, 1, -1);
		glColor3d(0, 0, 1); // BLUE
		glVertex3d(-1, 1, -1);
		glColor3d(0, 1, 1); // CYAN
		glVertex3d(-1, -1, -1);
		glColor3d(0, 0, 0); // BLACK
		glVertex3d(1, -1, -1);
	glEnd();


}

void DrawCylinder(int n)
{
	double alpha, teta=2*PI/n;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha)),1- fabs(cos(alpha)), fabs(cos(alpha)));
			glVertex3d(sin(alpha), 1, cos(alpha)); // 1
		glColor3d(fabs(sin(alpha+teta)), 1-fabs(cos(alpha + teta)), fabs(cos(alpha + teta)));
			glVertex3d(sin(alpha + teta), 1, cos(alpha + teta)); // 2
		glColor3d(0.7*fabs(sin(alpha + teta)), 1-0.5*fabs(cos(alpha + teta)), 1 - 0.5*fabs(sin(alpha + teta)));
			glVertex3d(sin(alpha + teta), -1, cos(alpha + teta)); // 3
		glColor3d(0.7*fabs(sin(alpha)), 1-0.5* fabs(cos(alpha)), 1 - 0.5*fabs(sin(alpha)));
			glVertex3d(sin(alpha), -1, cos(alpha)); // 4
		glEnd();
	}
}


void DrawCylinder1(int n, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		glBegin( GL_POLYGON);
		glNormal3d(sin(alpha), bottomr*(bottomr-topr),cos(alpha)); // normal to surface

			glColor3d(fabs(sin(alpha)), 1 - fabs(cos(alpha)), fabs(cos(alpha)));
		glVertex3d(topr*sin(alpha), 1, topr*cos(alpha)); // 1
			glColor3d(fabs(sin(alpha + teta)), 1 - fabs(cos(alpha + teta)), fabs(cos(alpha + teta)));
		glVertex3d(topr*sin(alpha + teta), 1, topr*cos(alpha + teta)); // 2
			glColor3d(0.7*fabs(sin(alpha + teta)), 1 - 0.5*fabs(cos(alpha + teta)), 1 - 0.5*fabs(sin(alpha + teta)));
		glVertex3d(bottomr*sin(alpha + teta), 0, bottomr*cos(alpha + teta)); // 3
			glColor3d(0.7*fabs(sin(alpha)), 1 - 0.5* fabs(cos(alpha)), 1 - 0.5*fabs(sin(alpha)));
		glVertex3d(bottomr*sin(alpha), 0, bottomr*cos(alpha)); // 4
		glEnd();
	}
}

void DrawConus(int n)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(0.2, 0.2 , 0.7);
		glVertex3d(0, 1, 0); // 1
		glColor3d(0.7*fabs(sin(alpha + teta)), 1 - 0.5*fabs(cos(alpha + teta)), 1 - 0.5*fabs(sin(alpha + teta)));
		glVertex3d(sin(alpha + teta), -1, cos(alpha + teta)); // 3
		glColor3d(0.7*fabs(sin(alpha)), 1 - 0.5* fabs(cos(alpha)), 1 - 0.5*fabs(sin(alpha)));
		glVertex3d(sin(alpha), -1, cos(alpha)); // 4
		glEnd();
	}
}

// n is amount of facets , slice is amount of layers
void DrawSphere(int n, int slices)
{
	double beta, teta = PI / slices;
	int i;

	for (beta = -PI / 2,i=0; beta < PI / 2; beta += teta,i++)
	{
			glPushMatrix();
			//		glRotated(i*offset, 0, 1, 0);
			glTranslated(0, sin(beta), 0);
			glScaled(1, sin(beta + teta) - sin(beta), 1);
			DrawCylinder1(n, cos(beta + teta), cos(beta));
			glPopMatrix();
	}
}

void DrawApple(int n, int slices)
{
	double beta, teta = PI / slices;
	int i;

	for (beta = 0, i = 0; beta < 2*PI ; beta += teta, i++)
	{
		glPushMatrix();
		//		glRotated(i*offset, 0, 1, 0);
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + teta) - sin(beta), 1);
		DrawCylinder1(n, 1+cos(beta + teta), 1+cos(beta));
		glPopMatrix();
	}
}

void DrawAirplane()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	glPushMatrix();
		glRotated(90, 0, 0, 1);
	// head
	glPushMatrix();
		glTranslated(0, 5, 0);
		glScaled(1, 6, 1);
		DrawCylinder1(50, 0, 1);
	glPopMatrix();

	// body
	glPushMatrix();
		glScaled(1, 5, 1);
		DrawCylinder1(50, 1, 1.5);
	glPopMatrix();

	// rear part of body
	glPushMatrix();
	glTranslated(0, -12, 0);
	glScaled(1, 12, 1);
	DrawCylinder1(50, 1.5, 0.7);
	glPopMatrix();

	// cockpit
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m1amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m1diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m1spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	glPushMatrix();
		glTranslated(0.15, 2.5, 0);
		glScaled(1.21, 5, 1.1);
		glRotated(90, 0, 1, 0);
		DrawSphere(40,6);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	// wings
	glNormal3d(1, 0, 0);
	glBegin(GL_POLYGON);
	glColor3d(0.9, 0.9, 1);
	glVertex3d(0, 3, 0);
	glVertex3d(0, -6, 6);
	glColor3d(0.4, 0.4, 1);
	glVertex3d(0, -8.5, 6);
	glVertex3d(0, -9.5, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3d(0.9, 0.9, 1);
	glVertex3d(0, 3, 0);
	glVertex3d(0, -6, -6);
	glColor3d(0.4, 0.4, 1);
	glVertex3d(0, -8.5, -6);
	glVertex3d(0, -9.5, 0);
	glEnd();

	// rear wing
	glNormal3d(0, 0, 1);
	glBegin(GL_POLYGON);
	glColor3d(0.9, 0.9, 1);
	glVertex3d(0, -2, 0);
	glVertex3d(4.5, -11, 0);
	glColor3d(0.4, 0.4, 1);
	glVertex3d(4.5, -13, 0);
	glVertex3d(0, -11, 0);
	glEnd();

	glPopMatrix();

}
void drawPropailor()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m1amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m1diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m1spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);


	glBegin(GL_POLYGON);
	glVertex3d(0, 0, -0.2);
	glVertex3d(0, 5, -0.2);
	glVertex3d(0, 5, 0.2);
	glVertex3d(0, 0, 0.2);
	glEnd();
}
void drawHelicopter()
{

	//head (CABIN) Color Gold
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m1amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m1diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m1spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);
	//Head

	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glRotated(180, 1, 0, 0);


	glPushMatrix();
	DrawSphere(50, 50);
	glPopMatrix();



	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	glPushMatrix();
	glScaled(1, 2, 1);
	DrawCylinder1(50, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 2, 0);
	DrawCylinder1(50, 0.4, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.5, 2, 0);
	glScaled(0.5, 2, 0.5);
	DrawCylinder1(50, 0.4, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 2, 0);
	glScaled(1, 2, 0.5);
	DrawCylinder1(50, 0.4, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 3, 0);
	glScaled(1, 4, 1);
	DrawCylinder1(50, 0.1, 0.4);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.7, 1, 0);
	glRotated(90, 0, 0, 1);
	glScaled(0.5, 0.5, 0.5);
	DrawCylinder1(50, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.5, 1, 0);
	glRotated(90, 0, 0, 1);
	glScaled(0.2, 1, 0.2);
	DrawCylinder1(50, 1, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.7, 1, 0);
	glRotated(90, 0, 0, 1);
	glScaled(0.5, 0.1, 0.5);
	DrawSphere(50, 2);
	glPopMatrix();

	//Wing

	glBegin(GL_POLYGON);
	glVertex3d(0, 6, 0);
	glVertex3d(0, 7, 0);
	glVertex3d(0, 7, 1);
	glVertex3d(0, 6.7, 1);

	glVertex3d(0, 6, 0);
	glVertex3d(0, 7, 0);
	glVertex3d(0, 7, -1);
	glVertex3d(0, 6.7, -1);

	glVertex3d(0, 6, 0);
	glVertex3d(0, 7, 0);
	glVertex3d(1, 7, 0);
	glVertex3d(1, 6.7, 0);


	glEnd();

	glPushMatrix();
	glTranslated(1.5, 1, 0);
	glRotated(propalor, 1, 0, 0);

	glRotated(0, 1, 0, 0);
	drawPropailor();
	glRotated(90, 1, 0, 0);
	drawPropailor();
	glRotated(180, 1, 0, 0);
	drawPropailor();
	glRotated(270, 1, 0, 0);
	drawPropailor();
	glPopMatrix();


	glPopMatrix();
}
void DrawControl()
{
	glColor3d(1, 1, 0);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glColor3d(0, 0, 0);

	glBegin(GL_LINES);
	glVertex2d(0, -1);
	glVertex2d(0, 1);
	glEnd();

	glColor3d(1, 0, 1);
	double h = airpitch;
	glBegin(GL_POLYGON);
	glVertex2d(-0.2, h-0.1);
	glVertex2d(-0.2, h+0.1);
	glVertex2d(0.2, h+0.1);
	glVertex2d(0.2, h-0.1);
	glEnd();

}
void display()
{  // loads background color and sets Z to infinity for each pixel
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glViewport(0, 0, WIDTH , HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyeX, eyeY, eyeZ, // eye coordinates (COP)
		eyeX+dir[0], eyeY+dir[1], eyeZ+dir[2], // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing
	DrawFloor();
	DrawAxes();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	for (i = 0; i < NUM_PLANES; i++)
	{
		glPushMatrix();
		glTranslated(Planes[i].airX - 20 + i*20, Planes[i].airY, Planes[i].airZ);
		glRotated(Planes[i].airsight_angle * 180 / PI + 90, 0, 1, 0);
		glRotated(Planes[i].airpitch * 180 / PI, 0, 0, 1);
		glRotated(Planes[i].airangular_speed * 4000, 1, 0, 0);
		//	glScaled(3, 3, 3);
		//	DrawAirplane();
		//DrawAirplane();
		glPopMatrix();
	}

	glPushMatrix();
	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airpitch * 180 / PI, 0, 0, 1);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	drawHelicopter();
	glPopMatrix();

	glDisable(GL_LIGHTING);

	// remote control
	glViewport(0, 0, 100, 200);
	// prepare to 2D
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // start transformations
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // start transformations
	DrawControl();
	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void display_top()
{  // loads background color and sets Z to infinity for each pixel
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(0, 150, 0, // eye coordinates (COP)
		0, 50 , -0.5, // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing

	DrawFloor();
	DrawAxes();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	//	glScaled(3, 4, 3);

	DrawAirplane();
	glDisable(GL_LIGHTING);
	glutSwapBuffers();
}

void display_cockpit()
{  // loads background color and sets Z to infinity for each pixel
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 2, 300);
	gluLookAt(airX, airY+3, airZ, // eye coordinates (COP)
		airX + airdir[0], airY+2.7+airdir[1], airZ + airdir[2], // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing

	DrawFloor();
	DrawAxes();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);


	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	//	glScaled(3, 4, 3);

	DrawAirplane();
	glDisable(GL_LIGHTING);
	glutSwapBuffers();
}

void display_combined()
{  // loads background color and sets Z to infinity for each pixel
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// REGULAR VIEW
	glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyeX, eyeY, eyeZ, // eye coordinates (COP)
		eyeX + dir[0], eyeY + dir[1], eyeZ + dir[2], // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing

	DrawFloor();
	DrawAxes();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);


	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	//	glScaled(3, 4, 3);

	DrawAirplane();
	glDisable(GL_LIGHTING);
	// TOP VIEW
	glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(0, 80, 0, // eye coordinates (COP)
		0, 50, -0.5, // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing

	DrawFloor();
	DrawAxes();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);


	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	//	glScaled(3, 4, 3);

	DrawAirplane();
	glDisable(GL_LIGHTING);
	// COCKPIT VIEW
	glViewport(0, 0, WIDTH, HEIGHT / 2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 2, 300);
	gluLookAt(airX, airY + 3, airZ, // eye coordinates (COP)
		airX + airdir[0], airY + 2.7 + airdir[1], airZ + airdir[2], // Point Of Interest
		0, 1, 0); // vector UP
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// start drawing

	DrawFloor();
	DrawAxes();


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);

	// define light parameters
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1pos);
	// define Material parameters
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m0amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m0diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m0spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100);


	glTranslated(airX, airY, airZ);
	glRotated(airsight_angle * 180 / PI + 90, 0, 1, 0);
	glRotated(airangular_speed * 4000, 1, 0, 0);
	//	glScaled(3, 4, 3);

	DrawAirplane();
	glDisable(GL_LIGHTING);
	glutSwapBuffers();
}

void idle()
{
	int choice,i;
	offset += 0.2;
	if (update)
	{
		UpdateGround2();
		UpdateGround3();
	}


	//CAMERA:
	// update motion and location
	sight_angle += angular_speed;
	dir[0] = sin(sight_angle);  // x
	dir[1] = sin(pitch);// y
	dir[2] = cos(sight_angle);// z

	eyeX += speed*dir[0];
	eyeY += speed*dir[1]; //pitch
	eyeZ += speed*dir[2];

	for (i = 0; i < NUM_PLANES; i++)
	{
		// creating random Movement for plane !
		choice = rand() % 6;  //0-5 0 included'd

		switch (choice)
		{
		case(0): // move LEFT
			Planes[i].airangular_speed += 0.0002;
			break;
		case(1): // move Right
			Planes[i].airangular_speed -= 0.0002;
			break;
		case(2): // move Forward
			Planes[i].airspeed += 0.01;
			if (Planes[i].airspeed >= 0.15)
				Planes[i].airspeed = 0.15;
			break;
		case(3): // move BackWard
			Planes[i].airspeed -= 0.001;
			break;
		case(4): // move BackWard
			Planes[i].airpitch += 0.01;
			break;
		case(5): // move BackWard
			Planes[i].airpitch -= 0.01;
			break;
		}


		// airplane motion
		Planes[i].airsight_angle += Planes[i].airangular_speed;
		Planes[i].airdir[0] = sin(Planes[i].airsight_angle);  // x
		Planes[i].airdir[1] = -sin(Planes[i].airpitch);// y
		Planes[i].airdir[2] = cos(Planes[i].airsight_angle);// z

		Planes[i].airX += Planes[i].airspeed*Planes[i].airdir[0];
		Planes[i].airY += Planes[i].airspeed*Planes[i].airdir[1];
		Planes[i].airZ += Planes[i].airspeed*Planes[i].airdir[2];
	}



	airsight_angle += airangular_speed;
	airdir[0] = sin(airsight_angle);  // x
	airdir[1] = -sin(airpitch);// y
	airdir[2] = cos(airsight_angle);// z

	airX += airspeed*airdir[0];
	airY += airspeed*airdir[1];
	airZ += airspeed*airdir[2];

	propalor += 20;

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angular_speed+=0.001;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.001;
		break;
	case GLUT_KEY_UP:
		speed+=0.01;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.01;
		break;
	case GLUT_KEY_PAGE_UP:
		pitch += 0.01;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pitch -= 0.01;
		break;
	}
}

void mouse(int button, int state, int x, int y)
{


}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		airangular_speed += 0.002;
		break;
	case 'w':
		airspeed += 0.01;
		break;
	case 's':
		airspeed -= 0.01;
		break;
	case 'd':
		airangular_speed -= 0.002;
		break;
	case 'r':
		airY += 0.1;
		break;
	case 'f':
		airY -= 0.1;
		break;
	case 'i':
		Smooth();
		break;
	case 'u':
		update = !update;
		break;
	}
}

void menu(int choice)
{
	switch (choice)
	{
	case 1:
		glutDisplayFunc(display);
		break;
	case 2:
		glutDisplayFunc(display_top);
		break;
	case 3:
		glutDisplayFunc(display_cockpit);
		break;
	case 4:
		glutDisplayFunc(display_combined);
		break;
	}
}

void drag(int x, int y)
{
	double xx, yy; // the logical coordinates
	xx = 2 * (x / 100.0) - 1;
	yy = 2 * ((HEIGHT-y) / 200.0) - 1;

	if (xx >= -0.2 && xx <= 0.2 && yy >= airpitch - 0.1 && yy <= airpitch + 0.1)
	{
		if(airpitch < 0.95)
			airpitch = yy;
		else airpitch = 0.94;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Graphics example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Regular view", 1);
	glutAddMenuEntry("Top view", 2);
	glutAddMenuEntry("Cockpit view", 3);
	glutAddMenuEntry("Combined view", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();
	glutMainLoop();
}