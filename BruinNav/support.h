//
//  support.hpp
//  Project4
//
//  Created by 文洁 顾 on 3/16/17.
//  Copyright © 2017 Wenjie Gu. All rights reserved.
//

#ifndef support_hpp
#define support_hpp

#include "provided.h"
bool operator<(const GeoCoord& lhs,const GeoCoord& rhs);
bool operator==(const GeoCoord& lhs, const GeoCoord& rhs);
bool operator>(const GeoCoord& lhs,const GeoCoord& rhs);
bool operator!=(const GeoCoord& lhs, const GeoCoord& rhs);
#endif /* support_hpp */
