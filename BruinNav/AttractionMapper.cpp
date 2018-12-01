#include "provided.h"
#include <string>
#include "MyMap.h"
#include <iostream>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<std::string, GeoCoord> m_attractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment ss;
        ml.getSegment(i, ss);
        if (!ss.attractions.empty())
        {
            size_t atnsize = ss.attractions.size();
            for (int j = 0; j < atnsize; j++){
                for(int k = 0; k < ss.attractions[j].name.size(); k++)
                    ss.attractions[j].name[k] = tolower(ss.attractions[j].name[k]);
                m_attractions.associate(ss.attractions[j].name, ss.attractions[j].geocoordinates);
                
            }
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for ( int i = 0; i < attraction.size(); i++)
        attraction[i] = tolower(attraction[i]);
    const GeoCoord* ptrAttraction = m_attractions.find(attraction);
    if(ptrAttraction == nullptr) return false;
    else{
        gc = *ptrAttraction;
        return true;
    }
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
