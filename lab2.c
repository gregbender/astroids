 /*
*	lab2.c
*   Greg Bender - gjb6676@cs.rit.edu
*   Computer Graphics I - Project 2
*   This is a program similar to the classic arcade game Asteroids
*   Use arrow keys to move around, and SPACEBAR to shoot
*/
#include <GL/glut.h>		// include glut library
#include<stdio.h>			// needed for printf function for testing
#include<math.h>			// needed for cos and sin functions
#include<malloc.h>			// functions needed for dynamic arrays
#include<stdlib.h>			// needed for absolute value function

/* DEFAULT SCREEN VARIABLES */
int screenHeight = 500;		// default screen height in pixels
int screenWidth = 500;		// default screen width in pixels
int viewNumber = 0;			// which scene to begin at
/* END DEFAULT SCREEN VARIABLES */

int keys[256];				// array used to hold if keys were pressed
int edge_counter = 1;			// used to make the line stippled edge animate

/* MOUSE TRACKING VARIABLES */
int lastPosition[3]	= {0, 0};		// keeps track of last mouse position x,y
/* END MOUSE TRACKING VARIABLES */

/* SHIP VARIABLES */
GLfloat shipHeight = 75.0f;		// default height of the ship
GLfloat shipWidth = 50.0f;		// default width of the ship

int thrust_time = 0;			// keeps track of how long to light up thruster
int thrust_time_max = 10;		// max time of thruster lightup
int thrust = 0;					// keeps track if spacebar (fire was pressed)

int turn_time = 0;				// keeps track of how long ship has been turning
int turn_time_max = 10;			// max time of turning animation
int turn_direction = 0;			// 1 is left, 2 is right hand turn
GLfloat turning_inc_deg = 5.0f;	// degrees to turn for each left/right button press
GLfloat turning_angle = 0.0f;	// angle used for turning animation
GLfloat turning_inc = 0.1f;		// how much to increment the turning angle by
GLfloat turning_angle_m = 0.5f;	// maximum angle allowed to turn at
GLfloat real_turn_angle = 0.0f; // turning angle correction, used for left&right turns

GLfloat angle_pointed = 0.0f;	// current angle ship is pointing
GLfloat acc_interval = 0.1f;    // a static variable that controls ship acceleration

GLfloat current_x = 0.0f;		// current x position of spacecraft
GLfloat current_y = 0.0f;		// current y position of spacecraft

GLfloat ship_center_x = 0.0f;	// center of the ship, x value
GLfloat ship_center_y = 0.0f;	// center of the ship, y value

GLfloat velocity_x = 0.0f;		// craft velocity in the x direction
GLfloat velocity_y = 0.0f;		// craft velocity in the y direction

GLfloat acc_x = 0.0f;			// craft acceleration in the x direction
GLfloat acc_y = 0.0f;			// craft acceleration in the y direction
int shields_on = 0;				// 0 if sheilds are off, 1 if they are one

// used to make the ship look different if the shields are on
GLubyte shield_stipple[] = {
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};
/* END SHIP VARIABLES */

/* STAR VARIABLES */
struct star {
	GLfloat xval;		// x position of star
	GLfloat yval;		// y position of star
	GLfloat xvol;		// x volocity
	GLfloat yvol;		// y velocity
};

struct star **starTable;	// holds the stars
int maxStars = 1000;	// maximum number of stars
/* END STAR VARIABLES */


/* BULLET VARIABLES */
struct bullet {
	GLfloat angle;		// angle bullet is currently flying
	GLfloat xspeed;		// speed in x direction
	GLfloat yspeed;		// speed in y direction
	GLfloat b_current_x;	// current x position
	GLfloat b_current_y;	// current y position
	GLfloat staticangle;
	int alive;		// is bullet still moving, or has it been destroyed
};

int bullet_size = 5.0f;		// size of each bullet
struct bullet **bulletTable;	// array to hold bullets
int maxBullets = 200;		// maximum number of bullets that can exist
GLfloat bulletSpeed = 3.5f;	// how fast should bullets go
int bullet_fired = 0;		// number of bullets fired
/* END BULLET VARIABLES */

/* ASTEROID VARIABLES */
struct asteroid {
	GLfloat a_current_x; // asteroid current x position
	GLfloat a_current_y; // asteroid current y position
	GLfloat a_current_z; // asteroid current z position
	GLfloat xspeed;		// speed in x direction
	GLfloat yspeed;		// speed in y direction
	GLfloat rotation;	// speed at which asteroid is rotating
	GLfloat rotationval; // current rotation degree of asteroid
	GLfloat size;		// size of asteroid based at it's center
	int alive;		// used to disable asteroid if it is hit
};

struct asteroid **asteroidTable;		// holds asteroid structs

int max_asteroids = 5;	// maximum possible asteroids
int asteroid_number = 0; // current number of asteroids that exist
/* END ASTROID VARIABLES */

/*
* this checks to see if the current arguments are about to cross the screen
* if it is, it reverses the velocity (and does so by passing the velocity by
* reference.  This way this function can be used for all elements (spaceship, bullets
* and asteroids)
*/
void check_cross_screen_x( GLfloat* val, GLfloat* val_velocity ) {

	if ( (*val >= (GLfloat)screenWidth - 30) || (*val <= -(GLfloat)screenWidth + 30) ) {
		*val_velocity = -*val_velocity;
		*val = *val + *val_velocity;
	}
} // end check_cross_screen_x

/*
* this checks to see if the current arguments are about to cross the screen
* if it is, it reverses the velocity (and does so by passing the velocity by
* reference.  This way this function can be used for all elements (spaceship, bullets
* and asteroids)
*/
void check_cross_screen_y( GLfloat* val, GLfloat* val_velocity ) {

	if ( (*val >= (GLfloat)screenHeight - 30) || (*val <= -(GLfloat)screenHeight + 30)) {
		*val_velocity = -*val_velocity;
		*val = *val + *val_velocity;
	}
} // end check_cross_screen_y

/*
* check to see if an asteroid and a bullet are touching
*/
asteroid_bullet_collide(int bullet_num, int asteroid_num) {

	// only check for collisiion if asteroid and bullet are 'alive'
	if ( (asteroidTable[asteroid_num]->alive == 1) && (bulletTable[bullet_num]->alive==1) ) {

		// find areas of each object
		GLfloat bullet_min_x = bulletTable[bullet_num]->b_current_x - bullet_size;
		GLfloat bullet_max_x = bulletTable[bullet_num]->b_current_x + bullet_size;
		GLfloat bullet_min_y = bulletTable[bullet_num]->b_current_y - bullet_size;
		GLfloat bullet_max_y = bulletTable[bullet_num]->b_current_y + bullet_size;

		GLfloat aster_min_x = asteroidTable[asteroid_num]->a_current_x - asteroidTable[asteroid_num]->size;
		GLfloat aster_max_x = asteroidTable[asteroid_num]->a_current_x + asteroidTable[asteroid_num]->size;
		GLfloat aster_min_y = asteroidTable[asteroid_num]->a_current_y - asteroidTable[asteroid_num]->size;
		GLfloat aster_max_y = asteroidTable[asteroid_num]->a_current_y + asteroidTable[asteroid_num]->size;

		// check for collision
		if ( (bullet_min_x > aster_min_x)
			&& (bullet_max_x < aster_max_x) &&
			(bullet_min_y > aster_min_y) &&
			(bullet_max_y < aster_max_y)) {

			// if there was a collision, turn off asteroid and bullet
			asteroidTable[asteroid_num]->alive = 0;
			bulletTable[bullet_num]->alive = 0;
		}
	}
} // asteroid bullet collide

/*
* check to see if an asteroid and the ship have collided
*/
void ship_asteroid_collide(int asteroid_num, GLfloat min_x, GLfloat max_x, GLfloat min_y, GLfloat max_y) {

	// only check for collisiion if asteroid are 'alive' and ships shield is down
	if ( (asteroidTable[asteroid_num]->alive == 1) && (shields_on == 0) ) {

		GLfloat aster_min_x = asteroidTable[asteroid_num]->a_current_x - asteroidTable[asteroid_num]->size;
		GLfloat aster_max_x = asteroidTable[asteroid_num]->a_current_x + asteroidTable[asteroid_num]->size;
		GLfloat aster_min_y = asteroidTable[asteroid_num]->a_current_y - asteroidTable[asteroid_num]->size;
		GLfloat aster_max_y = asteroidTable[asteroid_num]->a_current_y + asteroidTable[asteroid_num]->size;

		// check for collision
		if ( ((GLfloat)min_x > (GLfloat)aster_min_x) ) {

			if (((GLfloat)max_x < (GLfloat)aster_max_x)) {

				if (((GLfloat)min_y > (GLfloat)aster_min_y)){
					if ((((GLfloat)max_y < (GLfloat)aster_max_y))) {
						printf("SHIP DESTROYED");
					}
				}
			}
		}
	}
} // end ship_asteroid_collide

/*
* this fills the stars array with the initial stars to display
*/
void make_stars() {
	int i = 0;
	double randomh = 0.0f;
	double randomw = 0.0f;
	double posornegativex = 0.0f;
	double posornegativey = 0.0f;

	for (i = 0; i < maxStars; i++ ) {
		starTable[i] = malloc(sizeof(struct star));

		// used to make some of the stars appear in the other
		// four quadrants
		posornegativex = rand()%2;
		posornegativey = rand()%2;

		if ( posornegativex == 1.0f) {
			posornegativex = -1.0f;
		}
		else {
			posornegativex = 1.0f;
		}

		if ( posornegativey == 1.0f ) {
			posornegativey = -1.0f;
		}
		else {
			posornegativey = 1.0f;
		}

		randomw = (rand()%screenWidth) * posornegativex;
		randomh = (rand()%screenHeight) * posornegativey;

		// set star variables
		starTable[i]->xval = randomw;
		starTable[i]->yval = randomh;
		starTable[i]->xvol = 0.0f;
		starTable[i]->yvol = 0.0f;
	}
} // end make_stars

/*
* this function creates an asteroid
* and puts the asteroid in the asteroid array
*/
void create_asteroid(GLfloat x, GLfloat y, GLfloat z, GLfloat spin) {

	asteroidTable[asteroid_number] = malloc(sizeof(struct asteroid));

	asteroidTable[asteroid_number]->alive = 1;
	asteroidTable[asteroid_number]->a_current_x = x;
	asteroidTable[asteroid_number]->a_current_y = y;
	asteroidTable[asteroid_number]->a_current_z = z;
	asteroidTable[asteroid_number]->xspeed = spin * ((rand() % 5) + 2) / ((rand() % 5) + 2);
	asteroidTable[asteroid_number]->yspeed = spin * ((rand() % 5) + 2) / ((rand() % 5) + 2);
	asteroidTable[asteroid_number]->rotation = spin * ((rand() % 5) + 1) / ((rand() % 5) + 1);
	asteroidTable[asteroid_number]->rotationval = 0.0f;
	asteroidTable[asteroid_number]->size = (rand() % 75);
	asteroid_number++;
} // end create_asteroid

/*
* this increments the astroids on the screen and puts them in the next position
*/
void inc_asteroid() {

	int i = 0;
	for (i = 0; i < asteroid_number; i++ ) {

		// only bother doing it if asteroid is alive
		if ( (asteroidTable[i]->alive == 1)) {

			// next asteroid position is current position plus component speed
			asteroidTable[i]->a_current_x += asteroidTable[i]->xspeed;
			asteroidTable[i]->a_current_y += asteroidTable[i]->yspeed;
			asteroidTable[i]->rotationval += asteroidTable[i]->rotation;

			// make sure asteroid isn't attempting to cross the screen
			check_cross_screen_x(&asteroidTable[i]->a_current_x, &asteroidTable[i]->xspeed);
			check_cross_screen_y(&asteroidTable[i]->a_current_y, &asteroidTable[i]->yspeed);
		}
	}
} // end inc_asteroid

/*
* this displays an asteroid to the screen
*/
void display_asteroid() {

int i = 0;

	for (i = 0; i < asteroid_number; i++ ) {

		// only draw it if it is still alive
		if ( asteroidTable[i]->alive == 1 ) {

			GLfloat xval = asteroidTable[i]->a_current_x;
			GLfloat yval = asteroidTable[i]->a_current_y;
			GLfloat zval = asteroidTable[i]->a_current_z;
			GLfloat size = asteroidTable[i]->size;

			glPushMatrix();
			// MAKE a_current_x and a_current_y the center of the asteroid
			// and have a asteroid_radius value or something
			// and create the asteroid around the center

			glTranslatef( asteroidTable[i]->a_current_x, asteroidTable[i]->a_current_y, asteroidTable[i]->a_current_z);

			glRotatef(asteroidTable[i]->rotationval,1.0f,1.0f,1.0f);			// Rotate The Cube On X, Y & Z
			glTranslatef( -asteroidTable[i]->a_current_x, -asteroidTable[i]->a_current_y, -asteroidTable[i]->a_current_z);

			// draw asteroid
			glBegin(GL_QUADS);

			glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
			glVertex3f( size + xval, size + yval,-size + zval + 100);			// Top Right Of The Quad (Top)
			glVertex3f(-size + xval, size + yval,-size + zval);			// Top Left Of The Quad (Top)
			glVertex3f(-size + xval, size + yval, size + zval + 100);			// Bottom Left Of The Quad (Top)
			glVertex3f( size + xval, size + yval, size + zval);			// Bottom Right Of The Quad (Top)

			glColor3f(1.0f,0.5f,0.0f);			// Set The Color To Orange
			glVertex3f( size + xval,-size + yval, size + zval + 100);			// Top Right Of The Quad (Bottom)
			glVertex3f(-size + xval,-size + yval, size + zval);			// Top Left Of The Quad (Bottom)
			glVertex3f(-size + xval,-size + yval,-size + zval + 100);			// Bottom Left Of The Quad (Bottom)
			glVertex3f( size + xval,-size + yval,-size + zval);			// Bottom Right Of The Quad (Bottom)

			glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
			glVertex3f( size + xval, size + yval, size + zval + 100);			// Top Right Of The Quad (Front)
			glVertex3f(-size + xval, size + yval, size + zval);			// Top Left Of The Quad (Front)
			glVertex3f(-size + xval,-size + yval, size + zval + 100);			// Bottom Left Of The Quad (Front)
			glVertex3f( size + xval,-size + yval, size + zval);			// Bottom Right Of The Quad (Front)

			glColor3f(1.0f,1.0f,0.0f);			// Set The Color To Yellow
			glVertex3f( size + xval,-size + yval,-size + zval + 100);			// Bottom Left Of The Quad (Back)
			glVertex3f(-size + xval,-size + yval,-size + zval);			// Bottom Right Of The Quad (Back)
			glVertex3f(-size + xval, size + yval,-size + zval + 100);			// Top Right Of The Quad (Back)
			glVertex3f( size + xval, size + yval,-size + zval);			// Top Left Of The Quad (Back)

			glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
			glVertex3f(-size + xval, size + yval, size + zval + 100);			// Top Right Of The Quad (Left)
			glVertex3f(-size + xval, size + yval,-size + zval);			// Top Left Of The Quad (Left)
			glVertex3f(-size + xval,-size + yval,-size + zval + 100);			// Bottom Left Of The Quad (Left)
			glVertex3f(-size + xval,-size + yval, size + zval);			// Bottom Right Of The Quad (Left)

			glColor3f(1.0f,0.0f,1.0f);			// Set The Color To Violet
			glVertex3f( size + xval, size + yval,-size + zval + 100);			// Top Right Of The Quad (Right)
			glVertex3f( size + xval, size + yval, size + zval);			// Top Left Of The Quad (Right)
			glVertex3f( size + xval,-size + yval, size + zval + 100);			// Bottom Left Of The Quad (Right)
			glVertex3f( size + xval,-size + yval,-size + zval);			// Bottom Right Of The Quad (Right)

			glEnd();
			glPopMatrix();
		}
	}
} // end display_asteroid

/*
* this function determines the center of the spacecraft
* this is used as a result of poor design of the space craft,
* I should have centered it better
*/
int calc_ship_center_x() {
	GLfloat retval = current_x;
	return (retval);
}

/*
* this function determines the center of the spacecraft
* this is used as a result of poor design of the space craft,
* I should have centered it better
*/
int calc_ship_center_y() {
	GLfloat retval = current_y;
	return (retval);
}

/*
* this function is called whenever spacebar is pressed
* to fire a bullet..  it creates a bullet, initilizes it's structure
* and puts it in the bullet array
*/
void fire_bullet() {

	GLfloat bullet_pointed_deg = 0.0f;

	bulletTable[bullet_fired] = malloc(sizeof(struct bullet));

	if ( bullet_fired > (maxBullets - 1)) {
		bullet_fired = 0;
	}

	bulletTable[bullet_fired]->alive = 1;
	bulletTable[bullet_fired]->angle = angle_pointed;
	bulletTable[bullet_fired]->b_current_x = calc_ship_center_x();
	bulletTable[bullet_fired]->b_current_y = calc_ship_center_y();

	bullet_pointed_deg = bulletTable[bullet_fired]->angle * (3.141592654f / 180.0f);

	bulletTable[bullet_fired]->xspeed = bulletSpeed * cos(bullet_pointed_deg) + velocity_x;
	bulletTable[bullet_fired]->yspeed = bulletSpeed * sin(bullet_pointed_deg) + velocity_y;

	bulletTable[bullet_fired]->alive = 1;

	bullet_fired = bullet_fired + 1;
} // end fire_bullet

/*
* looks at each bullet, and incrememnts its position
*/
void increment_bullets() {

	int i = 0;
	if ( bullet_fired > 0 ) {

		// loop through each bullet
		for (i = 0; i < bullet_fired; i++ ) {

			// only bother doing it is bullet is still alive
			if ( (bulletTable[i]->alive==1) ) {

				// next position is current position plus component speed
				bulletTable[i]->b_current_x += bulletTable[i]->xspeed;
				bulletTable[i]->b_current_y += bulletTable[i]->yspeed;

				// see if the bullet is trying to cross the screen
				check_cross_screen_x(&bulletTable[i]->b_current_x, &bulletTable[i]->xspeed);
				check_cross_screen_y(&bulletTable[i]->b_current_y, &bulletTable[i]->yspeed);
			}
		}
	}
	glutPostRedisplay();		// tell screen it needs to be redrawn
} // end increment_bullets

/*
* draw edges
* this draws the edges of the screen
*/
void draw_edges() {

	GLushort pattern;

	if ( edge_counter == 1 ) {

		pattern = 0XF0F0;  /* pattern to be used for the line style is
				       1111000011110000 */
		edge_counter++;
	}
	else {
		pattern = 0x3F07;  /* pattern to be used for the line style is
				       1111000011110000 */
		edge_counter--;
	}

	glEnable ( GL_LINE_STIPPLE );
	  glLineStipple(1, pattern);
	  glLineWidth(4.0);

		glBegin( GL_LINE_STRIP );
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(-490.0f, -490.0f,-200.0f);
			glVertex3f(-490.0f, 490.0f,-200.0f);
			glVertex3f(490.0f, 490.0f,-200.0f);
			glVertex3f(490.0f, -490.0f,-200.0f);
			glVertex3f(-490.0f, -490.0f,-200.0f);
		glEnd();
	glDisable ( GL_LINE_STIPPLE );
} // end draw_edges

/*
* this draws stars in the background
*/
void draw_stars() {

		int i = 0;
		for (i=0; i < maxStars; i++ ) {

			glBegin( GL_POLYGON );
				glColor3f(1.0f, 1.0f, 1.0f);
   				glVertex3f(starTable[i]->xval, starTable[i]->yval, -10.0f);
   				glVertex3f(starTable[i]->xval, starTable[i]->yval+1.0f, -10.0f);
   				glVertex3f(starTable[i]->xval+1.0f, starTable[i]->yval+1.0f, -10.0f);
   				glVertex3f(starTable[i]->xval+1.0f, starTable[i]->yval, -10.0f);
			glEnd( );
		}
} // end draw stars

/*
*	function that is called whenever a mouse button is pressed
*   this is used to 'change' or switch between the 4 scenes
*/
void mouse( int button, int state, int x, int y ) {

	// remember where mouse was clicked
	lastPosition[0] = x;
	lastPosition[1] = y;

	// increment the viewNumber if a mouse button was pressed
	if ( state==GLUT_UP ) {

		// reset to first view
		if ( viewNumber == 2 ) {
			viewNumber = 0;
		}
		// or go to next view
		else {
			viewNumber++;
		}

	   	glutPostRedisplay();  // marks the window as needing to be redisplayed
	}
} // end mouse

/*
* this function calculates the acceleration, change in velocity
* as well as the spacecraft's new position.  The following website
* http://www.geocities.com/SiliconValley/Campus/9449/physics.htm
* had several concepts that helped me come up with this physics code
*/
void change_speed() {

	GLfloat scale_x = 0.0f;	// will be x component of acceleration
	GLfloat scale_y = 0.0f;	// will be y component of acceleration

	// convert angle_pointed from radians to degrees
	GLfloat angle_pointed_deg = angle_pointed * (3.141592654 / 180);

	// if the up arrow has been pressed (thrust)
	if ( thrust == 1 ) {

		// calculate x and y components of the thrust
		// by generating seperate x and y vectors
		scale_x = cos(angle_pointed_deg);
		scale_y = sin(angle_pointed_deg);

		// add it to the acceleration
		acc_x = acc_interval * scale_x;
		acc_y = acc_interval * scale_y;

		// set this variable so that the thrust animation
		// displays on the screen
		thrust_time = thrust_time_max;

		// turn off thruster
		thrust = 0;
	}

	// decrement thruster so it may stop displaying
	// on the screen
	thrust_time--;

	// add new acceleration to current velocity
	velocity_x = velocity_x + acc_x;
	velocity_y = velocity_y + acc_y;

	// calculate spacecraft's new position
	current_x = current_x + velocity_x;
	current_y = current_y + velocity_y;

	// now that acceleration has been added
	// set acceleration to zero
	acc_x = 0.0f;
	acc_y = 0.0f;

	// see if anything is attempting to 'cross the screen'
	// if it is, reverse it's current x,y, and velocity
	// this cases it to bounce back
	check_cross_screen_x(&current_x, &velocity_x);
	check_cross_screen_y(&current_y, &velocity_y);

} // end change_speed

/*
* this draws the bullets fired by the ship
*/
void drawBullets ( void ) {

	int i = 0;
	// loop through each bullet fired

	for (i = 0; i < bullet_fired; i++ ) {

		// only draw it if it is alive
		if ( bulletTable[i]->alive == 1 ) {

			glPushMatrix();
			// grab what things looked like before rotation and transformation

			glPolygonStipple(shield_stipple);
			glBegin( GL_POLYGON );
				glColor3f(0.0f, 1.0f, 0.0f);
   				glVertex3f(bulletTable[i]->b_current_x - bullet_size, bulletTable[i]->b_current_y - bullet_size, 0.0f);
   				glVertex3f(bulletTable[i]->b_current_x - bullet_size, bulletTable[i]->b_current_y + bullet_size, 0.0f);
   				glVertex3f(bulletTable[i]->b_current_x + bullet_size, bulletTable[i]->b_current_y + bullet_size, 0.0f);
   				glVertex3f(bulletTable[i]->b_current_x + bullet_size, bulletTable[i]->b_current_y - bullet_size, 0.0f);
			glEnd( );
			glPopMatrix();
		}
	}
} // end drawBullets

/*
* draw the spaceship centered at the provided parameters
*/
void draw_ship_at( GLfloat x, GLfloat y, GLfloat z ) {

		// the 3d position of the shaceship
		GLfloat z1coord = 0.0f;

		// draw ship differently if shields are on
		if ( shields_on == 1 ) {
			glEnable(GL_POLYGON_STIPPLE);
			glPolygonStipple(shield_stipple);
		}

		// draw the spaceship
		glColor3f(1.0f, 0.0f, 0.0f);

		glBegin(GL_TRIANGLES);
		glVertex3f(x - 25.0f, y - 40.0f, z);
		glVertex3f(x, y + 35.0f, z);
		glVertex3f(x + 25.0f, y - 40.0f, z);

	// START OTHER SIDES OF SPACECRAFT FOR TURNING
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(x - 25.0f, y - 40.0f, z - 20.0f);
		glVertex3f(x, y + 35.0f, z);
		glVertex3f(x + 25.0f, y - 40.0f, z - 20.0f);

	// END OTHER SIDES OF SPACECRAFT FOR TURNING

		// draw thrust behind ship if necessary
		if ( thrust_time > 0 ) {
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(x - 25.0f, y - 40.0f, z);
			glVertex3f(x, y - 80.0f, z);
			glVertex3f(x + 25.0f, y - 40.0f, z);
		}
	glEnd();

	  if ( shields_on == 1 ) {
		glDisable(GL_POLYGON_STIPPLE);
	  }
} // end draw_ship_at

/*
* this performs calculations to see if the spacecraft has turned
* and displays the 'sides' of the spaceship if it has turned
*/
void do_turning_calculations() {

	// this handles showing the side of the ship if
	// it is turning
	if ( (float)turn_time > (turn_time_max / 2.0) ) {

		if (turning_angle < turning_angle_m ) {
			turning_angle = turning_angle + turning_inc;
		}

		// fix angle for right hand turns
		if (turn_direction == 2 ) {
			real_turn_angle = 0.0f - turning_angle;
		}
		else {
			real_turn_angle = turning_angle;
		}

		turn_time = turn_time - 1;
	}
	else if ( (turn_time > 0) && ((float)turn_time <= (turn_time_max / 2.0))) {
		turning_angle = turning_angle + (-turning_inc);

		// fix angle for right hand turns
		if (turn_direction == 2 ) {
			real_turn_angle = 0.0f - turning_angle;
		}
		else {
			real_turn_angle = turning_angle;
		}
		turn_time--;
	}
} // end do_turning_calculations

/*
* this function handles the drawing of the ship and the drawing of
* the ships thrusters.  it also checks to see if bullets or asteroids
* need to be drawn
*/
void drawShip (void ) {

	// this translate fixes problem with rotating on the ships axis
	glTranslatef(current_x, current_y, 0.0f );

	// calculate spacecraft turns, and display the 'sides' of the ship if it
	// is turning
	do_turning_calculations();

	// rotate the ship based on arrow keys
	// the minus 90 degrees fixes a drawing problem
	glRotatef( angle_pointed - 90, real_turn_angle, 0.0f, 1.0f );

	// undo translate that was made to fix the rotation of the ship
	glTranslatef(  -current_x, -current_y, 0.0f );

	// draw the ship
	draw_ship_at(current_x, current_y, 0.0f);
} // end drawShip

/*
* this is the seperate part of draw world that doesn't actually draw
* anything, it just performs calculations for the next draw
* this was put in so i could have several orthos without messing up
* the drawing.
*/
void do_draw_calcs() {

	// init counters
	int i = 0;
	int j = 0;

	// calculate the change in speed and new position of spacecraft
	change_speed();

	// increment positions of the bullets
	increment_bullets();

	// increment positions of the asteroids
	inc_asteroid();

	// check for collisions
	// check each bullet against each asteroid
	for ( i = 0; i < bullet_fired; i++ ) {

		for( j = 0; j < asteroid_number; j++ ) {
			asteroid_bullet_collide(i, j);
		}
	}

	j = 0;
	// check for collisions
	// check for ship asteroid collision
	for( j = 0; j < asteroid_number; j++ ) {
		ship_asteroid_collide(j, (GLfloat)(calc_ship_center_x() - shipWidth), (GLfloat)(calc_ship_center_x() + shipWidth), (GLfloat)(calc_ship_center_y() - shipHeight), (GLfloat)(calc_ship_center_y() + shipHeight));
	}
} // do_draw_calcs()

/*
* this is the main drawing function.  It calls other functions that
* draw seperate elements of the screen.
* this draws the entire world, for the most part it just calls
* draw ship.  Other parts will be clipped away by the  display function
*/
void drawWorld( void ) {

	glPushMatrix();
	// draw the spaceship
	drawShip();
	glPopMatrix();

	// do bullet drawing
	if ( bullet_fired > 0 ) {
		drawBullets();
	}

	// do asteroid drawing
	if ( asteroid_number > 0 ) {
		display_asteroid();
	}

	// puts stars in the background
	glPushMatrix();
	draw_stars();
	glPopMatrix();

} // end drawWorld

/*
* this sets glColor to be a random color
*/
void random_color() {
	// make random color
	glColor3f((rand() % 256)/256.0f,(rand() % 256)/256.0f,(rand() % 256)/256.0f);
} // end random_color

/*
* generates a random x value for the screen
*/
int random_x() {

	int randomval = rand() % screenWidth;

	// maybe make it negative
	if ( (rand()%2) == 1 ) {
		randomval = -randomval;
	}

	return (randomval);
} // random_x

/*
* generates a random y value for the screen
*/
int random_y() {

	int randomval = rand() % screenHeight;

	// maybe make it negative
	if ( (rand()%2) == 1 ) {
		randomval = -randomval;
	}

	return (randomval);
} // random_y

/*
* generates a random value betwee -100 and 100
*/
random_small_val() {
	int randomval = rand() % 100;

	// maybe make it negative
	if ( (rand()%2) == 1 ) {
		randomval = -randomval;
	}

	return (randomval);
}

/*
* draw scene three
* this third scene was added to add in a few of the
* attributes that wern't included in the asteroids game
*/
void third_scene() {

		// generate starting x and y positions
		int x = random_x();
		int y = random_y();

		// this is used to slow down the animation, so we only animation ever
		// few scenes

		glBegin( GL_LINES );
			random_color();
			glVertex3f(x, y, -50.0f);
			glVertex3f(x - 50.0f, y - 50.0f, -50.0f);
			glVertex3f(x + 50.0f, y + 50.0f, -50.0f);
			glVertex3f(x + 100.0f, y + 100.0f, -50.0f);
		glEnd();

		// generate starting x and y positions
		x = random_x();
		y = random_y();
		glBegin( GL_TRIANGLE_STRIP );
			random_color();
			glVertex3f(x, y, -50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
		glEnd();

		// generate starting x and y positions
		x = random_x();
		y = random_y();
		glBegin( GL_TRIANGLE_FAN );
			random_color();

			glVertex3f(x, y,-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
		glEnd();

		// generate starting x and y positions
		x = random_x();
		y = random_y();
		glBegin( GL_QUAD_STRIP );
			random_color();

			glVertex3f(x, y, -50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
			glVertex3f(x + random_small_val(), y + random_small_val(),-50.0f);
		glEnd();

		random_color();
		glRectf(random_small_val(), random_small_val(), random_small_val(), 0.0f);
} // third_scene

/*
* This function is used to animation the screen.  It is called when everything
* is idle and nothing else is going on.
*/
void animate(void) {
	glutPostRedisplay();		// tell screen it needs to be redrawn
} // end animation

/*
* this is called every time the screen is displayed
* and it checks the keys array to see if things have changed
*/
void process_keys_array() {

		if ( keys[GLUT_KEY_LEFT] == 1 ) {
			angle_pointed += turning_inc_deg;
			turn_time = turn_time_max;
			turn_direction = 1;
			if (angle_pointed >= 360.0f) angle_pointed -= 360.0f ;
		}
		if ( keys[GLUT_KEY_RIGHT] == 1 ) {
			angle_pointed += -turning_inc_deg;
			turn_time = turn_time_max;
			turn_direction = 2;
			if (angle_pointed <= -360.0f) angle_pointed += 360.0f ;
		}
		if ( keys[GLUT_KEY_UP] == 1 ) {
			thrust = 1;
		}

} // end process_keys_array

/*
* This sets up the world, and calls a particular viewport depending on
* the viewnumber
*/
void display( void ) {

	// see if any keys were pressed
	process_keys_array();

	// set clear color to black
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	// clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// this does the calculations that need to be done before
	// the world is displayed.  this was put in so more than
	// one ortho can occur
	do_draw_calcs();

	// first scene
	if ( viewNumber == 0 ) {

		glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

		/* Set viewWindow - In this case, everything in the
  		 * world
		 */
		glOrtho( -500, 500, -500, 500, -500, 500 );

		/* Set viewport - In this case, the whole
		 * screen, although window resizing has not
		 * been handled
		 */
		glViewport( 0, 0, screenWidth, screenHeight );

		// draw the stippled edges of the screen
		glPushMatrix();
		draw_edges();
		glPopMatrix();

		// draw rest of world
		drawWorld();
	}
	// second scene
	else if ( viewNumber == 1 ) {

      	glMatrixMode (GL_MODELVIEW);
       	glLoadIdentity ();

		/* Set viewWindow - In this case, only the red
		 * square is selected, all else is clipped away
		 */
		glOrtho( -500, 500, -500, 500, -500, 500 );

		/* Set viewport - In this case, the middle half of the
		 * screen, although window resizing has not been
		 * handled
		 */
		glViewport( 0, 0, screenWidth, screenHeight );

		// draw the stippled edges of the screen
		glPushMatrix();
		draw_edges();
		glPopMatrix();

		drawWorld();

		// draw a border around the secondary window
		glLineWidth(1.0);
		glBegin( GL_LINE_LOOP );
			glColor3f(1.0f, 1.0f, 1.0f);

			glVertex3f(300.0f,300.0f,-50.0f);
			glVertex3f(300.0f,499.0f,-50.0f);
			glVertex3f(499.0f,499.0f,-50.0f);
			glVertex3f(499.0f,300.0f,-50.0f);
		glEnd();

		//glOrtho( -500, 500, -500, 500, -500, 500 );
		glViewport( 400, 400, 100, 100 );
		drawWorld();
	}
	// third scene
	else if ( viewNumber == 2 ) {
      	glMatrixMode (GL_MODELVIEW);
       	glLoadIdentity ();

		/* Set viewWindow - In this case, only the red
		 * square is selected, all else is clipped away
		 */
		glOrtho( -500, 500, -500, 500, -500, 500 );
		glViewport( 0, 0, screenWidth, screenHeight );
		third_scene();
	}

	// flush buffers
	glFlush();

	// swap buffers for animation
	glutSwapBuffers();
} // end display

/*
* mouse motion checks for mouse motion
*/
void mouseMotion( int x, int y ) {

    int 	currentPosition[2];
    int 	dx = 0;
    int 	dy = 0;

	currentPosition[0] = x;
	currentPosition[1] = y;

    dx = currentPosition[0] - lastPosition[0];
    dy = currentPosition[1] - lastPosition[1];

	// set spacecrafts new position
	current_x = dx;
	current_y = -dy;

	// redisplay the screen
    glutPostRedisplay();
} // end mouseMotion

/*
* this turns the shields on or off if a button is pressed
*/
void shields() {
	if ( shields_on == 1 ) {
		shields_on--;
	}
	else {
		shields_on++;
	}
} // end shields

/*
* callback function to handle 'regular' keyboard keys
*/
void keyboard ( unsigned char key, int x, int y ) {
	switch ( key ) {
		// if spacebar pressed, fire bullets
		case ' ':
			fire_bullet();
			break;
		case 's':
			shields();
			break;
		case 'S':
			shields();
			break;
       	default:
           	break;
   	}
} // end keyboard


/*
*   Deal with special key strokes (arrow keys)
*   and make changes in rotation or thrust
*/
void HandleSpecialKeyboard(int key,int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:
			keys[key] = 1;
			break;
		case GLUT_KEY_RIGHT:
			keys[key] = 1;
			break;
		case GLUT_KEY_UP:
			keys[key] = 1;
			break;
		case GLUT_KEY_DOWN:
			keys[key] = 1;
			break;
	}
	// redisplay the screen
	glutPostRedisplay();
} // end HandleSpecialKeyboard

/*
* this function is called whenever a key is released
* this allows us to have two keys down at the same time
* (allows the spacecraft to turn and accelerate at same time)
*/
void HandleSpecialKeyboardUP(int key,int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT:
			keys[key] = 0;
			break;
		case GLUT_KEY_RIGHT:
			keys[key] = 0;
			break;
		case GLUT_KEY_UP:
			keys[key] = 0;
			break;
		case GLUT_KEY_DOWN:
			keys[key] = 0;
			// do nothing when key down is pressed
			break;
	}

	// redisplay the screen
	glutPostRedisplay();
} // end HandleSpecialKeyboardUp

/*
*	this initilizes and creates the asteroids
*/
void asteroid_init() {

	int i = 0;
	GLfloat xcoord = 0.0f;
	GLfloat ycoord = 0.0f;
	GLfloat zcoord = 0.0f;
	GLfloat posornegativex = 0.0f;
	GLfloat posornegativey = 0.0f;
	for (i=0; i < max_asteroids; i++ ) {

		// used to make some of the stars appear in the other
		// four quadrants
		posornegativex = rand()%2;
		posornegativey = rand()%2;

		if ( posornegativex == 1.0f) {
			posornegativex = -1.0f;
		}
		else {
			posornegativex = 1.0f;
		}


		if ( posornegativey == 1.0f ) {
			posornegativey = -1.0f;
		}
		else {
			posornegativey = 1.0f;
		}

		xcoord = (rand()%(screenWidth - 50)) * posornegativex;
		ycoord = (rand()%(screenHeight - 50)) * posornegativey;

		create_asteroid(xcoord, ycoord, zcoord, posornegativey);
	}
} // end asteroid_init

/*
* displays controlls to the screen
*/
void display_controls() {
	// print movement instructions to the screen
	printf("Asteroids Controls\n");
	printf("-----------------------\n");
	printf("UP ARROW    - Fire Engines\n");
	printf("LEFT ARROW  - Turn Left\n");
	printf("RIGHT ARROW - Turn Right\n");
	printf("SPACEBAR    - Fire Weapon\n");
	printf("S KEY       - Toggle Shields\n\n");

	printf("LEFT MOUSE CLICK - Make a zoom window appear in top right of screen\n");
    printf("                 - and toggle between asteroids game and extra scene\n");
} // end display_controls

/*
*	main function -- initilizes openGL, registers callback functions
*   and starts display function
*/
int main( int argc, char** argv ) {

	// use a different method for keeping track of keys, so that we can
	// press two down at a time
	int i = 0;
	//int seed = 51;
	int seed = 52;
	for(i = 0; i < 256 ; i++ ) {
		keys[i] = 0;
	}

	// initilize random number generator
	srand(seed);

	// display game instructions to screen
	display_controls();

	// initilize array to hold bullets
	bulletTable = calloc(maxBullets, sizeof(struct bullet));

	// initilize array to hold asteroids
	asteroidTable = calloc(max_asteroids, sizeof(struct asteroid));

	// initilize array to hold stars and make stars
	starTable = calloc(maxStars, sizeof(struct star));
	make_stars();

 	glutInit( &argc, argv );

	// initilize and create the asteroids
	asteroid_init();

	// use double buffering for animation
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

	glutInitWindowSize( screenWidth, screenHeight );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "Greg Bender - Project2" );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_LINE_STIPPLE);
	glShadeModel( GL_SMOOTH );  // Smooth Shading

	// make things look nice
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// register callback functions
	glutKeyboardFunc(keyboard);	// KEYBOARD - glut keyboard function
	glutSpecialFunc(HandleSpecialKeyboard); // KEYBOARD - handle arrow keys

	// handle when a key is 'let go', this along with the keys array
	// allows us to have two keys pressed at the same time
	glutSpecialUpFunc(HandleSpecialKeyboardUP);

	glutMouseFunc( mouse );			// check for mouse buttons
    glutMotionFunc( mouseMotion );  // check for mouse movement
	glutDisplayFunc( display );		// run function 'display'
	glutIdleFunc( animate );			// set idle callback function

	// start main loop
	glutMainLoop();

	// free up used memory
	free(bulletTable);
	free(asteroidTable);
	free(starTable);
} // end main
