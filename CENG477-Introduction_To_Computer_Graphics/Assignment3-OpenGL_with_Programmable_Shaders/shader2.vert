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

void main()
{
    float PI = 3.14159265;
    float r = 350.0;
    float alpha, beta;
    alpha = 2 * PI * position.x;
    beta =  PI * position.z;
    vec3 vertexPosition;
    vertexPosition.x = r * sin(beta) * cos(alpha);
    vertexPosition.y = r * sin(beta) * sin(alpha);
    vertexPosition.z = r * cos(beta);

    // get texture value, compute height

    textureCoordinate = vec2(position.x+counter/250.0, position.z);
    vec4 mappedColor = texture(heightMapTexture, textureCoordinate);
    vertexNormal = normalize(vertexPosition);
    vec3 vertexHeight = heightFactor * mappedColor.r * vertexNormal;
    vertexPosition = vertexPosition + vertexHeight;

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

    vec3 cameraPos;
    cameraPos.x = cameraPosition.x;
    cameraPos.y = cameraPosition.y;
    cameraPos.z = cameraPosition.z;
    vec4 transformedToCameraVector = MV * vec4(cameraPos - vertexPosition, 0.0);
    vec3 transformedToCameraVectorvec3;
    transformedToCameraVectorvec3.x = transformedToCameraVector.x;
    transformedToCameraVectorvec3.y = transformedToCameraVector.y;
    transformedToCameraVectorvec3.z = transformedToCameraVector.z;
    ToCameraVector = transformedToCameraVectorvec3;
   
   // set gl_Position variable correctly to give the transformed vertex position

   gl_Position = MVP * vec4(vertexPosition, 1.0);
    
}
