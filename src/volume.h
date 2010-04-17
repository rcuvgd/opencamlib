/*  Copyright 2010 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VOLUME_H
#define VOLUME_H

#include <iostream>
#include <list>
#include <vector>

class Point;
class Triangle;
class MillingCutter;
class CylCutter;
class Bbox;
class Ocode;

/// bounding-box volume
class Bbox {
    public:
        Bbox();
        double maxx;
        double minx;
        double maxy;
        double miny;
        double maxz;
        double minz;
        bool isInside(Point& p) const;
        void addPoint(Point& p);
};



/// base-class for defining volumes to build octrees
class OCTVolume {
    public:
        OCTVolume(){};
        
        /// return true if Point p is inside volume
        virtual bool isInside(Point& p) const = 0;
        /// return true if we are in the bounding box
        bool isInsideBB(Point& p) const;
        bool isInsideBBo(Ocode& o) const;

        /// bounding-box
        Bbox bb;
};

/* required wrapper class for virtual functions in boost-python */
/// \brief a wrapper around OCTVolume required for boost-python
class OCTVolumeWrap : public OCTVolume, public bp::wrapper<OCTVolume>
{
    public:
    bool isInside(Point &p) const
    {
        return this->get_override("isInside")(p);
    }

};



/// sphere centered at center
class SphereOCTVolume: public OCTVolume {
    public:
        SphereOCTVolume();
        Point center;
        double radius;
        bool isInside(Point& p) const;
        //bool isInsideBB(Point& p) const;
        void calcBB();
};

/// cube at center with side-length side
class CubeOCTVolume: public OCTVolume {
    public:
        CubeOCTVolume();
        Point center;
        double side;
        bool isInside(Point& p) const;
        void calcBB();
};

/// cylinder volume
class CylinderOCTVolume: public OCTVolume {
    public:
        CylinderOCTVolume();
        Point p1;
        Point p2;
        double radius;
        bool isInside(Point& p) const;
        void calcBB();
};

/// box-volume
/// from corner, go out in the three directions v1,v2,v3
/// interior points = corner + a*v1 + b*v2 + c*v3  
/// where a, b, c are in [0,1]
class BoxOCTVolume: public OCTVolume {
    public:
        BoxOCTVolume();
        Point corner;
        Point v1;
        Point v2;
        Point v3;
        bool isInside(Point& p) const;
};

class EtubeOCTVolume: public OCTVolume {
    public:
        EtubeOCTVolume();
        EtubeOCTVolume(Point& p1in, Point& p2in, Point& ain, Point& bin);
        Point p1; // start of move
        Point p2; // end of move
        Point a; // a-axis of ellipse
        Point b; // b-axis of ellipse
        bool isInside(Point& p) const;
};


/// cutter-swept volume of a CylCutter
class CylMoveOCTVolume: public OCTVolume {
    public:
        CylMoveOCTVolume() {};
        CylMoveOCTVolume(const CylCutter& c, const Point& p1, const Point& p2);
        Point p1;
        Point p2;
        CylCutter c;
        CylinderOCTVolume c1;
        CylinderOCTVolume c2;
        EtubeOCTVolume etube;
        BoxOCTVolume box;
        bool isInside(Point& p) const;
};


#endif