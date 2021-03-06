// Description:
//   Simple 3D Model loader.
//
// Copyright (C) 2001 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//
#include "Model.hpp"

#include "Trace.hpp"
#include "Tokenizer.hpp"
#include "ResourceManager.hpp"

#include <vector>
using namespace std;

#ifdef IPHONE
float Model::MODEL_SCALE = 1.3f;
#else
float Model::MODEL_SCALE = 1.0f;
#endif

Model::Model( void):
    _numVerts(0),
    _numColors(0),
    _numFaces(0),
    _verts(0),
    _norms(0),
    _colors(0),
    _faces(0),
    _offset(0,0,0),
    _fixNormals(false)
{
    XTRACE();
}

Model::~Model()
{
    XTRACE();
    delete[] _faces;
}

//Load model from file
bool Model::load( const char *filename)
{
    XTRACE();
    //TODO: don't need to keep model data around, just VBO data
    
    //HACK to fixup models with bad normals or models that require
    //GL_LIGHT_MODEL_TWO_SIDE, but doesn't work on IPHONE
    string fileName = filename;
    int len = fileName.length();
    if( fileName[len-7] == 'X')
    {
        _fixNormals = true;
        fileName.replace( fileName.end()-7, fileName.end(), ".model");
    }

    if( ! ResourceManagerS::instance()->selectResource( fileName))
    {
	LOG_ERROR << "Unable to open: [" << fileName << "]" << endl;
        return false;
    }
    ziStream &infile = ResourceManagerS::instance()->getInputStream();

    LOG_INFO << "  Model " << fileName << endl;

    vec3f scale;
    scale.x()=scale.y()=scale.z() = 1.0;

    string line;
    int linecount = 0;
    while( !getline( infile, line).eof())
    {
        linecount++;

        //explicitly skip comments
        if( line[0] == '#') continue;
        Tokenizer  t( line);
        string token = t.next();
        if( token == "Name")
        {
            _name = t.next();
//            LOG_INFO << "Name = [" << _name << "]" << endl;
        }
        else if( token == "Scale")
        {
            scale.x() = (float)atof( t.next().c_str()) * MODEL_SCALE;
            scale.y() = (float)atof( t.next().c_str()) * MODEL_SCALE;
            scale.z() = (float)atof( t.next().c_str()) * MODEL_SCALE;
#if 0
            LOG_INFO << "Scale = [" 
                     << scale.x() << ","
                     << scale.y() << ","
                     << scale.z()
                     << "]" << endl;
#endif
        }
        else if( token == "Colors")
        {
            _numColors = atoi( t.next().c_str());
            if( !readColors( infile, linecount))
            {
		LOG_ERROR << "Error reading colors:" 
                      << " line:" << linecount << endl;
		return false;
            }
        }
        else if( token == "Vertices")
        {
            int numVerts = atoi( t.next().c_str());
            verifyAndAssign( numVerts, _numVerts);
            if( !readVertices( infile, scale, linecount))
            {
		LOG_ERROR << "Error reading vertices:" 
                      << " line:" << linecount << endl;
		return false;
            }
        }
        else if( token == "Normals")
        {
            int numNorms = atoi( t.next().c_str());
            verifyAndAssign( numNorms, _numVerts);
            if( !readNormals( infile, linecount))
            {
		LOG_ERROR << "Error reading normals:" 
                      << " line:" << linecount << endl;
		return false;
            }
        }
        else if( token == "Faces")
        {
            _numFaces = atoi( t.next().c_str());
            _faces = new FaceInfo[ _numFaces];
            if( !readFaces( infile, linecount))
            {
		LOG_ERROR << "Error reading faces:" 
                      << " line:" << linecount << endl;
		return false;
            }
        }
        else if( token == "Offset")
        {
            _offset.x() = (float)atof( t.next().c_str()) * MODEL_SCALE;
            _offset.y() = (float)atof( t.next().c_str()) * MODEL_SCALE;
            _offset.z() = (float)atof( t.next().c_str()) * MODEL_SCALE;
#if 0
            LOG_INFO << "Offset = [" 
                     << _offset.x() << ","
                     << _offset.y() << ","
                     << _offset.z()
                     << "]" << endl;
#endif
	}
        else
        {
            LOG_ERROR << "Syntax error: [" 
                      << token << "] line:" << linecount << endl;
	    return false;
        }
    }

    prepareModel();

    return true;
}

//read vertices section
bool Model::readVertices( ziStream &infile, const vec3f &scale, int &linecount)
{
    XTRACE();
    string line;
    for( int i=0; i<_numVerts; i++)
    {
	if( getline( infile, line).eof())
        {
            return false;
        }
        linecount++;

        Tokenizer t( line);

        float x = (float)atof( t.next().c_str());
        float y = (float)atof( t.next().c_str());
        float z = (float)atof( t.next().c_str());

        _verts.push_back( vec3f( x * scale.x(), y * scale.y(), z * scale.z()) );

        if( t.tokensReturned() != 3) return false;

	if( i==0)
	{
	    _min = _verts[i];
	    _max = _verts[i];
	}
	else
	{
	    if( _verts[i].x() < _min.x()) _min.x() = _verts[i].x();
	    if( _verts[i].y() < _min.y()) _min.y() = _verts[i].y();
	    if( _verts[i].z() < _min.z()) _min.z() = _verts[i].z();

	    if( _verts[i].x() > _max.x()) _max.x() = _verts[i].x();
	    if( _verts[i].y() > _max.y()) _max.y() = _verts[i].y();
	    if( _verts[i].z() > _max.z()) _max.z() = _verts[i].z();
	}
    }

    return true;
}

//read normals section
bool Model::readNormals( ziStream &infile, int &linecount)
{
    XTRACE();
    string line;
    for( int i=0; i<_numVerts; i++)
    {
	if( getline( infile, line).eof())
        {
            return false;
        }
        linecount++;

        Tokenizer t( line);

        float x = (float)atof( t.next().c_str());
        float y = (float)atof( t.next().c_str());
        float z = (float)atof( t.next().c_str());

        vec3f norm ( x, y, z);
        //norm.normalize();

        _norms.push_back( norm);

        if( _fixNormals && (_norms[i].z() < 0))
        {
            _norms[i].x() = -_norms[i].x();
            _norms[i].y() = -_norms[i].y();
            _norms[i].z() = -_norms[i].z();
        }

        if( t.tokensReturned() != 3) return false;
    }

    return true;
}

//read faces section
bool Model::readFaces( ziStream &infile, int &linecount)
{
    XTRACE();
    string line;
    for( int i=0; i<_numFaces; i++)
    {
	if( getline( infile, line).eof())
        {
            return false;
        }
        linecount++;

        Tokenizer t( line);

        _faces[i].v1 = atoi( t.next().c_str());
        _faces[i].v2 = atoi( t.next().c_str());
        _faces[i].v3 = atoi( t.next().c_str());
        _faces[i].v4 = atoi( t.next().c_str());
        _faces[i].smooth = (atoi( t.next().c_str()) == 1);
        _faces[i].color = atoi( t.next().c_str());

        if( t.tokensReturned() != 6) return false;
    }

    return true;
}

//read colors section
bool Model::readColors( ziStream &infile, int &linecount)
{
    XTRACE();
    string line;
    for( int i=0; i<_numColors; i++)
    {
	if( getline( infile, line).eof())
        {
            return false;
        }
        linecount++;

        Tokenizer t( line);
        float r = (float)atof( t.next().c_str());
        float g = (float)atof( t.next().c_str());
        float b = (float)atof( t.next().c_str());

        _colors.push_back( vec4f(r, g, b, 1.0f)); 

        if( t.tokensReturned() != 3) return false;
    }

    return true;
}

//hum
void Model::verifyAndAssign( const int newVert, int & currVertVal)
{
    XTRACE();
    if( (currVertVal != 0) && (newVert!=currVertVal))
    {
	LOG_ERROR << "Vertex count inconsistency!" << endl; 
    }
    else
    {
	currVertVal = newVert;
    }
}

void Model::reset( void)
{
    _vbo.reset();
}

//re-load model
void Model::reload( void)
{
    XTRACE();
    //assuming all VBOs were nuked when resolution switched
    prepareModel();
}

void Model::addTriangle( const vec4f & color, const vec4f & avgNormal, bool hasColor, int v1, int v2, int v3, bool smooth,
    std::vector<vec3f> &verts, std::vector<vec3f> &norms, std::vector<vec4f> &colors)
{
    int f[3]; 
    f[0] = v1; f[1] = v2; f[2] = v3;

    for( int i=0; i<3; i++)
    {
        if( hasColor) 
        {
            colors.push_back( color);
        }

	if( smooth)
	{
	    norms.push_back( _norms[ f[i]] );
	}
	else
	{
	    norms.push_back( avgNormal);
	}

	verts.push_back( _verts[ f[i]] );
    }
}

void Model::draw()
{
    _vbo.draw();
}

void Model::prepareModel( void)
{
    std::vector<vec3f> verts;
    std::vector<vec3f> norms;
    std::vector<vec2f> texels;
    std::vector<vec4f> colors;

    vec3f color(1.0, 0.0, 0.0);
    for( int i=0; i<_numFaces; i++)
    {
        if( _numColors) color = _colors[ _faces[i].color];

        vec3f avgNormal = _norms[ _faces[i].v1] + _norms[ _faces[i].v2] + _norms[ _faces[i].v3];
        if( _faces[i].v4 != 0)
        {
            avgNormal +=  _norms[ _faces[i].v4]; 
        }
        avgNormal.normalize();
        
        addTriangle( color, avgNormal, _numColors > 0, _faces[i].v1, _faces[i].v2, _faces[i].v3, _faces[i].smooth,
            verts, norms, colors);        
        if( _faces[i].v4 != 0) //quad
        {
            addTriangle( color, avgNormal, _numColors > 0, _faces[i].v3, _faces[i].v4, _faces[i].v1, _faces[i].smooth,
                verts, norms, colors);
        }
    }

    _vbo.init(verts, norms, texels, colors);
}
