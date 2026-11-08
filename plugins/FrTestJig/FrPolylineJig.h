#pragma once
#include "dbjig.h"   
#include "dbpl.h"    
#include "dbmain.h"  
#include "dbents.h"  
#include "gept2dar.h"
#include <FrPolyline.h>

class FrPolylineJig: public AcEdJig {
    AcDbPolyline* line_;
    AcGePoint3d currentPt_;

    public:
    FrPolylineJig(FrPolyline& line) : line_(line.entity())
};