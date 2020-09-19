#include "Light.h"

/* Constructor. Implemented for you. */
PointLight::PointLight(const Vector3f & position, const Vector3f & intensity)
    : position(position), intensity(intensity)
{
}

// Compute the contribution of light at point p using the
// inverse square law formula
Vector3f PointLight::computeLightContribution(const Vector3f& p)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
	Vector3f contribution;

	float distance = sqrt(pow(p.x - this->position.x, 2) + pow(p.y - this->position.y, 2) + pow(p.z - this->position.z, 2));
	contribution.r = intensity.r / pow(distance, 2);
	contribution.g = intensity.g / pow(distance, 2);
	contribution.b = intensity.b / pow(distance, 2);

	return contribution;
}

