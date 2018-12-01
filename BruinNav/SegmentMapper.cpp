#include "provided.h"
#include <vector>
#include "MyMap.h"
#include "support.h"
using namespace std;


class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment> > m_segments;
    void AddToMap(const GeoCoord& gc, const StreetSegment& ss);
};


SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::AddToMap(const GeoCoord& gc, const StreetSegment& ss)
{
    vector<StreetSegment>* ptr = m_segments.find(gc);
    if (ptr == nullptr)
    {
        vector<StreetSegment> col_ss;
        col_ss.push_back(ss);
        m_segments.associate(gc, col_ss);
    }
    else {
        ptr->push_back(ss);
    }
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    StreetSegment ss;
    for(int i = 0; i < ml.getNumSegments(); i++)
    {
        ml.getSegment(i, ss);
        GeoCoord start = ss.segment.start, end = ss.segment.end;
        AddToMap(start, ss);
        AddToMap(end, ss);
        for(int k = 0; k < ss.attractions.size(); k++)
        {
            AddToMap(ss.attractions[k].geocoordinates, ss);
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    const vector<StreetSegment>* ptrSegment = m_segments.find(gc);
	return *ptrSegment;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
