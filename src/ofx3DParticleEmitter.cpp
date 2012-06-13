//
// ofx3DParticleEmitter.cpp
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

#include "ofx3DParticleEmitter.h"

// ------------------------------------------------------------------------
// Lifecycle
// ------------------------------------------------------------------------

ofx3DParticleEmitter::ofx3DParticleEmitter()
{
	init();
}

ofx3DParticleEmitter::~ofx3DParticleEmitter()
{
	exit();
}

void ofx3DParticleEmitter::init() {
	ofxParticleEmitter::init();
}

void ofx3DParticleEmitter::exit()
{	
	if ( texture != NULL )
		delete texture;
	texture = NULL;
	
	if ( particles != NULL )
		delete particles;
	particles = NULL;
	
	if ( vertices != NULL )
		delete vertices;
	vertices = NULL;
	
	glDeleteBuffers( 1, &verticesID );
}

bool ofx3DParticleEmitter::loadFromXml( const std::string& filename )
{
	bool ok = false;
	
	settings = new ofxXmlSettings();
	
	ok = settings->loadFile( filename );
	if ( ok )
	{
		parseParticleConfig();
		setupArrays();
		
		ok = active = true;
	}

	delete settings;
	settings = NULL;
	
	return ok;
}

void ofx3DParticleEmitter::parseParticleConfig()
{
	if ( settings == NULL )
	{
		ofLog( OF_LOG_ERROR, "ofx3DParticleEmitter::parseParticleConfig() - XML settings is invalid!" );
		return;
	}
	
	settings->pushTag( "particleEmitterConfig" );

	std::string imageFilename	= settings->getAttribute( "texture", "name", "" );
	std::string imageData		= settings->getAttribute( "texture", "data", "" );
	
	if ( imageFilename != "" )
	{
		ofLog( OF_LOG_WARNING, "ofx3DParticleEmitter::parseParticleConfig() - loading image file" );
		
		texture = new ofImage();
		texture->loadImage( imageFilename );
		texture->setUseTexture( true );
		texture->setAnchorPercent( 0.5f, 0.5f );
		
		textureData = texture->getTextureReference().getTextureData();
	}
	else if ( imageData != "" )
	{
		// TODO
		
		ofLog( OF_LOG_ERROR, "ofx3DParticleEmitter::parseParticleConfig() - image data found but not yet implemented!" );
		return;
	}

    emitterType					= settings->getAttribute( "emitterType", "value", emitterType );
	
	sourcePosition.x			= settings->getAttribute( "sourcePosition", "x", sourcePosition.x );
	sourcePosition.y			= settings->getAttribute( "sourcePosition", "y", sourcePosition.y );
	sourcePosition.z			= settings->getAttribute( "sourcePosition", "z", sourcePosition.z );
	
	speed						= settings->getAttribute( "speed", "value", speed );
	speedVariance				= settings->getAttribute( "speedVariance", "value", speedVariance );
	particleLifespan			= settings->getAttribute( "particleLifespan", "value", particleLifespan );
	particleLifespanVariance	= settings->getAttribute( "particleLifespanVariance", "value", particleLifespanVariance );
	angle						= settings->getAttribute( "angle", "value", angle );
	angleVariance				= settings->getAttribute( "angleVariance", "value", angleVariance );
	
	gravity.x					= settings->getAttribute( "gravity", "x", gravity.x );
	gravity.y					= settings->getAttribute( "gravity", "y", gravity.y );
	gravity.z					= settings->getAttribute( "gravity", "z", gravity.z );
	
	radialAcceleration			= settings->getAttribute( "radialAcceleration", "value", radialAcceleration );
	tangentialAcceleration		= settings->getAttribute( "tangentialAcceleration", "value", tangentialAcceleration );
	
	startColor.red				= settings->getAttribute( "startColor", "red", startColor.red );
	startColor.green			= settings->getAttribute( "startColor", "green", startColor.green );
	startColor.blue				= settings->getAttribute( "startColor", "blue", startColor.blue );
	startColor.alpha			= settings->getAttribute( "startColor", "alpha", startColor.alpha );
	
	startColorVariance.red		= settings->getAttribute( "startColorVariance", "red", startColorVariance.red );
	startColorVariance.green	= settings->getAttribute( "startColorVariance", "green", startColorVariance.green );
	startColorVariance.blue		= settings->getAttribute( "startColorVariance", "blue", startColorVariance.blue );
	startColorVariance.alpha	= settings->getAttribute( "startColorVariance", "alpha", startColorVariance.alpha );
	
	finishColor.red				= settings->getAttribute( "finishColor", "red", finishColor.red );
	finishColor.green			= settings->getAttribute( "finishColor", "green", finishColor.green );
	finishColor.blue			= settings->getAttribute( "finishColor", "blue", finishColor.blue );
	finishColor.alpha			= settings->getAttribute( "finishColor", "alpha", finishColor.alpha );
	
	finishColorVariance.red		= settings->getAttribute( "finishColorVariance", "red", finishColorVariance.red );
	finishColorVariance.green	= settings->getAttribute( "finishColorVariance", "green", finishColorVariance.green );
	finishColorVariance.blue	= settings->getAttribute( "finishColorVariance", "blue", finishColorVariance.blue );
	finishColorVariance.alpha	= settings->getAttribute( "finishColorVariance", "alpha", finishColorVariance.alpha );
	
	maxParticles				= settings->getAttribute( "maxParticles", "value", maxParticles );
	startParticleSize			= settings->getAttribute( "startParticleSize", "value", startParticleSize );
	startParticleSizeVariance	= settings->getAttribute( "startParticleSizeVariance", "value", startParticleSizeVariance );
	finishParticleSize			= settings->getAttribute( "finishParticleSize", "value", finishParticleSize );
	finishParticleSizeVariance	= settings->getAttribute( "finishParticleSizeVariance", "value", finishParticleSizeVariance );
	duration					= settings->getAttribute( "duration", "value", duration );
	blendFuncSource				= settings->getAttribute( "blendFuncSource", "value", blendFuncSource );
	blendFuncDestination		= settings->getAttribute( "blendFuncDestination", "value", blendFuncDestination );
	
	maxRadius					= settings->getAttribute( "maxRadius", "value", maxRadius );
	maxRadiusVariance			= settings->getAttribute( "maxRadiusVariance", "value", maxRadiusVariance );
	radiusSpeed					= settings->getAttribute( "radiusSpeed", "value", radiusSpeed );
	minRadius					= settings->getAttribute( "minRadius", "value", minRadius );
	
	rotatePerSecond				= settings->getAttribute( "rotatePerSecond", "value", rotatePerSecond );
	rotatePerSecondVariance		= settings->getAttribute( "rotatePerSecondVariance", "value", rotatePerSecondVariance );
}

void ofx3DParticleEmitter::setupArrays()
{
	// Allocate the memory necessary for the particle emitter arrays
	particles = (Particle3D*)malloc( sizeof( Particle3D ) * maxParticles );
	vertices = (PointSprite3D*)malloc( sizeof( PointSprite3D ) * maxParticles );
	
	// If one of the arrays cannot be allocated throw an assertion as this is bad
	assert( particles && vertices );
	
	// Generate the vertices VBO
	glGenBuffers( 1, &verticesID );
	
	// Set the particle count to zero
	particleCount = 0;
	
	// Reset the elapsed time
	elapsedTime = 0;
}

// ------------------------------------------------------------------------
// Particle Management
// ------------------------------------------------------------------------

bool ofx3DParticleEmitter::addParticle()
{
	// If we have already reached the maximum number of particles then do nothing
	if(particleCount == maxParticles)
		return false;
	
	// Take the next particle out of the particle pool we have created and initialize it
	Particle3D *particle = &particles[particleCount];
	initParticle( particle );

	// Increment the particle count
	particleCount++;
	
	// Return true to show that a particle has been created
	return true;
}

void ofx3DParticleEmitter::initParticle( Particle3D* particle )
{
	// Init the position of the particle.  This is based on the source position of the particle emitter
	// plus a configured variance.  The RANDOM_MINUS_1_TO_1 macro allows the number to be both positive
	// and negative
	particle->position.x = sourcePosition.x + sourcePositionVariance.x * RANDOM_MINUS_1_TO_1();
	particle->position.y = sourcePosition.y + sourcePositionVariance.y * RANDOM_MINUS_1_TO_1();
	particle->position.z = sourcePosition.z + sourcePositionVariance.z * RANDOM_MINUS_1_TO_1();
    particle->startPos.x = sourcePosition.x;
    particle->startPos.y = sourcePosition.y;
    particle->startPos.z = sourcePosition.z;
	
	// Init the direction of the particle.  The newAngle is calculated using the angle passed in and the
	// angle variance.
	float newAngle = (GLfloat)DEGREES_TO_RADIANS(angle + angleVariance * RANDOM_MINUS_1_TO_1());
	
	// Create a new Vector2f using the newAngle
	Vector3f vector = Vector3fMake(cosf(newAngle), sinf(newAngle), cosf(newAngle)); ////
	
	// Calculate the vectorSpeed using the speed and speedVariance which has been passed in
	float vectorSpeed = speed + speedVariance * RANDOM_MINUS_1_TO_1();
	
	// The particles direction vector is calculated by taking the vector calculated above and
	// multiplying that by the speed
	particle->direction = Vector3fMultiply(vector, vectorSpeed);
	
	// Set the default diameter of the particle from the source position
	particle->radius = maxRadius + maxRadiusVariance * RANDOM_MINUS_1_TO_1();
	particle->radiusDelta = (maxRadius / particleLifespan) * (1.0 / MAXIMUM_UPDATE_RATE);
	particle->angle = DEGREES_TO_RADIANS(angle + angleVariance * RANDOM_MINUS_1_TO_1());
	particle->degreesPerSecond = DEGREES_TO_RADIANS(rotatePerSecond + rotatePerSecondVariance * RANDOM_MINUS_1_TO_1());
    
    particle->radialAcceleration = radialAcceleration;
    particle->tangentialAcceleration = tangentialAcceleration;
	
	// Calculate the particles life span using the life span and variance passed in
	particle->timeToLive = MAX(0, particleLifespan + particleLifespanVariance * RANDOM_MINUS_1_TO_1());
	
	// Calculate the particle size using the start and finish particle sizes
	GLfloat particleStartSize = startParticleSize + startParticleSizeVariance * RANDOM_MINUS_1_TO_1();
	GLfloat particleFinishSize = finishParticleSize + finishParticleSizeVariance * RANDOM_MINUS_1_TO_1();
	particle->particleSizeDelta = ((particleFinishSize - particleStartSize) / particle->timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	particle->particleSize = MAX(0, particleStartSize);
	
	// Calculate the color the particle should have when it starts its life.  All the elements
	// of the start color passed in along with the variance are used to calculate the star color
	Color4f start = {0, 0, 0, 0};
	start.red = startColor.red + startColorVariance.red * RANDOM_MINUS_1_TO_1();
	start.green = startColor.green + startColorVariance.green * RANDOM_MINUS_1_TO_1();
	start.blue = startColor.blue + startColorVariance.blue * RANDOM_MINUS_1_TO_1();
	start.alpha = startColor.alpha + startColorVariance.alpha * RANDOM_MINUS_1_TO_1();
	
	// Calculate the color the particle should be when its life is over.  This is done the same
	// way as the start color above
	Color4f end = {0, 0, 0, 0};
	end.red = finishColor.red + finishColorVariance.red * RANDOM_MINUS_1_TO_1();
	end.green = finishColor.green + finishColorVariance.green * RANDOM_MINUS_1_TO_1();
	end.blue = finishColor.blue + finishColorVariance.blue * RANDOM_MINUS_1_TO_1();
	end.alpha = finishColor.alpha + finishColorVariance.alpha * RANDOM_MINUS_1_TO_1();
	
	// Calculate the delta which is to be applied to the particles color during each cycle of its
	// life.  The delta calculation uses the life span of the particle to make sure that the 
	// particles color will transition from the start to end color during its life time.  As the game
	// loop is using a fixed delta value we can calculate the delta color once saving cycles in the 
	// update method
	particle->color = start;
	particle->deltaColor.red = ((end.red - start.red) / particle->timeToLive) * (1.0 / MAXIMUM_UPDATE_RATE);
	particle->deltaColor.green = ((end.green - start.green) / particle->timeToLive)  * (1.0 / MAXIMUM_UPDATE_RATE);
	particle->deltaColor.blue = ((end.blue - start.blue) / particle->timeToLive)  * (1.0 / MAXIMUM_UPDATE_RATE);
	particle->deltaColor.alpha = ((end.alpha - start.alpha) / particle->timeToLive)  * (1.0 / MAXIMUM_UPDATE_RATE);
}


// ------------------------------------------------------------------------
// Update
// ------------------------------------------------------------------------

void ofx3DParticleEmitter::update()
{
	if ( !active ) return;

	// Calculate the emission rate
	emissionRate = maxParticles / particleLifespan;

	GLfloat aDelta = (ofGetElapsedTimeMillis()-lastUpdateMillis)/1000.0f;
	
	// If the emitter is active and the emission rate is greater than zero then emit
	// particles
	if(active && emissionRate) {
		float rate = 1.0f/emissionRate;
		emitCounter += aDelta;
		while(particleCount < maxParticles && emitCounter > rate) {
			addParticle();
			emitCounter -= rate;
		}
		
		elapsedTime += aDelta;
		if(duration != -1 && duration < elapsedTime)
			stopParticleEmitter();
	}
	
	// Reset the particle index before updating the particles in this emitter
	particleIndex = 0;
	
	// Loop through all the particles updating their location and color
	while(particleIndex < particleCount) {
		
		// Get the particle for the current particle index
		Particle3D *currentParticle = &particles[particleIndex];
        
        // FIX 1
        // Reduce the life span of the particle
        currentParticle->timeToLive -= aDelta;
		
		// If the current particle is alive then update it
		if(currentParticle->timeToLive > 0) {
			
			// If maxRadius is greater than 0 then the particles are going to spin otherwise
			// they are effected by speed and gravity
			if (emitterType == kParticleTypeRadial) {
				
                // FIX 2
                // Update the angle of the particle from the sourcePosition and the radius.  This is only
				// done of the particles are rotating
				currentParticle->angle += currentParticle->degreesPerSecond * aDelta;
				currentParticle->radius -= currentParticle->radiusDelta;
                
				Vector3f tmp;
				tmp.x = sourcePosition.x - cosf(currentParticle->angle) * currentParticle->radius;
				tmp.y = sourcePosition.y - sinf(currentParticle->angle) * currentParticle->radius;
				tmp.z = sourcePosition.z;//sourcePosition.z - cosf(currentParticle->angle) * currentParticle->radius; ////
				currentParticle->position = tmp;
				
				if (currentParticle->radius < minRadius)
					currentParticle->timeToLive = 0;
			} else {
				Vector3f tmp, radial, tangential;
                
                radial = Vector3fZero;
                Vector3f diff = Vector3fSub(currentParticle->startPos, Vector3fZero);
                
                currentParticle->position = Vector3fSub(currentParticle->position, diff);
                
                if (currentParticle->position.x || currentParticle->position.y)
                    radial = Vector3fNormalize(currentParticle->position);
                
                tangential.x = radial.x;
                tangential.y = radial.y;
                tangential.z = radial.z;
                radial = Vector3fMultiply(radial, currentParticle->radialAcceleration);
                
                GLfloat newy = tangential.x;
                tangential.x = -tangential.y;
                tangential.y = newy;
                tangential = Vector3fMultiply(tangential, currentParticle->tangentialAcceleration);
                
				tmp = Vector3fAdd( Vector3fAdd(radial, tangential), gravity);
                tmp = Vector3fMultiply(tmp, aDelta);
				currentParticle->direction = Vector3fAdd(currentParticle->direction, tmp);
				tmp = Vector3fMultiply(currentParticle->direction, aDelta);
				currentParticle->position = Vector3fAdd(currentParticle->position, tmp);
                currentParticle->position = Vector3fAdd(currentParticle->position, diff);
			}
			
			// Update the particles color
			currentParticle->color.red += currentParticle->deltaColor.red;
			currentParticle->color.green += currentParticle->deltaColor.green;
			currentParticle->color.blue += currentParticle->deltaColor.blue;
			currentParticle->color.alpha += currentParticle->deltaColor.alpha;

			// Place the position of the current particle into the vertices array
			vertices[particleIndex].x = currentParticle->position.x;
			vertices[particleIndex].y = currentParticle->position.y;
            vertices[particleIndex].z = currentParticle->position.z;
			
			// Place the size of the current particle in the size array
			currentParticle->particleSize += currentParticle->particleSizeDelta;
			vertices[particleIndex].size = MAX(0, currentParticle->particleSize);
			
			// Place the color of the current particle into the color array
			vertices[particleIndex].color = currentParticle->color;
			
			// Update the particle counter
			particleIndex++;
		} else {
			
			// As the particle is not alive anymore replace it with the last active particle 
			// in the array and reduce the count of particles by one.  This causes all active particles
			// to be packed together at the start of the array so that a particle which has run out of
			// life will only drop into this clause once
			if(particleIndex != particleCount - 1)
				particles[particleIndex] = particles[particleCount - 1];
			particleCount--;
		}
	}

	lastUpdateMillis = ofGetElapsedTimeMillis();
}



void ofx3DParticleEmitter::draw(int x /* = 0 */, int y /* = 0 */)
{
	if ( !active ) return;
	
	glPushMatrix();
	glTranslatef( x, y, 0.0f );
	
	drawTextures();
	
	glPopMatrix();
}

void ofx3DParticleEmitter::drawTextures()
{

	glEnable(GL_BLEND);
	glBlendFunc(blendFuncSource, blendFuncDestination);
	
	for( int i = 0; i < particleCount; i++ )
	{
		PointSprite3D* ps = &vertices[i];
		ofSetColor( ps->color.red*255.0f, ps->color.green*255.0f, 
				   ps->color.blue*255.0f, ps->color.alpha*255.0f );
        texture->draw( ps->x, ps->y, ps->z, ps->size, ps->size );
	}
	
	glDisable(GL_BLEND);
}
