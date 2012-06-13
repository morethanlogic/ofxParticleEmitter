//
// ofx3DParticleEmitter.h
//
// Copyright (c) 2010 71Squared, ported to Openframeworks by Shawn Roske
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef _OFX_3D_PARTICLE_EMITTER
#define _OFX_3D_PARTICLE_EMITTER

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxParticleEmitter.h"

// ------------------------------------------------------------------------
// Structures
// ------------------------------------------------------------------------

// Structure that defines a vector using x and y
typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Vector3f;

// Structure that holds the location and size for each point sprite
typedef struct 
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat size;
	Color4f color;
} PointSprite3D;

// Structure used to hold particle specific information
typedef struct 
{
	Vector3f	position;
	Vector3f	direction;
    Vector3f	startPos;
	Color4f		color;
	Color4f		deltaColor;
    GLfloat		radialAcceleration;
    GLfloat		tangentialAcceleration;
	GLfloat		radius;
	GLfloat		radiusDelta;
	GLfloat		angle;
	GLfloat		degreesPerSecond;
	GLfloat		particleSize;
	GLfloat		particleSizeDelta;
	GLfloat		timeToLive;
} Particle3D;

// ------------------------------------------------------------------------
// Inline functions
// ------------------------------------------------------------------------

// Return a zero populated Vector2f
static const Vector3f Vector3fZero = {0.0f, 0.0f, 0.0f};

// Return a populated Vector2d structure from the floats passed in
static inline Vector3f Vector3fMake(GLfloat x, GLfloat y, GLfloat z) {
	Vector3f r; r.x = x; r.y = y; r.z = z;
	return r;
}

// Return a Vector3f containing v multiplied by s
static inline Vector3f Vector3fMultiply(Vector3f v, GLfloat s) {
	Vector3f r; 
	r.x = v.x * s;
	r.y = v.y * s;
	r.z = v.z * s;
	return r;
}

// Return a Vector3f containing v1 + v2
static inline Vector3f Vector3fAdd(Vector3f v1, Vector3f v2) {
	Vector3f r; 
	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;
	r.z = v1.z + v2.z;
	return r;
}

// Return a Vector3f containing v1 - v2
static inline Vector3f Vector3fSub(Vector3f v1, Vector3f v2) {
	Vector3f r; 
	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;
	r.z = v1.z - v2.z;
	return r;
}

// Return the dot product of v1 and v2
static inline GLfloat Vector3fDot(Vector3f v1, Vector3f v2) {
	return (GLfloat) v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Return the length of the vector v
static inline GLfloat Vector3fLength(Vector3f v) {
	return (GLfloat) sqrtf(Vector3fDot(v, v));
}

// Return a Vector2f containing a normalized vector v
static inline Vector3f Vector3fNormalize(Vector3f v) {
	return Vector3fMultiply(v, 1.0f/Vector3fLength(v));
}

// ------------------------------------------------------------------------
// ofx3DParticleEmitter
// ------------------------------------------------------------------------

class ofx3DParticleEmitter: public ofxParticleEmitter
{
	
public:
	
	ofx3DParticleEmitter();
	~ofx3DParticleEmitter();
	
	bool	loadFromXml( const std::string& filename );
	void	update();
    void    draw(int x, int y);
    
	int				emitterType;
	Vector3f		sourcePosition, sourcePositionVariance;			
	GLfloat			angle, angleVariance;								
	GLfloat			speed, speedVariance;	
	GLfloat			radialAcceleration, tangentialAcceleration;
	GLfloat			radialAccelVariance, tangentialAccelVariance;
	Vector3f		gravity;	
	GLfloat			particleLifespan, particleLifespanVariance;			
	Color4f			startColor, startColorVariance;						
	Color4f			finishColor, finishColorVariance;
	GLfloat			startParticleSize, startParticleSizeVariance;
	GLfloat			finishParticleSize, finishParticleSizeVariance;
	GLint			maxParticles;
	GLint			particleCount;
	GLfloat			duration;
	int				blendFuncSource, blendFuncDestination;

	// Particle ivars only used when a maxRadius value is provided.  These values are used for
	// the special purpose of creating the spinning portal emitter
	GLfloat			maxRadius;						// Max radius at which particles are drawn when rotating
	GLfloat			maxRadiusVariance;				// Variance of the maxRadius
	GLfloat			radiusSpeed;					// The speed at which a particle moves from maxRadius to minRadius
	GLfloat			minRadius;						// Radius from source below which a particle dies
	GLfloat			rotatePerSecond;				// Number of degrees to rotate a particle around the source position per second
	GLfloat			rotatePerSecondVariance;		// Variance in degrees for rotatePerSecond
	

protected:

    void    exit();
	void	init();
	
	void	parseParticleConfig();
	void	setupArrays();
	
	bool	addParticle();
	void	initParticle( Particle3D* particle );
	
	void	drawTextures();
	
	GLuint			verticesID;		// Holds the buffer name of the VBO that stores the color and vertices info for the particles
	Particle3D*		particles;		// Array of particles that hold the particle emitters particle details
	PointSprite3D*	vertices;		// Array of vertices and color information for each particle to be rendered
};

#endif