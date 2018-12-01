#include "provided.h"
#include <string>
#include <vector>
#include <queue>
#include "MyMap.h"
#include "support.h"

using namespace std;


class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    MapLoader m_ml;
    AttractionMapper m_am;
    SegmentMapper m_sm;
    struct Vertex
    {
        Vertex(GeoCoord sGc,GeoCoord sParent, double sgScore, double sfScore) :gc(sGc), parent(sParent), gScore(sgScore), fScore(sfScore){}
        bool operator<(const Vertex& other) const
        {
            if (fScore < other.fScore)
                return false;
            else return true;
        }
        GeoCoord gc;
        GeoCoord parent;
        double gScore;
        double fScore;
    };
    bool reachDestination(const Vertex& vtx, const GeoCoord& end) const
    {
        StreetSegment ss = m_sm.getSegments(end)[0];
        if((vtx.gc == ss.segment.start && vtx.parent == ss.segment.end) || (vtx.parent == ss.segment.start && vtx.gc == ss.segment.end))
            return true;
        else return false;
    }
    NavResult ReconstructPath(GeoCoord start, GeoCoord end, GeoCoord current, vector<NavSegment> &directions, MyMap<GeoCoord, GeoCoord>& cameFrom) const;
    void translate(vector<StreetSegment>& path, vector<NavSegment>& directions) const;
};

NavigatorImpl::NavigatorImpl()
{
    m_ml.load("mapdata.txt");
    m_am.init(m_ml);
    m_sm.init(m_ml);
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    bool loadResult = m_ml.load(mapFile);
    m_am.init(m_ml);
    m_sm.init(m_ml);
    return loadResult;
}

void NavigatorImpl::translate(vector<StreetSegment>& path, vector<NavSegment>& directions) const
{
    for(int i = path.size()-1; i >= 0; i--)
    {
        StreetSegment current = path[i];
        double angle = angleOfLine(current.segment);
        string angles[] = {"east", "northeast", "north", "northwest", "west", "southwest", "south", "southeast", "east"};
        string angle_s = angles[ int((angle+22.4999999) / 45) ];
        double streetLength = distanceEarthMiles(current.segment.start, current.segment.end);
        NavSegment proceed(angle_s, current.streetName, streetLength, current.segment);
        directions.push_back(proceed);
        
        if(i != 0)
        {
            StreetSegment next = path[i-1];
            if(current.streetName ==  next.streetName) continue;
            angle = angleBetween2Lines(current.segment, next.segment);
            string turn = (angle < 180) ? "left" : "right";
            NavSegment turnstyle(turn, next.streetName);
            directions.push_back(turnstyle);
        }
    }
}

NavResult NavigatorImpl::ReconstructPath(GeoCoord start, GeoCoord end, GeoCoord current, vector<NavSegment> &directions, MyMap<GeoCoord, GeoCoord>& cameFrom) const
{
    vector<StreetSegment> path;
    StreetSegment start_street = m_sm.getSegments(start)[0];
    StreetSegment end_street = m_sm.getSegments(end)[0];
    
    if (start_street.segment.start == end_street.segment.start && start_street.segment.end == end_street.segment.end)
    {
        start_street.segment.start = start;
        start_street.segment.end = end;
        path.push_back(start_street);
        translate(path, directions);
        return NAV_SUCCESS;
    }
    end_street.segment.start = current;
    end_street.segment.end = end;
    path.push_back(end_street);

    GeoCoord gc = current;
    while(gc != start)
    {
        GeoCoord* gc_parent = cameFrom.find(gc);
        if(gc_parent == nullptr) return NAV_NO_ROUTE;
        if (*gc_parent == start) {
            start_street.segment.start = start;
            start_street.segment.end = gc;
            path.push_back(start_street);
            break;
        }
        
        vector<StreetSegment> candidates = m_sm.getSegments(gc);
        for(int i = 0; i < candidates.size(); i++)
        {
            StreetSegment seg = candidates[i];
            if(seg.segment.start ==  *gc_parent && seg.segment.end == gc){
                path.push_back(seg);
                break;
            }
            if(seg.segment.start == gc && seg.segment.end == *gc_parent)
            {
                swap(seg.segment.start, seg.segment.end);
                path.push_back(seg);
                break;
            }
        }
        gc = *gc_parent;
    }
    translate(path, directions);
    return NAV_SUCCESS;
}


NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord gc_start, gc_end;
    if(!m_am.getGeoCoord(start, gc_start)) return NAV_BAD_SOURCE;
    if(!m_am.getGeoCoord(end, gc_end)) return NAV_BAD_DESTINATION;
    
    priority_queue<Vertex> OpenSet;
    MyMap<GeoCoord, bool> ClosedSet;
    MyMap<GeoCoord, GeoCoord> cameFrom;
    
    Vertex startPoint(gc_start, gc_start, 0, distanceEarthKM(gc_start, gc_end));
    OpenSet.push(startPoint);
    
    while(!OpenSet.empty())
    {
        Vertex current = OpenSet.top();
        GeoCoord currentGeo = current.gc;
        GeoCoord currentParent = current.parent;
        OpenSet.pop();
        
        if (ClosedSet.find(currentGeo) != nullptr)
            continue;
        ClosedSet.associate(currentGeo, true);
        
        cameFrom.associate(currentGeo, currentParent);
        
        if(reachDestination(current, gc_end)){
            return ReconstructPath(gc_start, gc_end, currentParent, directions, cameFrom);
        }
        
        vector<StreetSegment> neighborhood = m_sm.getSegments(currentGeo);
        for(int i = 0; i < neighborhood.size(); i++)
        {
            GeoCoord neighborStart =neighborhood[i].segment.start;
            GeoCoord neighborEnd = neighborhood[i].segment.end;
            if(neighborStart != currentGeo)
            {
                double gx = current.gScore + distanceEarthMiles(currentGeo, neighborStart);
                double fx = gx + distanceEarthMiles(neighborStart, gc_end);
                Vertex neighbor(neighborStart, currentGeo, gx, fx);
                OpenSet.push(neighbor);
            }
            if(neighborEnd != currentGeo)
            {
                double gx = current.gScore + distanceEarthMiles(currentGeo, neighborEnd);
                double fx = gx + distanceEarthMiles(neighborEnd, gc_end);
                Vertex neighbor(neighborEnd, currentGeo, gx, fx);
                OpenSet.push(neighbor);
            }
        }
    }
    return NAV_NO_ROUTE;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
