#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shape.h"
#include "tinyxml2.h"
#include "Image.h"
#include <limits>
#include <iostream>

using namespace tinyxml2;

/* 
 * Must render the scene from each camera's viewpoint and create an image.
 * You can use the methods of the Image class to save the image as a PPM file. 
 */
void Scene::renderScene(void)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
	int num_of_camera = cameras.size();
	for(int i = 0; i<num_of_camera; i++){
		int nx = cameras.at(i)->imgPlane.nx;
		int ny = cameras.at(i)->imgPlane.ny;
		Image image(nx, ny);
		for(int x = 0; x<nx; x++){
			for(int y = 0; y<ny; y++){
				Ray ray = cameras.at(i)->getPrimaryRay(x,y);
				Color mixed_color = findMixedColor(ray, this->maxRecursionDepth);
				image.setPixelValue(x,y,mixed_color);
			}
		}
		image.saveImage(cameras.at(i)->imageName);
	}

}

Color Scene::findMixedColor(const Ray & ray, int recursionDepth){
	float channel_red = 0, channel_green = 0, channel_blue = 0;
	float t=std::numeric_limits<float>::max();
	Shape *object = NULL;
	ReturnVal return_val;
	int num_of_object = objects.size();
	for(int o = 0; o<num_of_object; o++){
		ReturnVal r_val = objects.at(o)->intersect(ray);
		if(r_val.intersection_check == true){
			if(r_val.t < t && r_val.t > pScene->intTestEps){
				t = r_val.t;
				object = objects.at(o);
				return_val = r_val;
			}
		} 
	}
	if(object != NULL){
		Vector3f ambient_reflectance = this->materials.at(object->matIndex - 1)->ambientRef;
		Color ambient_color;
		channel_red = ambient_reflectance.r*this->ambientLight.r;
		channel_green = ambient_reflectance.g*this->ambientLight.g;
		channel_blue = ambient_reflectance.b*this->ambientLight.b;
		Color diffuse_color;
		diffuse_color.red = 0;
		diffuse_color.grn = 0;
		diffuse_color.blu = 0;

		Color specular_color;
		specular_color.red = 0;
		specular_color.grn = 0;
		specular_color.blu = 0;
		int num_of_light = lights.size();
		for(int l = 0; l<num_of_light; l++){
			Vector3f e_i = lights.at(l)->computeLightContribution(return_val.intersection_point);
			Vector3f dir, w_i;
			float len = (lights.at(l)->position.x - return_val.intersection_point.x)*(lights.at(l)->position.x - return_val.intersection_point.x) + (lights.at(l)->position.y - return_val.intersection_point.y)*(lights.at(l)->position.y - return_val.intersection_point.y) + (lights.at(l)->position.z - return_val.intersection_point.z)*(lights.at(l)->position.z - return_val.intersection_point.z);
			len = sqrt(len);
			dir.x = (lights.at(l)->position.x - return_val.intersection_point.x) / len;
			dir.y = (lights.at(l)->position.y - return_val.intersection_point.y) / len;
			dir.z = (lights.at(l)->position.z - return_val.intersection_point.z) / len;

			Vector3f intersection_point = return_val.intersection_point;
			w_i = dir;
			float shadowRayEps = this->shadowRayEps;
			Ray shadowRay;
			shadowRay.origin.x = intersection_point.x + return_val.surface_normal.x * shadowRayEps;
			shadowRay.origin.y = intersection_point.y + return_val.surface_normal.y * shadowRayEps;
			shadowRay.origin.z = intersection_point.z + return_val.surface_normal.z * shadowRayEps;
			float normalizing = sqrt(pow(lights.at(l)->position.x - shadowRay.origin.x,2)+pow(lights.at(l)->position.y - shadowRay.origin.y,2)+pow(lights.at(l)->position.z - shadowRay.origin.z,2));
			shadowRay.direction.x = (lights.at(l)->position.x - shadowRay.origin.x)/normalizing;
			shadowRay.direction.y = (lights.at(l)->position.y - shadowRay.origin.y)/normalizing;
			shadowRay.direction.z = (lights.at(l)->position.z - shadowRay.origin.z)/normalizing;
			ReturnVal value;
			bool intersectWithObject = false;
			float light_t = shadowRay.gett(w_i);
			for(int obj = 0; obj < num_of_object; obj++){
				value = objects.at(obj)->intersect(shadowRay);
				if(value.intersection_check == true && value.t >= 0 && light_t > value.t){
					intersectWithObject = true;
					break;
				}
			}
			if(intersectWithObject == true){
				continue;
			}

			float cos = dir.x*return_val.surface_normal.x + dir.y*return_val.surface_normal.y + dir.z*return_val.surface_normal.z;

			if(cos<0){
				cos = 0;
			}
			Vector3f diffuse_reflectance = this->materials.at(object->matIndex - 1)->diffuseRef;

			channel_red += diffuse_reflectance.r*cos*e_i.r;
			channel_green += diffuse_reflectance.g*cos*e_i.g;
			channel_blue += diffuse_reflectance.b*cos*e_i.b;

			Vector3f cameraPosition = ray.origin;
			Vector3f w_o, h;
			w_i = dir;
			w_o.x = cameraPosition.x - return_val.intersection_point.x;
			w_o.y = cameraPosition.y - return_val.intersection_point.y;
			w_o.z = cameraPosition.z - return_val.intersection_point.z;
			len = sqrt(pow(w_o.x, 2) + pow(w_o.y, 2) + pow(w_o.z, 2));
			w_o.x /= len;
			w_o.y /= len;
			w_o.z /= len;
			float h_len = sqrt(pow((w_i.x+w_o.x), 2) + pow((w_i.y+w_o.y), 2) + pow((w_i.z+w_o.z), 2));
			h.x = (w_i.x + w_o.x) / h_len;
			h.y = (w_i.y + w_o.y) / h_len;
			h.z = (w_i.z + w_o.z) / h_len;
			Vector3f specular_reflectance = this->materials.at(object->matIndex - 1)->specularRef;
			int phongExp = this->materials.at(object->matIndex - 1)->phongExp;
			cos = h.x*return_val.surface_normal.x + h.y*return_val.surface_normal.y + h.z*return_val.surface_normal.z;
			if(cos < 0){
				cos = 0;
			}

			channel_red += specular_reflectance.r * pow(cos, phongExp) * e_i.r;
			channel_green += specular_reflectance.g * pow(cos, phongExp) * e_i.g;
			channel_blue += specular_reflectance.b * pow(cos, phongExp) * e_i.b;
		}	

		Color mixed_color, mirror_color;
		mirror_color.red = 0;
		mirror_color.grn = 0;
		mirror_color.blu = 0;
		if(recursionDepth > 0){
			Vector3f mirror_reflectance = this->materials.at(object->matIndex - 1)->mirrorRef;
			if(mirror_reflectance.r > 0 || mirror_reflectance.g > 0 || mirror_reflectance.b > 0){
				Vector3f intersection_point = return_val.intersection_point;
				Vector3f cameraPosition = ray.origin;
				Vector3f w_o, w_r;
				w_o.x = cameraPosition.x - intersection_point.x;
				w_o.y = cameraPosition.y - intersection_point.y;
				w_o.z = cameraPosition.z - intersection_point.z;
				float len;
				len = sqrt(pow(w_o.x, 2) + pow(w_o.y, 2) + pow(w_o.z, 2));
				w_o.x /= len;
				w_o.y /= len;
				w_o.z /= len;
				Vector3f normal_vector = return_val.surface_normal;
				float dotProduct = normal_vector.x * w_o.x + normal_vector.y * w_o.y + normal_vector.z * w_o.z;
				w_r.x = -1 * w_o.x + 2*normal_vector.x*dotProduct;
				w_r.y = -1 * w_o.y + 2*normal_vector.y*dotProduct;
				w_r.z = -1 * w_o.z + 2*normal_vector.z*dotProduct;
				len = sqrt(pow(w_r.x, 2) + pow(w_r.y, 2) + pow(w_r.z, 2));
				w_r.x /= len;
				w_r.y /= len;
				w_r.z /= len;
				Ray reflectionRay(intersection_point, w_r);
				mirror_color = findMixedColor(reflectionRay, recursionDepth-1);
				mirror_color.red *= mirror_reflectance.r;
				mirror_color.grn *= mirror_reflectance.g;
				mirror_color.blu *= mirror_reflectance.b;
				channel_red += mirror_color.red;
				channel_green += mirror_color.grn;
				channel_blue += mirror_color.blu;
			}
		}

		if(channel_red > 255){
			channel_red = 255;
		}
		if(channel_green > 255){
			channel_green = 255;
		}
		if(channel_blue > 255){
			channel_blue = 255;
		}
		mixed_color.red = channel_red;
		mixed_color.grn = channel_green;
		mixed_color.blu = channel_blue;
		return mixed_color;
	}
	else{
		Color background_color;
		background_color.red = this->backgroundColor.r;
		background_color.grn = this->backgroundColor.g;
		background_color.blu = this->backgroundColor.b;
		return background_color;
	}
}

// Parses XML file. 
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement *pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;

	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if(pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);

	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if(pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if(pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);

	// Parse cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while(pCamera != nullptr)
	{
        int id;
        char imageName[64];
        Vector3f pos, gaze, up;
        ImagePlane imgPlane;

		eResult = pCamera->QueryIntAttribute("id", &id);
		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
		camElement = pCamera->FirstChildElement("NearPlane");
		str = camElement->GetText();
		sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
		camElement = pCamera->FirstChildElement("NearDistance");
		eResult = camElement->QueryFloatText(&imgPlane.distance);
		camElement = pCamera->FirstChildElement("ImageResolution");	
		str = camElement->GetText();
		sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
		camElement = pCamera->FirstChildElement("ImageName");
		str = camElement->GetText();
		strcpy(imageName, str);

		cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// Parse materals
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement *pMaterial = pElement->FirstChildElement("Material");
	XMLElement *materialElement;
	while(pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;
	
		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if(materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
		}
				else
		{
			materials[curr]->mirrorRef.r = 0.0;
			materials[curr]->mirrorRef.g = 0.0;
			materials[curr]->mirrorRef.b = 0.0;
		}
		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if(materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		pMaterial = pMaterial->NextSiblingElement("Material");
	}

	// Parse vertex data
	pElement = pRoot->FirstChildElement("VertexData");
	int cursor = 0;
	Vector3f tmpPoint;
	str = pElement->GetText();
	while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while(str[cursor] != '\0')
	{
		for(int cnt = 0 ; cnt < 3 ; cnt++)
		{
			if(cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if(cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++; 
			while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}

	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");
	
	// Parse spheres
	XMLElement *pObject = pElement->FirstChildElement("Sphere");
	XMLElement *objElement;
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);

		objects.push_back(new Sphere(id, matIndex, cIndex, R, &vertices));

		pObject = pObject->NextSiblingElement("Sphere");
	}

	// Parse triangles
	pObject = pElement->FirstChildElement("Triangle");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

		objects.push_back(new Triangle(id, matIndex, p1Index, p2Index, p3Index, &vertices));

		pObject = pObject->NextSiblingElement("Triangle");
	}

	// Parse meshes
	pObject = pElement->FirstChildElement("Mesh");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle> faces;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Faces");
		objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
		str = objElement->GetText();
		while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;
		while(str[cursor] != '\0')
		{
			for(int cnt = 0 ; cnt < 3 ; cnt++)
			{
				if(cnt == 0)
					p1Index = atoi(str + cursor) + vertexOffset;
				else if(cnt == 1)
					p2Index = atoi(str + cursor) + vertexOffset;
				else
					p3Index = atoi(str + cursor) + vertexOffset;
				while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
					cursor++; 
				while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
			}
			faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index, p3Index, &vertices)));
		}

		objects.push_back(new Mesh(id, matIndex, faces, &vertices));

		pObject = pObject->NextSiblingElement("Mesh");
	}

	// Parse lights
	int id;
	Vector3f position;
	Vector3f intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement *lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while(pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(position, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
}

