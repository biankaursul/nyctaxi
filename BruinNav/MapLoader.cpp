#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    std::vector<StreetSegment> m_streetsegs;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if (!infile) return false;
    else{
        std::string name;
        while(getline(infile, name))
        {
            StreetSegment ss1;
            ss1.streetName = name;
            
            std::string line;
            getline(infile, line);
            line = line + ' ';
            std::string coord;
            vector<std::string> coords;
            for(int i = 0; i < line.size(); i++)
            {
                if (coord.empty()) {
                    if (line[i] == ',' || line[i] == ' ')
                        continue;
                    coord += line[i];
                }
                else {
                    if (line[i] == ',' || line[i] == ' ') {
                        coords.push_back(coord);
                        coord = "";
                    }
                    else {
                        coord += line[i];
                    }
                }
            }
            GeoCoord start(coords[0], coords[1]);
            GeoCoord end(coords[2], coords[3]);
            GeoSegment seg(start, end);
            ss1.segment = seg;
            
            int numAttractions;
            infile >> numAttractions;
            infile.ignore(10000, '\n');
            for(int j = 0; j < numAttractions; j++)
            {
                Attraction atn;
                std::string attraction;
                getline(infile, attraction);
                attraction += ' ';
                std::string attractionName;
                vector<std::string> coords2;
                std::string coord2;
                GeoCoord attractionLoc;
                bool namedone = false;
                for(int k = 0; k < attraction.size(); k++)
                {
                    if (attraction[k] == '|'){
                        namedone = true;
                        
                        continue;
                    }
                    if(!namedone)
                    {
                        attractionName += attraction[k];
                    }
                    else{
                        if (coord2.empty()) {
                            if (attraction[k] == ',' || attraction[k] == ' ')
                                continue;
                            coord2 += attraction[k];
                        }
                        else {
                            if (attraction[k] == ',' || attraction[k] == ' ') {
                                coords2.push_back(coord2);
                                coord2 = "";
                            }
                            else {
                                coord2 += attraction[k];
                            }
                        }
                    }
                }
                atn.name = attractionName;
                GeoCoord atngeo(coords2[0],coords2[1]);
                atn.geocoordinates = atngeo;
                ss1.attractions.push_back(atn);
            }
            m_streetsegs.push_back(ss1);
        }

        
        return true;
    }
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_streetsegs.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    size_t num = getNumSegments();
    if(segNum >= 0 && segNum < num){
        seg = m_streetsegs[segNum];
        return true;
    }
    else return false;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
