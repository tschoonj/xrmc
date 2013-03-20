/*
Copyright (C) 2013 Bruno Golosio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///////////////////////////////////
//     xrmc_geom3d.h             //
//        31/01/2013             //
//   author : Bruno Golosio      //
///////////////////////////////////
//  geom3d, quadric, quadricarray, qvolume, qvolumearray class definitions
//

#ifndef GEOM3DH
#define GEOM3DH
#include <string>
#include "xrmc_device.h"
#include "xrmc_math.h"
#include "xrmc_composition.h"

using namespace std;

// quadric class definition, member variables and functions
class quadric
{
public:
  matr4 Matr; // 4 x 4 real matrix
  int NInters; // number of intersection of a trajectory with the quadric
  double tInters[2]; // parametric coordinates of the intersections
  int Enter[2]; // crossing directions: from outside to inside or viceversa

  quadric(); // constructor
  int Print(); // display matrix elements method
  double Prod3(double *x, double *y); // evaluates the quadratic form 
                                      // Mij * xi * yj on 3d vectors
  double Prod4(double *x, double *y); // evaluates the quadratic form 
                                      // Mij * xi * yj on 4d vectors
  int Inside(vect3 x); // check if x is inside (0) or outside (1) the quadric
  int Ellipsoid(double *x0, double *a); // build ellipsoid quadric
  int Plane(double *x0, double *u); // build plane quadric
  int CilinderX(double *x0, double *a); // Cylinder parallel to x axis
  int CilinderY(double *x0, double *a); // Cylinder parallel to y axis
  int CilinderZ(double *x0, double *a); // Cylinder parallel to z axis
  int Intersect(vect3 x0, vect3 u);// intersections of a line with the quadric
  int Transform(matr4 M); // Congruence transform of the quadric with matrix M
  int SetElem(int i, int j, double elem); // Mij and Mji to the value elem
  int ChangeSign(); // change the sign
};

typedef map<string, quadric*> quadric_map;
typedef pair<string, quadric*> quadric_map_pair;
typedef pair<quadric_map::iterator, bool> quadric_map_insert_pair;

// quadricarray class definition, member variables and functions
class quadricarray : public xrmc_device
{
 public:
  int NQuadr; // number of quadrics in the array
  int MaxNQuadr; // maximum number of quadrics in the array
  quadric *Quadr; // pointer to the quadric array
  quadric_map QuadricMap; // map of quadrics with their names

  ~quadricarray() { // destructor
    if (Quadr!=NULL) delete[] Quadr;
  }
  // constructor
  quadricarray(string dev_name) {
    Quadr = NULL;
    NQuadr = 0;
    xrmc_device(dev_name, "quadricarray");
  }
  int Load(FILE *fp); // method for loading quadric array from file
  int SetDefault(); // set default values for quadricarray parameters

  // insert name and pointer to the quadric in the quadric map
  int MapQuadric(FILE *fp);
  quadricarray *Clone(string dev_name);
};

//////////////////////////////////////////////////////////////////////
// qvolume class definition, member variables and functions
// qvolume is a 3d object delimited by quadric surfaces
//////////////////////////////////////////////////////////////////////
class qvolume
{
 public:
  int NQuadr; // number of quadrics delimiting the object
  int iPhaseIn; // index of the phase inside the object
  int iPhaseOut; // index of the phase surrounding the object
  string PhaseInName; // name of the phase inside the object
  string PhaseOutName; // name of the phase surrounding the object
  quadric **Quadr; // array of pointers to the quadrics delimiting the object

  ~qvolume() { // destructor
    if (Quadr!=NULL) delete[] Quadr;
  }
  qvolume() { // constructor
    Quadr = NULL;
    NQuadr = 0;
  }
  int Init(string s_ph_in, string s_ph_out, int n_quadr); // initializ. method
  // method for finding the intersections of a line with the object
  int Intersect(vect3 x0, vect3 u, double *t, int *iph0, int *iph1, \
		 int *n_inters);
  qvolume& operator= (const qvolume &QVolume);
};


class geom3d : public xrmc_device
{
 public:
  vect3 X; // sample region center coordinates
  double HW[3]; // sample region half sides

  quadricarray *QArr; // array of quadrics used in the geometric description
  string QArrName;
  composition *Comp; // input composition device
  string CompName;

  int NQVol; // number of 3d objects used in the geometric description
  int MaxNQVol; // maximum number of 3d objects in the geometric description
  qvolume *QVol; // array of 3d objects

  ~geom3d() { // destructor
    for (int i=0; i<NQVol; i++) {
      if (QVolMap[i]!=NULL) delete[] QVolMap[i];
    }
    if (QVolMap!=NULL) delete[] QVolMap;
    if (QVol!=NULL) delete[] QVol;
  }
  // constructor
  geom3d(string dev_name) {
    QVolMap = NULL;
    QVol = NULL;
    NQVol = 0;
    xrmc_device(dev_name, "geom3d");
  }
  int Load(FILE *fp); // method for loading the geometric description
  int ImportDevice(xrmc_device_map *dev_map); // import device method
  int Init(); // geometry initialization method
  int SetDefault(); // set default values for geometric description

  // method for finding the intersections of a straight line
  //  with all quadrics and all 3d objects
  int Intersect(vect3 x0, vect3 u, double *t, int *iph0, int *iph1, \
		int *n_inters);
  geom3d *Clone(string dev_name);
 private:
  string **QVolMap; // map of the quadrics delimiting the objects 
};

#endif
