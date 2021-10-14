
//#include <TRDBase/GeometryBase.h>
// #include <DataFormatsTRD/Hit.h>
// #include <TRDBase/GeometryBase.h>
// #include <TRDBase/Geometry.h>
// #include <iostream>

class SpacePointConverter
{
// private:
//   o2::trd::Geometry* geo;

public:
  SpacePointConverter() {
    o2::trd::Geometry::instance()->createPadPlaneArray();
  }

  array<double,3> Hit2RowColTime(const o2::trd::Hit& hit)
  {
    return Local2RowColTime(hit.GetDetectorID(),
                            hit.getLocalT(), hit.getLocalC(), hit.getLocalR());
  }


  array<double,3> Local2RowColTime(int det, double x, double y, double z)
  {

    double vDrift = 1.5625; // in cm/us
    double t0 = 4.0; // time (in timebins) of start of drift region

    auto geo = o2::trd::Geometry::instance();
    auto padPlane = geo->getPadPlane((det)%6, (det/6)%5);

    array<double,3> rct;

    double iPadLen = padPlane->getLengthIPad();
    double oPadLen = padPlane->getLengthOPad();
    int nRows = padPlane->getNrows();

    double lengthCorr = padPlane->getLengthOPad()/padPlane->getLengthIPad();

    // calculate position based on inner pad length
    rct[0] = - z / padPlane->getLengthIPad() + padPlane->getNrows()/2;

    // correct row for outer pad rows
    if (rct[0] <= 1.0) {
      rct[0] = 1.0 - (1.0-rct[0])*lengthCorr;
    }

    if (rct[0] >= double(nRows-1)) {
      rct[0] = double(nRows-1) + (rct[0] - double(nRows-1))*lengthCorr;
    }

    // sanity check: is the padrow coordinate reasonable?
    if ( rct[0]<0.0 || rct[0]>double(nRows) ) {
      std::cout << "ERROR: hit with z=" << z << ", padrow " << rct[0]
           << " outside of chamber" << std::endl;
    }

    // simple conversion of pad / local y coordinate
    // ignore different width of outer pad
    rct[1] = y / padPlane->getWidthIPad() + 144./2.;

    // time coordinate
    if (x<-0.35) {
      // drift region
      rct[2] = t0 - (x+0.35) / (vDrift/10.0);
    } else {
      // anode region: very rough guess
      rct[2] = t0 - 1.0 + fabs(x);
    }

    return rct;
  }
};
