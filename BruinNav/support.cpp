//
//  support.cpp
//  Project4
//
//  Created by 文洁 顾 on 3/16/17.
//  Copyright © 2017 Wenjie Gu. All rights reserved.
//

#include "support.h"

bool operator<(const GeoCoord& lhs,const GeoCoord& rhs)
{
    if(lhs.latitude < rhs.latitude) return true;
    else if (lhs.latitude == rhs.latitude && lhs.longitude < rhs.longitude) return true;
    else return false;
}

bool operator==(const GeoCoord& lhs, const GeoCoord& rhs)
{
    if(lhs.latitude == rhs.latitude && lhs.longitude == rhs.longitude) return true;
    else return false;
}

bool operator>(const GeoCoord& lhs,const GeoCoord& rhs)
{
    if(lhs.latitude > rhs.latitude) return true;
    else if (lhs.latitude == rhs.latitude && lhs.longitude > rhs.longitude) return true;
    else return false;
}

bool operator!=(const GeoCoord& lhs, const GeoCoord& rhs)
{
    if(lhs.latitude == rhs.latitude && lhs.longitude == rhs.longitude) return false;
    else return true;
}
