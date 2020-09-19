#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "Scene.h"
#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "tinyxml2.h"
#include "Helpers.h"

using namespace tinyxml2;
using namespace std;

/*
	Transformations, clipping, culling, rasterization are done here.
	You can define helper functions inside Scene class implementation.
*/

double Scene::f(double x, double y, Vec3 point0, Vec3 point1){
	double x0 = point0.getElementAt(0);
	double y0 = point0.getElementAt(1);
	double x1 = point1.getElementAt(0);
	double y1 = point1.getElementAt(1);

	return x*(y0-y1) + y*(x1-x0) + x0*y1 - y0*x1;
}

double Scene::findMaxX(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2){
	double v0 = vertex0.getElementAt(0);
	double v1 = vertex1.getElementAt(0);
	double v2 = vertex2.getElementAt(0);
	double maxX;
	if(v0 >= v1 && v0 >= v2){
		maxX = v0;
	}
	else if(v1 >= v0 && v1 >= v2){
		maxX = v1;
	}
	else if(v2 >= v0 && v2 >= v1){
		maxX = v2;
	}
	return maxX;
}

double Scene::findMinX(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2){
	double v0 = vertex0.getElementAt(0);
	double v1 = vertex1.getElementAt(0);
	double v2 = vertex2.getElementAt(0);
	double minX;
	if(v0 <= v1 && v0 <= v2){
		minX = v0;
	}
	else if(v1 <= v0 && v1 <= v2){
		minX = v1;
	}
	else if(v2 <= v0 && v2 <= v1){
		minX = v2;
	}
	return minX;
}

double Scene::findMaxY(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2){
	double v0 = vertex0.getElementAt(1);
	double v1 = vertex1.getElementAt(1);
	double v2 = vertex2.getElementAt(1);
	double maxY;
	if(v0 >= v1 && v0 >= v2){
		maxY = v0;
	}
	else if(v1 >= v0 && v1 >= v2){
		maxY = v1;
	}
	else if(v2 >= v0 && v2 >= v1){
		maxY = v2;
	}
	return maxY;
}

double Scene::findMinY(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2){
	double v0 = vertex0.getElementAt(1);
	double v1 = vertex1.getElementAt(1);
	double v2 = vertex2.getElementAt(1);
	double minY;
	if(v0 <= v1 && v0 <= v2){
		minY = v0;
	}
	else if(v1 <= v0 && v1 <= v2){
		minY = v1;
	}
	else if(v2 <= v0 && v2 <= v1){
		minY = v2;
	}
	return minY;
}


void Scene::triangleRasterization(Camera *camera, Vec3 firstVertex, Vec3 secondVertex, Vec3 thirdVertex){

	int horRes = camera->horRes;
    int verRes = camera->verRes;

	double minX = findMinX(firstVertex, secondVertex, thirdVertex);
	double maxX = findMaxX(firstVertex, secondVertex, thirdVertex);
	double minY = findMinY(firstVertex, secondVertex, thirdVertex);
	double maxY = findMaxY(firstVertex, secondVertex, thirdVertex);

	double r0 = colorsOfVertices.at(firstVertex.colorId-1)->r;
	double g0 = colorsOfVertices.at(firstVertex.colorId-1)->g;
	double b0 = colorsOfVertices.at(firstVertex.colorId-1)->b;

	double r1 = colorsOfVertices.at(secondVertex.colorId-1)->r;
	double g1 = colorsOfVertices.at(secondVertex.colorId-1)->g;
	double b1 = colorsOfVertices.at(secondVertex.colorId-1)->b;

	double r2 = colorsOfVertices.at(thirdVertex.colorId-1)->r;
	double g2 = colorsOfVertices.at(thirdVertex.colorId-1)->g;
	double b2 = colorsOfVertices.at(thirdVertex.colorId-1)->b;

	double alfa,beta,gama,r,g,b;

	for(int j = minY; j < maxY; j++){ // eski hali: j<=maxY. şimdiki halinde iceland bayrağındaki çizgiler gidiyor.
		for(int i = minX; i < maxX; i++){ // i<=maxX
			alfa = f(i, j, secondVertex, thirdVertex)/f(firstVertex.getElementAt(0), firstVertex.getElementAt(1), secondVertex, thirdVertex);
			beta = f(i, j, thirdVertex, firstVertex)/f(secondVertex.getElementAt(0), secondVertex.getElementAt(1), thirdVertex, firstVertex);
			gama = f(i, j, firstVertex, secondVertex)/f(thirdVertex.getElementAt(0), thirdVertex.getElementAt(1), firstVertex, secondVertex);
			if(alfa >= 0 && beta >= 0 && gama >= 0){
				if(i < 0 || i >= horRes || j < 0 || j >= verRes){
					continue;
				}
				r = alfa*r0 + beta*r1 + gama*r2;
				g = alfa*g0 + beta*g1 + gama*g2;
				b = alfa*b0 + beta*b1 + gama*b2;
				this->image[i][j].r = makeBetweenZeroAnd255(r);
				this->image[i][j].g = makeBetweenZeroAnd255(g);
				this->image[i][j].b = makeBetweenZeroAnd255(b);
			}
		
		}
	}
}

Matrix4 Scene::modelingTransformation(int modelIndex){

	Matrix4 initialMatrix = getIdentityMatrix();

	Model *targetModel;
	targetModel = models.at(modelIndex);
	int numOfTransformations = targetModel->numberOfTransformations;
	char transformationType;
	int transformationId;

	for(int k = 0; k < numOfTransformations; k++){

		transformationType = targetModel->transformationTypes.at(k);
		transformationId = targetModel->transformationIds.at(k);

		if(transformationType == 't'){
			Matrix4 translationMatrix = getIdentityMatrix();

			Translation *translationVal;
			translationVal = translations.at(transformationId-1);
			translationMatrix.val[0][3] = translationVal->tx;
			translationMatrix.val[1][3] = translationVal->ty;
			translationMatrix.val[2][3] = translationVal->tz;

			Matrix4 tempMatrix = multiplyMatrixWithMatrix(translationMatrix, initialMatrix);

		    for (int i = 0; i < 4; i++){
		        for (int j = 0; j < 4; j++){
		            initialMatrix.val[i][j] = tempMatrix.val[i][j];
		        }
		    }
		}
		else if(transformationType == 'r'){
			Matrix4 m = getIdentityMatrix();
			Matrix4 m_1 = getIdentityMatrix();
			Matrix4 rotationMatrix = getIdentityMatrix();

			Rotation *rotationVal;
			rotationVal = rotations.at(transformationId-1);
			double angle = rotationVal->angle;
			double ux = rotationVal->ux;
			double uy = rotationVal->uy;
			double uz = rotationVal->uz;

			Vec3 vec1 = Vec3(ux, uy, uz, -1);
			Vec3 u = normalizeVec3(vec1);
			Vec3 vec2;

			if(ABS(u.x) <= ABS(u.y) && ABS(u.x) <= ABS(u.z)){
				vec2 = Vec3(0, u.z, -1*u.y, -1);
			}
			else if(ABS(u.y) <= ABS(u.x) && ABS(u.y) <= ABS(u.z)){
				vec2 = Vec3(u.z, 0, -1*u.x, -1);
			}
			else if(ABS(u.z) <= ABS(u.x) && ABS(u.z) <= ABS(u.y)){
				vec2 = Vec3(u.y, -1*u.x, 0, -1);
			}

			Vec3 v = normalizeVec3(vec2);

			Vec3 vec_w = crossProductVec3(u, v);

			Vec3 w = normalizeVec3(vec_w);

			m.val[0][0] = u.getElementAt(0);
			m.val[0][1] = u.getElementAt(1);
			m.val[0][2] = u.getElementAt(2);

			m.val[1][0] = v.getElementAt(0);
			m.val[1][1] = v.getElementAt(1);
			m.val[1][2] = v.getElementAt(2);

			m.val[2][0] = w.getElementAt(0);
			m.val[2][1] = w.getElementAt(1);
			m.val[2][2] = w.getElementAt(2);


			m_1.val[0][0] = u.getElementAt(0);
			m_1.val[0][1] = v.getElementAt(0);
			m_1.val[0][2] = w.getElementAt(0);

			m_1.val[1][0] = u.getElementAt(1);
			m_1.val[1][1] = v.getElementAt(1);
			m_1.val[1][2] = w.getElementAt(1);

			m_1.val[2][0] = u.getElementAt(2);
			m_1.val[2][1] = v.getElementAt(2);
			m_1.val[2][2] = w.getElementAt(2);			

			double radian = (angle * M_PI) / 180;

			rotationMatrix.val[1][1] = cos(radian);
			rotationMatrix.val[1][2] = -1*sin(radian);

			rotationMatrix.val[2][1] = sin(radian);
			rotationMatrix.val[2][2] = cos(radian);

			Matrix4 temp1Matrix = multiplyMatrixWithMatrix(rotationMatrix, m);

			Matrix4 mergedMatrix = multiplyMatrixWithMatrix(m_1, temp1Matrix);

			Matrix4 tempMatrix = multiplyMatrixWithMatrix(mergedMatrix, initialMatrix);

			for (int i = 0; i < 4; i++){
		        for (int j = 0; j < 4; j++){
		            initialMatrix.val[i][j] = tempMatrix.val[i][j];
		        }
		    }
		}
		else if(transformationType == 's'){
			Matrix4 scaleMatrix = getIdentityMatrix();
			Scaling *scalingVal;

			scalingVal = scalings.at(transformationId-1);

			scaleMatrix.val[0][0] = scalingVal->sx;
			scaleMatrix.val[1][1] = scalingVal->sy;
			scaleMatrix.val[2][2] = scalingVal->sz;

			Matrix4 tempMatrix = multiplyMatrixWithMatrix(scaleMatrix, initialMatrix);

			for (int i = 0; i < 4; i++){
		        for (int j = 0; j < 4; j++){
		            initialMatrix.val[i][j] = tempMatrix.val[i][j];
		        }
		    }
		}
	}

	return initialMatrix;
}

Matrix4 Scene::cameraTransformation(Camera *camera){

	Matrix4 cameraMatrix = Matrix4();
	double ex, ey, ez, ux, uy, uz, vx, vy, vz, wx, wy, wz;

	ex = camera->pos.getElementAt(0);
	ey = camera->pos.getElementAt(1);
	ez = camera->pos.getElementAt(2);

	ux = camera->u.getElementAt(0);
	uy = camera->u.getElementAt(1);
	uz = camera->u.getElementAt(2);

	vx = camera->v.getElementAt(0);
	vy = camera->v.getElementAt(1);
	vz = camera->v.getElementAt(2);

	wx = camera->w.getElementAt(0);
	wy = camera->w.getElementAt(1);
	wz = camera->w.getElementAt(2);

	cameraMatrix.val[0][0] = ux;
	cameraMatrix.val[0][1] = uy;
	cameraMatrix.val[0][2] = uz;
	cameraMatrix.val[0][3] = -1*(ux*ex + uy*ey + uz*ez);

	cameraMatrix.val[1][0] = vx;
	cameraMatrix.val[1][1] = vy;
	cameraMatrix.val[1][2] = vz;
	cameraMatrix.val[1][3] = -1*(vx*ex + vy*ey + vz*ez);

	cameraMatrix.val[2][0] = wx;
	cameraMatrix.val[2][1] = wy;
	cameraMatrix.val[2][2] = wz;
	cameraMatrix.val[2][3] = -1*(wx*ex + wy*ey + wz*ez);

	cameraMatrix.val[3][3] = 1;

	return cameraMatrix;
}

Matrix4 Scene::projectionTransformation(Camera *camera){

	Matrix4 projectionMatrix = Matrix4();

	double l, r, b, t, n, f;

	l = camera->left;
	r = camera->right;
	b = camera->bottom;
	t = camera->top;
	n = camera->near;
	f = camera->far;

	if(projectionType == 1){
		projectionMatrix.val[0][0] = (2*n)/(r-l);
		projectionMatrix.val[1][1] = (2*n)/(t-b);
		projectionMatrix.val[2][2] = (f+n)/(n-f);

		projectionMatrix.val[0][2] = (r+l)/(r-l);
		projectionMatrix.val[1][2] = (t+b)/(t-b);
		projectionMatrix.val[3][2] = -1;

		projectionMatrix.val[2][3] = (2*n*f)/(n-f);
	}
	else{
		projectionMatrix.val[0][0] = 2/(r-l);
		projectionMatrix.val[1][1] = 2/(t-b);
		projectionMatrix.val[2][2] = 2/(n-f);

		projectionMatrix.val[0][3] = (r+l)/(l-r);
		projectionMatrix.val[1][3] = (t+b)/(b-t);
		projectionMatrix.val[2][3] = (f+n)/(n-f);

		projectionMatrix.val[3][3] = 1;		
	}

	return projectionMatrix;
}

Matrix4 Scene::viewportTransformation(Camera *camera){

	Matrix4 viewportMatrix;

	double nx = camera->horRes;
	double ny = camera->verRes;

	viewportMatrix.val[0][0] = nx/2;
	viewportMatrix.val[0][3] = (nx-1)/2;

	viewportMatrix.val[1][1] = ny/2;
	viewportMatrix.val[1][3] = (ny-1)/2;

	viewportMatrix.val[2][2] = 0.5;
	viewportMatrix.val[2][3] = 0.5;

	return viewportMatrix;
}

bool Scene::visible(double den, double num, double& t_e, double& t_l){
	double t;
	if(den > 0){//potentially entering
		t = num/den;
		if(t > t_l){
			return false;
		}
		if(t > t_e){
			t_e = t;
		}
	}
	else if(den < 0){//potentially leaving
		t = num/den;
		if(t < t_e){
			return false;
		}
		if(t < t_l){
			t_l = t;
		}
	}
	else if(num > 0){//line parallel to edge
		return false;
	}
	return true;
}

int Scene::clippingAlgorithm(Vec4 v0, Vec4 v1, Vec4& new0, Vec4& new1, Color& c0, Color& c1){
	double t_e, t_l, dx, dy, dz, xmin, xmax, ymin, ymax, zmin, zmax;
	double x0, x1, y0, y1, z0, z1;
	int isVisible = 0; //çizme

	x0 = v0.x;
	x1 = v1.x;
	y0 = v0.y;
	y1 = v1.y;
	z0 = v0.z;
	z1 = v1.z;

	t_e = 0;
	t_l = 1;
	dx = x1 - x0;
	dy = y1 - y0;
	dz = z1 - z0;
	xmin = ymin = zmin = -1;
	xmax = ymax = zmax = 1;

	if(visible(dx, xmin-x0, t_e, t_l)){ // left
		if(visible(-1*dx, x0-xmax, t_e, t_l)){ // right
			if(visible(dy, ymin-y0, t_e, t_l)){ // bottom
				if(visible(-1*dy, y0-ymax, t_e, t_l)){ // top
					if(visible(dz, zmin-z0, t_e, t_l)){ // front
						if(visible(-1*dz, z0-zmax, t_e, t_l)){ // back
							if(t_l < 1 && t_e > 0){ //both
								isVisible = 1;
							}
							else if(t_l < 1){ //second
								isVisible = 2;
							}
							else if(t_e > 0){ //first
								isVisible = 3;
							}
							else{ //not change
								isVisible = 4;
							}
							if(t_l < 1){
								new1.x = x0 + dx*t_l;
								//cout << x0 + dx*t_l << endl;
								new1.y = y0 + dy*t_l;
								new1.z = z0 + dz*t_l;
								int r1 = t_l*colorsOfVertices.at(v1.colorId-1)->r + (1-t_l)*colorsOfVertices.at(v0.colorId-1)->r;
								int g1 = t_l*colorsOfVertices.at(v1.colorId-1)->g + (1-t_l)*colorsOfVertices.at(v0.colorId-1)->g;
								int b1 = t_l*colorsOfVertices.at(v1.colorId-1)->b + (1-t_l)*colorsOfVertices.at(v0.colorId-1)->b;

								c1.r = r1;
								c1.g = g1;
								c1.b = b1;
							}

							if(t_e > 0){
								new0.x = x0 + dx*t_e;
								new0.y = y0 + dy*t_e;
								new0.z = z0 + dz*t_e;
								int r2 = t_e*colorsOfVertices.at(v1.colorId-1)->r + (1-t_e)*colorsOfVertices.at(v0.colorId-1)->r;
								int g2 = t_e*colorsOfVertices.at(v1.colorId-1)->g + (1-t_e)*colorsOfVertices.at(v0.colorId-1)->g;
								int b2 = t_e*colorsOfVertices.at(v1.colorId-1)->b + (1-t_e)*colorsOfVertices.at(v0.colorId-1)->b;
								
								c0.r = r2;
								c0.g = g2;
								c0.b = b2;
							}
						}
					}
				}
			}
		}
	}

	return isVisible;
}

void Scene::transformVertices(Camera *camera, vector< vector<bool> > &visibilityVector){

	int numberOfModels = models.size();
	int numberOfVertices = vertices.size();
	Triangle currTriangle;
	int vertexId;
	vector<Vec4> transformedVertices(numberOfVertices);
	
	

	Matrix4 cameraTransformationMatrix = cameraTransformation(camera);
	Matrix4 projectionTransformationMatrix = projectionTransformation(camera);
	Matrix4 viewportTransformationMatrix = viewportTransformation(camera);

	for(int modelIndex=0; modelIndex<numberOfModels; modelIndex++){

		vector<Vec3> twoDVertices(numberOfVertices);

		vector<bool> transformationFlag(numberOfVertices, false);

		Model *currModel = models.at(modelIndex);
		Matrix4 modelingTransformationMatrix = modelingTransformation(modelIndex);
		Matrix4 camWithModel = multiplyMatrixWithMatrix(cameraTransformationMatrix, modelingTransformationMatrix);
		Matrix4 viewingTransformationMatrix = multiplyMatrixWithMatrix(projectionTransformationMatrix, camWithModel);
		int numberOfTriangles = currModel->numberOfTriangles;

		vector<bool> isTriangleClippedVector(numberOfTriangles, false);
		vector< vector<Vec3> > linesOfModel(numberOfTriangles);
		vector< Color > colorsOfLines;


		int colorIndex = 1;

		for(int triangleIndex=0; triangleIndex<numberOfTriangles; triangleIndex++){

			currTriangle = currModel->triangles.at(triangleIndex);
			for(int vertexIndex=0; vertexIndex<3; vertexIndex++){

				if(vertexIndex==0){
					vertexId = currTriangle.getFirstVertexId();
				}
				else if(vertexIndex==1){
					vertexId = currTriangle.getSecondVertexId();
				}
				else{
					vertexId = currTriangle.getThirdVertexId();
				}
				
				if(!transformationFlag.at(vertexId-1)){
					Vec4 currVertex;
					currVertex.x = vertices.at(vertexId-1)->x; 
					currVertex.y = vertices.at(vertexId-1)->y; 
					currVertex.z = vertices.at(vertexId-1)->z; 
					currVertex.t = 1;

					Vec4 transformedVertex = multiplyMatrixWithVec4(viewingTransformationMatrix, currVertex);
					transformedVertices.at(vertexId-1).x = transformedVertex.x;
					transformedVertices.at(vertexId-1).y = transformedVertex.y;
					transformedVertices.at(vertexId-1).z = transformedVertex.z;
					transformedVertices.at(vertexId-1).t = transformedVertex.t;
					transformedVertices.at(vertexId-1).colorId = vertices.at(vertexId-1)->colorId;

					transformedVertices.at(vertexId-1).x /= transformedVertex.t;
					transformedVertices.at(vertexId-1).y /= transformedVertex.t;
					transformedVertices.at(vertexId-1).z /= transformedVertex.t;
					transformedVertices.at(vertexId-1).t /= transformedVertex.t;

					transformedVertex.x /= transformedVertex.t;
					transformedVertex.y /= transformedVertex.t;
					transformedVertex.z /= transformedVertex.t;
					transformedVertex.t /= transformedVertex.t;

					Vec4 temp = multiplyMatrixWithVec4(viewportTransformationMatrix, transformedVertex);

					Vec3 twoDPoint;
					twoDPoint.x = temp.x;
					twoDPoint.y = temp.y;
					twoDPoint.colorId = vertices.at(vertexId-1)->colorId;

					twoDVertices.at(vertexId-1).x = (int) twoDPoint.x;
					twoDVertices.at(vertexId-1).y = (int) twoDPoint.y;
					twoDVertices.at(vertexId-1).colorId = twoDPoint.colorId;

					transformationFlag.at(vertexId-1) = true;
				}
			}
			Vec4 firstVertex3D = transformedVertices.at(currTriangle.getFirstVertexId()-1);
			Vec3 firstVertex;
			firstVertex.x = firstVertex3D.x;
			firstVertex.y = firstVertex3D.y;
			firstVertex.z = firstVertex3D.z;

			Vec4 secondVertex3D = transformedVertices.at(currTriangle.getSecondVertexId()-1);
			Vec3 secondVertex;
			secondVertex.x = secondVertex3D.x;
			secondVertex.y = secondVertex3D.y;
			secondVertex.z = secondVertex3D.z;

			Vec4 thirdVertex3D = transformedVertices.at(currTriangle.getThirdVertexId()-1);
			Vec3 thirdVertex;
			thirdVertex.x = thirdVertex3D.x;
			thirdVertex.y = thirdVertex3D.y;
			thirdVertex.z = thirdVertex3D.z;

			if(currModel->type == 0){ // Wireframe, clipped it
				Vec4 first0 = firstVertex3D;
				Vec4 first1 = firstVertex3D;
				Vec4 second0 = secondVertex3D;
				Vec4 second1 = secondVertex3D;
				Vec4 third0 = thirdVertex3D;
				Vec4 third1 = thirdVertex3D;

				Color firstColor0 = Color(colorsOfVertices.at(firstVertex3D.colorId-1)->r, colorsOfVertices.at(firstVertex3D.colorId-1)->g, colorsOfVertices.at(firstVertex3D.colorId-1)->b);
				Color firstColor1 = Color(colorsOfVertices.at(firstVertex3D.colorId-1)->r, colorsOfVertices.at(firstVertex3D.colorId-1)->g, colorsOfVertices.at(firstVertex3D.colorId-1)->b);
				Color secondColor0 = Color(colorsOfVertices.at(secondVertex3D.colorId-1)->r, colorsOfVertices.at(secondVertex3D.colorId-1)->g, colorsOfVertices.at(secondVertex3D.colorId-1)->b);
				Color secondColor1 = Color(colorsOfVertices.at(secondVertex3D.colorId-1)->r, colorsOfVertices.at(secondVertex3D.colorId-1)->g, colorsOfVertices.at(secondVertex3D.colorId-1)->b);
				Color thirdColor0 = Color(colorsOfVertices.at(thirdVertex3D.colorId-1)->r, colorsOfVertices.at(thirdVertex3D.colorId-1)->g, colorsOfVertices.at(thirdVertex3D.colorId-1)->b);
				Color thirdColor1 = Color(colorsOfVertices.at(thirdVertex3D.colorId-1)->r, colorsOfVertices.at(thirdVertex3D.colorId-1)->g, colorsOfVertices.at(thirdVertex3D.colorId-1)->b);

				int ab = clippingAlgorithm(firstVertex3D, secondVertex3D, first0, second0, firstColor0, secondColor0);
				int bc = clippingAlgorithm(secondVertex3D, thirdVertex3D, second1, third0, secondColor1, thirdColor0);
				int ca = clippingAlgorithm(thirdVertex3D, firstVertex3D, third1, first1, thirdColor1, firstColor1);

				if(!(ab == 4 && bc == 4 && ca == 4)){
					isTriangleClippedVector.at(triangleIndex) = true;
					vector<Vec3> line;
					if(ab != 0){
						first0.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(firstColor0);

						second0.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(secondColor0);

						Vec4 temp = multiplyMatrixWithVec4(viewportTransformationMatrix, first0);
						Vec4 temp2 = multiplyMatrixWithVec4(viewportTransformationMatrix, second0);
						Vec3 v0, v1;

						v0.x = (int) temp.x;
						v0.y = (int) temp.y;
						v0.z = (int) temp.z;
						v0.colorId = first0.colorId;

						v1.x = (int) temp2.x;
						v1.y = (int) temp2.y;
						v1.z = (int) temp2.z;
						v1.colorId = second0.colorId;

						line.push_back(v0);
						line.push_back(v1);
					}					
					if(bc != 0){
						second1.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(secondColor1);

						third0.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(thirdColor0);

						Vec4 temp = multiplyMatrixWithVec4(viewportTransformationMatrix, second1);
						Vec4 temp2 = multiplyMatrixWithVec4(viewportTransformationMatrix, third0);
						Vec3 v0, v1;

						v0.x = (int) temp.x;
						v0.y = (int) temp.y;
						v0.z = (int) temp.z;
						v0.colorId = second1.colorId;

						v1.x = (int) temp2.x;
						v1.y = (int) temp2.y;
						v1.z = (int) temp2.z;
						v1.colorId = third0.colorId;

						line.push_back(v0);
						line.push_back(v1);
					}

					if(ca != 0){
						third1.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(thirdColor1);

						first1.colorId = colorIndex;
						colorIndex++;
						colorsOfLines.push_back(firstColor1);

						Vec4 temp = multiplyMatrixWithVec4(viewportTransformationMatrix, third1);
						Vec4 temp2 = multiplyMatrixWithVec4(viewportTransformationMatrix, first1);
						Vec3 v0, v1;

						v0.x = (int) temp.x;
						v0.y = (int) temp.y;
						v0.z = (int) temp.z;
						v0.colorId = third1.colorId;

						v1.x = (int) temp2.x;
						v1.y = (int) temp2.y;
						v1.z = (int) temp2.z;
						v1.colorId = first1.colorId;

						line.push_back(v0);
						line.push_back(v1);
					}

					linesOfModel.at(triangleIndex) = line;
				}
			}

			if(cullingEnabled){

				Vec3 cameraPosition;
				Vec3 differenceOfTwoPoints = subtractVec3(thirdVertex, secondVertex);
				Vec3 centerOfTwoPoints;
				centerOfTwoPoints.x = differenceOfTwoPoints.x / 2;
				centerOfTwoPoints.y = differenceOfTwoPoints.y / 2;
				centerOfTwoPoints.z = differenceOfTwoPoints.z / 2;

				Vec3 centerOfTriangle;
				centerOfTriangle.x = firstVertex.x + 2.0 * (centerOfTwoPoints.x-firstVertex.x) / 3;
				centerOfTriangle.y = firstVertex.y + 2.0 * (centerOfTwoPoints.y-firstVertex.y) / 3;
				centerOfTriangle.z = firstVertex.z + 2.0 * (centerOfTwoPoints.z-firstVertex.z) / 3;

				Vec3 eyeVector = subtractVec3(centerOfTriangle, cameraPosition);

				Vec3 surfaceNormal = crossProductVec3(subtractVec3(secondVertex, firstVertex), subtractVec3(thirdVertex, firstVertex));

				if(dotProductVec3(surfaceNormal, eyeVector) <= 0){
					visibilityVector.at(modelIndex).push_back(false);
				}
				else{
					visibilityVector.at(modelIndex).push_back(true);
				}

			}
		}

		drawLine(camera, twoDVertices, visibilityVector, modelIndex, colorsOfLines, isTriangleClippedVector, linesOfModel);
	}
}

void Scene::midpointAlgorithm(Camera *camera, Vec3 point0, Vec3 point1, bool isClipped, vector< Color > colorsOfLines){

	int horRes = camera->horRes;
    int verRes = camera->verRes;

	double x0 = point0.getElementAt(0);
	double y0 = point0.getElementAt(1);
	double x1 = point1.getElementAt(0);
	double y1 = point1.getElementAt(1);

	double r0,g0,b0,r1,g1,b1;

	if(isClipped){
		r0 = colorsOfLines.at(point0.colorId-1).r;
		g0 = colorsOfLines.at(point0.colorId-1).g;
		b0 = colorsOfLines.at(point0.colorId-1).b;

		r1 = colorsOfLines.at(point1.colorId-1).r;
		g1 = colorsOfLines.at(point1.colorId-1).g;
		b1 = colorsOfLines.at(point1.colorId-1).b;
	}
	else{
		r0 = colorsOfVertices.at(point0.colorId-1)->r;
		g0 = colorsOfVertices.at(point0.colorId-1)->g;
		b0 = colorsOfVertices.at(point0.colorId-1)->b;

		r1 = colorsOfVertices.at(point1.colorId-1)->r;
		g1 = colorsOfVertices.at(point1.colorId-1)->g;
		b1 = colorsOfVertices.at(point1.colorId-1)->b;
	}

	double d;
	double slope = (y1-y0)/(x1-x0);

	double r = r0;
	double g = g0;
	double b = b0;

	double r_increment_x = (r1 - r0) / (x1 - x0);
	double g_increment_x = (g1 - g0) / (x1 - x0);
	double b_increment_x = (b1 - b0) / (x1 - x0);

	double r_increment_y = (r1 - r0) / (y1 - y0);
	double g_increment_y = (g1 - g0) / (y1 - y0);
	double b_increment_y = (b1 - b0) / (y1 - y0);

	int tempx;
	int tempy;

	if(slope>= 0 && slope<1){ //normal

		if(x0>x1){
			tempx = x0;
			x0 = x1;
			x1 = tempx;

			tempy = y0;
			y0 = y1;
			y1 = tempy;

			r = r1;
			b = b1;
			g = g1;
		}

		d = 2*(y0-y1) + (x1-x0);
		int j = y0;
		for(double i = x0; i<=x1; i++){
			//cout << i << "," << j << endl;
			if(i < 0 || i >= horRes || j < 0 || j >= verRes){
				continue;
			}
			this->image[i][j].r = makeBetweenZeroAnd255(r);
			this->image[i][j].g = makeBetweenZeroAnd255(g);
			this->image[i][j].b = makeBetweenZeroAnd255(b);			
			
			if(d < 0){ // choose NE
				j = j + 1;
				d += 2*((y0 - y1) + (x1 - x0));
			}
			else{ // choose E
				d += 2*(y0 - y1);
			}
			r += r_increment_x;
			g += g_increment_x;
			b += b_increment_x;
		}
	}
	else if(slope>=1){ //swap x and y
		
		if(x0>x1){
			tempx = x0;
			x0 = x1;
			x1 = tempx;

			tempy = y0;
			y0 = y1;
			y1 = tempy;

			r = r1;
			b = b1;
			g = g1;			
		}

		d = 2*(x0-x1) + (y1-y0);
		int j = x0;
		for(double i = y0; i<=y1; i++){
			if(i < 0 || i >= horRes || j < 0 || j >= verRes){
				continue;
			}			
			this->image[j][i].r = makeBetweenZeroAnd255(r);
			this->image[j][i].g = makeBetweenZeroAnd255(g);
			this->image[j][i].b = makeBetweenZeroAnd255(b);
			
			if(d < 0){ // choose NE
				j = j + 1;
				d += 2*((x0 - x1) + (y1 - y0));
			}
			else{ // choose N
				d += 2*(x0 - x1);
			}
			r += r_increment_y;
			g += g_increment_y;
			b += b_increment_y;			
		}

	}
	else if(slope<0 && slope>=-1){ //negate x

		if(x1>x0){
			tempx = x0;
			x0 = x1;
			x1 = tempx;

			tempy = y0;
			y0 = y1;
			y1 = tempy;

			r = r1;
			b = b1;
			g = g1;
		}

		d = 2*(y0-y1) - (x1-x0);
		int j = y0;
		for(double i = x0; i>=x1; i--){
			if(i < 0 || i >= horRes || j < 0 || j >= verRes){
				continue;
			}
			this->image[i][j].r = makeBetweenZeroAnd255(r);
			this->image[i][j].g = makeBetweenZeroAnd255(g);
			this->image[i][j].b = makeBetweenZeroAnd255(b);
	
			if(d < 0){ // choose NW
				j = j + 1;
				d += 2*((y0 - y1) + (-1*(x1 - x0)));
			}
			else{ // choose W
				d += 2*(y0 - y1);
			}
			r -= r_increment_x;
			g -= g_increment_x;
			b -= b_increment_x;
		}
	}
	else if(slope<-1){ //negate x and, swap x and y

		if(x1>x0){
			tempx = x0;
			x0 = x1;
			x1 = tempx;

			tempy = y0;
			y0 = y1;
			y1 = tempy;
			
			r = r1;
			b = b1;
			g = g1;	
		}


		d = -2*(x0-x1) + (y1-y0);
		int j = x0;
		for(double i = y0; i<=y1; i++){
			if(i < 0 || i >= horRes || j < 0 || j >= verRes){
				continue;
			}
			this->image[j][i].r = makeBetweenZeroAnd255(r);
			this->image[j][i].g = makeBetweenZeroAnd255(g);
			this->image[j][i].b = makeBetweenZeroAnd255(b);

			if(d < 0){ // choose NW
				j = j - 1;
				d += 2*(-1*(x0 - x1) + (y1 - y0));
			}
			else{ // choose N
				d += -2*(x0 - x1);
			}
			r += r_increment_y;
			g += g_increment_y;
			b += b_increment_y;
		}
	}

}

void Scene::drawLine(Camera *camera, vector<Vec3> twoDVertices, vector< vector<bool> >visibilityVector, int modelIndex, vector< Color > colorsOfLines, vector<bool> isTriangleClippedVector, vector< vector<Vec3> > linesOfModel){

	Triangle currTriangle;
	int firstVertexId, secondVertexId, thirdVertexId;
	Vec3 firstVertex, secondVertex, thirdVertex;
	int numberOfTriangles;

	Model *currModel = models.at(modelIndex);
	numberOfTriangles = currModel->numberOfTriangles;
	int type = currModel->type;
	for(int triangleIndex=0; triangleIndex<numberOfTriangles; triangleIndex++){
		if(!cullingEnabled){
			currTriangle = currModel->triangles.at(triangleIndex);
			firstVertexId = currTriangle.getFirstVertexId();
			secondVertexId = currTriangle.getSecondVertexId();
			thirdVertexId = currTriangle.getThirdVertexId();

			firstVertex = twoDVertices.at(firstVertexId-1);
			secondVertex = twoDVertices.at(secondVertexId-1);
			thirdVertex = twoDVertices.at(thirdVertexId-1);

			if(type == 0){
				if(isTriangleClippedVector.at(triangleIndex)){
					vector<Vec3> currLines = linesOfModel.at(triangleIndex);
					int len = currLines.size();
					for(int i=0; i<len;){
						Vec3 p0, p1;
						p0 = currLines.at(i++);
						p1 = currLines.at(i++);
						midpointAlgorithm(camera, p0, p1, true, colorsOfLines);
					}
				}
				else{
					midpointAlgorithm(camera, firstVertex, secondVertex, false, colorsOfLines);
					midpointAlgorithm(camera, secondVertex, thirdVertex, false, colorsOfLines);
					midpointAlgorithm(camera, firstVertex, thirdVertex, false, colorsOfLines);
				}	
			}
			else{
				triangleRasterization(camera, firstVertex, secondVertex, thirdVertex);
			}
		}
		else if(visibilityVector.at(modelIndex).at(triangleIndex)){
			currTriangle = currModel->triangles.at(triangleIndex);
			firstVertexId = currTriangle.getFirstVertexId();
			secondVertexId = currTriangle.getSecondVertexId();
			thirdVertexId = currTriangle.getThirdVertexId();

			firstVertex = twoDVertices.at(firstVertexId-1);
			secondVertex = twoDVertices.at(secondVertexId-1);
			thirdVertex = twoDVertices.at(thirdVertexId-1);

			if(type == 0){	
				if(isTriangleClippedVector.at(triangleIndex)){
					vector<Vec3> currLines = linesOfModel.at(triangleIndex);
					int len = currLines.size();
					for(int i=0; i<len;){
						Vec3 p0, p1;
						p0 = currLines.at(i++);
						p1 = currLines.at(i++);
						midpointAlgorithm(camera, p0, p1, true, colorsOfLines);
					}
				}
				else{
					midpointAlgorithm(camera, firstVertex, secondVertex, false, colorsOfLines);
					midpointAlgorithm(camera, secondVertex, thirdVertex, false, colorsOfLines);
					midpointAlgorithm(camera, firstVertex, thirdVertex, false, colorsOfLines);
				}
			}
			else{
				triangleRasterization(camera, firstVertex, secondVertex, thirdVertex);
			}
		}
	}

}

void Scene::forwardRenderingPipeline(Camera *camera)
{
	// TODO: Implement this function.

	int numberOfModels = models.size();
	vector< vector<bool> > visibilityVector(numberOfModels);

	transformVertices(camera, visibilityVector);


}

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL)
		pElement->QueryBoolText(&cullingEnabled);

	// read projection type
	pElement = pRoot->FirstChildElement("ProjectionType");
	if (pElement != NULL)
		pElement->QueryIntText(&projectionType);

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera *cam = new Camera();

		pCamera->QueryIntAttribute("id", &cam->cameraId);

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

		cam->gaze = normalizeVec3(cam->gaze);
		cam->u = crossProductVec3(cam->gaze, cam->v);
		cam->u = normalizeVec3(cam->u);

		cam->w = inverseVec3(cam->gaze);
		cam->v = crossProductVec3(cam->u, cam->gaze);
		cam->v = normalizeVec3(cam->v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam->left, &cam->right, &cam->bottom, &cam->top,
			   &cam->near, &cam->far, &cam->horRes, &cam->verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam->outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		vertices.push_back(vertex);
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		Translation *translation = new Translation();

		pTranslation->QueryIntAttribute("id", &translation->translationId);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		translations.push_back(translation);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		Scaling *scaling = new Scaling();

		pScaling->QueryIntAttribute("id", &scaling->scalingId);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		scalings.push_back(scaling);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		Rotation *rotation = new Rotation();

		pRotation->QueryIntAttribute("id", &rotation->rotationId);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		rotations.push_back(rotation);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read models
	pElement = pRoot->FirstChildElement("Models");

	XMLElement *pModel = pElement->FirstChildElement("Model");
	XMLElement *modelElement;
	while (pModel != NULL)
	{
		Model *model = new Model();

		pModel->QueryIntAttribute("id", &model->modelId);
		pModel->QueryIntAttribute("type", &model->type);

		// read model transformations
		XMLElement *pTransformations = pModel->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		pTransformations->QueryIntAttribute("count", &model->numberOfTransformations);

		while (pTransformation != NULL)
		{
			char transformationType;
			int transformationId;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			model->transformationTypes.push_back(transformationType);
			model->transformationIds.push_back(transformationId);

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		// read model triangles
		XMLElement *pTriangles = pModel->FirstChildElement("Triangles");
		XMLElement *pTriangle = pTriangles->FirstChildElement("Triangle");

		pTriangles->QueryIntAttribute("count", &model->numberOfTriangles);

		while (pTriangle != NULL)
		{
			int v1, v2, v3;

			str = pTriangle->GetText();
			sscanf(str, "%d %d %d", &v1, &v2, &v3);

			model->triangles.push_back(Triangle(v1, v2, v3));

			pTriangle = pTriangle->NextSiblingElement("Triangle");
		}

		models.push_back(model);

		pModel = pModel->NextSiblingElement("Model");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	// if image is filled before, just change color rgb values with the background color
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}