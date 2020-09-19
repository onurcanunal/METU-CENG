#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"


#include "Matrix4.h"

using namespace std;

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;
	int projectionType;

	vector< vector<Color> > image;
	vector< Camera* > cameras;
	vector< Vec3* > vertices;
	vector< Color* > colorsOfVertices;
	vector< Scaling* > scalings;
	vector< Rotation* > rotations;
	vector< Translation* > translations;
	vector< Model* > models;

	Scene(const char *xmlPath);

	void initializeImage(Camera* camera);
	void forwardRenderingPipeline(Camera* camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);

	void transformVertices(Camera *camera, vector< vector<bool> > &visibleTriangles);
	void overrideMatrix(Matrix4 m1, Matrix4 m2);
	Matrix4 modelingTransformation(int modelIndex);
	Matrix4 cameraTransformation(Camera *camera);
	Matrix4 projectionTransformation(Camera *camera);
	Matrix4 viewportTransformation(Camera *camera);
	bool visible(double den, double num, double& t_e, double& t_l);
	void midpointAlgorithm(Camera *camera, Vec3 point0, Vec3 point1, bool isClipped, vector< Color > colorsOfLines);
	void drawLine(Camera *camera, vector<Vec3> twoDVertices, vector< vector<bool> >visibleTriangles, int modelIndex, vector< Color > colorsOfLines, vector<bool> isTriangleClippedVector, vector< vector<Vec3> > linesOfModel);
	int clippingAlgorithm(Vec4 v0, Vec4 v1, Vec4& new0, Vec4& new1, Color& c0, Color& c1);
	double f(double x, double y, Vec3 point0, Vec3 point1);
	double findMaxX(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2);
	double findMinX(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2);
	double findMaxY(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2);
	double findMinY(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2);
	void triangleRasterization(Camera *camera, Vec3 firstVertex, Vec3 secondVertex, Vec3 thirdVertex);
};

#endif
