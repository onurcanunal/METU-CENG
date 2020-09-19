#include "Camera.h"

Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane)  // Image plane parameters
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
	int len, i=0;
	this->id = id;

	while(imageName[i] != '\0'){
		this->imageName[i] = imageName[i];
		i++;
	}
	this->imageName[i] = '\0';

	this->pos.x = pos.x;
	this->pos.y = pos.y;
	this->pos.z = pos.z;

	this->gaze.x = gaze.x;
	this->gaze.y = gaze.y;
	this->gaze.z = gaze.z;

	this->up.x = up.x;
	this->up.y = up.y;
	this->up.z = up.z;

	this->imgPlane = imgPlane;

	this->u_axis.x = up.y * -1 * gaze.z - up.z * -1 * gaze.y;
	this->u_axis.y = up.z * -1 * gaze.x - up.x * -1 * gaze.z;
	this->u_axis.z = up.x * -1 * gaze.y - up.y * -1 * gaze.x;

}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */

	Vector3f m;
	m.x = this->pos.x + this->gaze.x*this->imgPlane.distance;
	m.y = this->pos.y + this->gaze.y*this->imgPlane.distance;
	m.z = this->pos.z + this->gaze.z*this->imgPlane.distance;

	Vector3f q;
	q.x = m.x + this->imgPlane.left*this->u_axis.x + this->imgPlane.top*this->up.x;
	q.y = m.y + this->imgPlane.left*this->u_axis.y + this->imgPlane.top*this->up.y;
	q.z = m.z; //since q and m are point in the same plane parallel with up and u_axis vectors, z values are the same


	//x = col, y = row
	float s_u;
	float s_v;

	s_u = ((float) col+0.5)*((this->imgPlane.right - this->imgPlane.left)/this->imgPlane.nx);
	s_v = ((float) row+0.5)*((this->imgPlane.top - this->imgPlane.bottom)/this->imgPlane.ny);


	Vector3f s;
	s.x = q.x + s_u*this->u_axis.x - s_v*this->up.x;
	s.y = q.y + s_u*this->u_axis.y - s_v*this->up.y;
	s.z = q.z + s_u*this->u_axis.z - s_v*this->up.z;

	Vector3f direction;
	Vector3f origin;
	
	origin.x = this->pos.x;
	origin.y = this->pos.y;
	origin.z = this->pos.z;

	direction.x = s.x - this->pos.x;
	direction.y = s.y - this->pos.y;
	direction.z = s.z - this->pos.z;

	Ray ray(origin,direction);

	return ray;

}

