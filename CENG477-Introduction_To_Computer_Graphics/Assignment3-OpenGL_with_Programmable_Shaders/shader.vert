#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec3 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform sampler2D heightMapTexture;
uniform int widthTexture;
uniform int heightTexture;
uniform vec3 lightPosition;

uniform int counter;

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

uniform mat4 Z;


vec3 createNeighbourVertex(int direction, vec3 targetVertex){
    vec3 newVector;
    if(direction == 0){ //E
        newVector = vec3(targetVertex.x+1, 0, targetVertex.z);
    }
    else if(direction == 1){ //NE
        newVector = vec3(targetVertex.x+1, 0, targetVertex.z+1);
    }
    else if(direction == 2){ //N
        newVector = vec3(targetVertex.x, 0, targetVertex.z+1);
    }
    else if(direction == 3){ //W
        newVector = vec3(targetVertex.x-1, 0, targetVertex.z);
    }
    else if(direction == 4){ //SW
        newVector = vec3(targetVertex.x-1, 0, targetVertex.z-1);
    }
    else if(direction == 5){ //S
        newVector = vec3(targetVertex.x, 0, targetVertex.z-1);
    }
    
    if((newVector.x >= 0 && newVector.x < widthTexture) && (newVector.z >= 0 && newVector.z < heightTexture)){
        // calculate y
        vec2 mappedCoordinate;
        mappedCoordinate.x = 1 - float(newVector.x + counter)/widthTexture;
        mappedCoordinate.y = 1 - float(newVector.z)/heightTexture;

        vec4 mappedColor = texture(heightMapTexture, mappedCoordinate);

        newVector.y = heightFactor*(mappedColor.r);

        return newVector;
    }
    else{
        vec3 notExistNeighbour = vec3(-1,-1,-1);
        return notExistNeighbour;
    }
}

vec3 createNormal(vec3 targetVertex, vec3 firstVertex, vec3 secondVertex){
    vec3 normalVector;
    if(firstVertex.x < 0 || secondVertex.x < 0){
        normalVector = vec3(2.0,2.0,2.0);
    }
    else{
        normalVector = normalize(cross(firstVertex-targetVertex, secondVertex-targetVertex));
    }
    return normalVector;
}

void main()
{

    vec3 vertexPosition;
    vertexPosition.x = position.x;
    vertexPosition.z = position.z;

    // get texture value, compute height

    textureCoordinate = vec2((1 - float(vertexPosition.x + counter) / widthTexture), 1 - float(vertexPosition.z) / heightTexture);
    vec4 mappedColor = texture(heightMapTexture, textureCoordinate);
    vertexPosition.y = heightFactor * mappedColor.r;

    // compute normal vector using also the heights of neighbor vertices

    vec3 eastVertex = createNeighbourVertex(0, vertexPosition);
    vec3 northEastVertex = createNeighbourVertex(1, vertexPosition);
    vec3 northVertex = createNeighbourVertex(2, vertexPosition);
    vec3 westVertex = createNeighbourVertex(3, vertexPosition);
    vec3 southWestVertex = createNeighbourVertex(4, vertexPosition);
    vec3 southVertex = createNeighbourVertex(5, vertexPosition);

    vec3 normal1 = createNormal(vertexPosition, northEastVertex, eastVertex);
    vec3 normal2 = createNormal(vertexPosition, northVertex, northEastVertex);
    vec3 normal3 = createNormal(vertexPosition, westVertex, northVertex);
    vec3 normal4 = createNormal(vertexPosition, southWestVertex, westVertex);
    vec3 normal5 = createNormal(vertexPosition, southVertex, southWestVertex);
    vec3 normal6 = createNormal(vertexPosition, eastVertex, southVertex);

    vertexNormal = vec3(0.0,0.0,0.0);

    if(normal1.x <= 1.0){
        vertexNormal += normal1;
    }
    if(normal2.x <= 1.0){
        vertexNormal += normal2;
    }
    if(normal3.x <= 1.0){
        vertexNormal += normal3;
    }
    if(normal4.x <= 1.0){
        vertexNormal += normal4;
    }
    if(normal5.x <= 1.0){
        vertexNormal += normal5;
    }
    if(normal6.x <= 1.0){
        vertexNormal += normal6;
    }

    // Z = (M^-1)^T (normal transformation matrix)

    vec4 transformedVertexNormal = Z * vec4(vertexNormal, 0.0);
    vec3 transformedVertexNormalvec3;
    transformedVertexNormalvec3.x = transformedVertexNormal.x;
    transformedVertexNormalvec3.y = transformedVertexNormal.y;
    transformedVertexNormalvec3.z = transformedVertexNormal.z;
    vertexNormal = normalize(transformedVertexNormalvec3);

    // compute toLight vector vertex coordinate in VCS

    vec4 transformedToLightVector = MV * vec4(lightPosition - vertexPosition, 0.0);
    vec3 transformedToLightVectorvec3;
    transformedToLightVectorvec3.x = transformedToLightVector.x;
    transformedToLightVectorvec3.y = transformedToLightVector.y;
    transformedToLightVectorvec3.z = transformedToLightVector.z;
    ToLightVector = transformedToLightVectorvec3;

    vec4 transformedToCameraVector = MV * vec4(cameraPosition - vertexPosition, 0.0);
    vec3 transformedToCameraVectorvec3;
    transformedToCameraVectorvec3.x = transformedToCameraVector.x;
    transformedToCameraVectorvec3.y = transformedToCameraVector.y;
    transformedToCameraVectorvec3.z = transformedToCameraVector.z;
    ToCameraVector = transformedToCameraVectorvec3;
    
   // set gl_Position variable correctly to give the transformed vertex position

   gl_Position = MVP * vec4(vertexPosition, 1.0);
    
}
