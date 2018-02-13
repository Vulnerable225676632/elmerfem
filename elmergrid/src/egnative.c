/*  
   ElmerGrid - A simple mesh generation and manipulation utility  
   Copyright (C) 1995- , CSC - IT Center for Science Ltd.   

   Author: Peter R�back
   Email: Peter.Raback@csc.fi
   Address: CSC - IT Center for Science Ltd.
            Keilaranta 14
            02101 Espoo, Finland

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


 /* -----------------------:  egnative.c  :----------------------

   These subroutines are used to create a mesh. They operate on 
   variables that are of type MeshType and GridType. The routines 
   are used to initialize the grid used for the calculation. 
   */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <stdarg.h>

#include "egutils.h"
#include "egdef.h"
#include "egtypes.h"
#include "egnative.h"
#include "egmesh.h"
#include "egelmer.h"


#define DEBUG 0


void InitGrid(struct GridType *grid)
/* Initializes the grid of a specific mesh. A grid can differ 
   between various differential equations. 
   */
{
  int i,j;

  grid->layered = FALSE;
  grid->layeredbc = TRUE;
  grid->layerbcoffset = 0;
  grid->triangles = FALSE;
  grid->triangleangle = 0.0;
  grid->partitions = FALSE;
  grid->wantedelems = 0;
  grid->wantedelems3d = 0;
  grid->wantednodes3d = 0;
  grid->limitdx = 0.0;
  grid->limitdxverify = FALSE;

  grid->dimension = 2;
  grid->xcells = grid->ycells = grid->zcells = 0;
  grid->nocells = 0;
  grid->noknots = grid->noelements = 0;
  grid->totxelems = grid->totyelems = grid->totzelems = 0;
  grid->minxelems = grid->minyelems = 1;
  grid->minzelems = 2;
  grid->firstmaterial = 1;
  grid->lastmaterial = INT_MAX;
  grid->autoratio = 1;
  grid->xyratio = 1.0;
  grid->xzratio = 1.0;
  grid->nonodes = 4;
  grid->numbering = NUMBER_XY;
  grid->rotate = FALSE;
  grid->rotateradius1 = 0.0;
  grid->rotateradius2 = 0.0;
  grid->rotateimprove = 1.0;
  grid->rotateblocks = 4;
  grid->rotatecartesian = FALSE;
  grid->reduceordermatmin = 0;
  grid->reduceordermatmax = 0;
  grid->polarradius = 1.0;
  grid->elemorder = 1;
  grid->elemmidpoints = FALSE;

  grid->rotatecurve = FALSE;
  grid->curverad = 0.5;
  grid->curveangle = 90.0;
  grid->curvezet = 1.0;

  for(i=0;i<=MAXCELLS;i++) {
    grid->xelems[i] = 0;
    grid->xlinear[i] = TRUE;
    grid->xratios[i] = 1.;
    grid->xexpand[i] = 1.;
    grid->xdens[i] = 1.;
    grid->x[i] = 0.; 
  }
  for(i=0;i<=MAXCELLS;i++) {
    grid->yelems[i] = 0;
    grid->ylinear[i] = TRUE;
    grid->yratios[i] = 1.0;
    grid->yexpand[i] = 1.0;
    grid->ydens[i] = 1.0;
    grid->y[i] = 0.;
  }
  for(i=0;i<=MAXCELLS;i++) {
    grid->zelems[i] = 0;
    grid->zlinear[i] = TRUE;
    grid->zratios[i] = 1.0;
    grid->zexpand[i] = 1.0;
    grid->zdens[i] = 1.0;
    grid->z[i] = 0.;
    grid->zfirstmaterial[i] = 1;
    grid->zlastmaterial[i] = INT_MAX;
    grid->zmaterial[i] = 0; 
  }

  grid->zmaterialmapexists = FALSE;
  grid->zhelicityexists = FALSE;
  grid->zhelicity = 0.0;

  /* Initilizes the numbering of the cells. */
  for(j=0;j<=MAXCELLS+1;j++)
    for(i=0;i<=MAXCELLS+1;i++) 
      grid->structure[j][i] = MAT_NOTHING; 

  for(j=0;j<=MAXCELLS+1;j++)
    for(i=0;i<=MAXCELLS+1;i++) 
      grid->numbered[j][i] = 0; 

  grid->noboundaries = 0;
  for(i=0;i<MAXBOUNDARIES;i++) {
    grid->boundint[i] = 0;
    grid->boundext[i] = 0;
    grid->boundsolid[i] = 0;
    grid->boundtype[i] = 0;
  }

  grid->mappings = 0;
  for(i=0;i<MAXMAPPINGS;i++) {
    grid->mappingtype[i] = 0;
    grid->mappingline[i] = 0;    
    grid->mappinglimits[2*i] = 0;    
    grid->mappinglimits[2*i+1] = 0;    
    grid->mappingpoints[i] = 0;
  }
}


void ExampleGrid1D(struct GridType **grids,int *nogrids,int info) 
/* Creates an example grid that might be used to analyze 
   flow trough a step. */
{
  int j;
  struct GridType *grid;

  (*nogrids) = 1;
  (*grids) = (struct GridType*)
    malloc((size_t) (*nogrids)*sizeof(struct GridType)); 

  grid = grids[0];

  InitGrid(grid);

  grid->nonodes = 2;
  grid->xcells = 3;
  grid->ycells = 1;
  grid->wantedelems = 40;
  grid->firstmaterial = 1;
  grid->lastmaterial = 1;
  grid->autoratio = 1;
  grid->coordsystem = COORD_CART1;
  grid->numbering = NUMBER_1D;

  grid->x[0] = 0.0;
  grid->x[1] = 1.0;
  grid->x[2] = 3.0;
  grid->x[3] = 4.0;

  grid->xexpand[1] = 2.0;
  grid->xexpand[3] = 0.5;
  grid->xdens[2] = 0.5;

  for(j=1;j<=3;j++)
    grid->structure[1][j] = 1;

  grid->noboundaries = 2;
  grid->boundint[0] = 1;
  grid->boundint[1] = 1;

  grid->boundext[0] = -1;
  grid->boundext[1] = -2;

  grid->boundtype[0] = 1;
  grid->boundtype[1] = 2;

  grid->boundsolid[0] = 1;
  grid->boundsolid[1] = 1;

  grid->mappings = 0;
  
  if(info) printf("A simple 1D example mesh was created\n");
}


void ExampleGrid2D(struct GridType **grids,int *nogrids,int info) 
/* Creates an example grid that might be used to analyze 
   flow trough a step. */
{
  int j;
  struct GridType *grid;

  (*nogrids) = 1;
  (*grids) = (struct GridType*)
    malloc((size_t) (*nogrids)*sizeof(struct GridType)); 

  grid = grids[0];

  InitGrid(grid);

  grid->xcells = 4;
  grid->ycells = 3;
  grid->wantedelems = 100;
  grid->totxelems = grid->totyelems = grid->totzelems = 10;
  grid->firstmaterial = 1;
  grid->lastmaterial = 1;
  grid->autoratio = 1;
  grid->coordsystem = COORD_CART2;
  grid->numbering = NUMBER_XY;

  grid->x[0] = -1.0;
  grid->x[1] = 0.0;
  grid->x[2] = 2.0;
  grid->x[3] = 6.0;
  grid->x[4] = 7.0;

  grid->y[0] = 0.0;
  grid->y[1] = 0.5;
  grid->y[2] = 0.75;
  grid->y[3] = 1.0;

  grid->xexpand[2] = 0.4;
  grid->xexpand[3] = 5.0;

  grid->yexpand[2] = 2.0;
  grid->yexpand[3] = 0.5;

  for(j=1;j<=3;j++)
    grid->structure[j][1] = 2;
  for(j=1;j<=3;j++)
    grid->structure[j][2] = 1;
  grid->structure[1][2] = 0;
  for(j=1;j<=3;j++)
    grid->structure[j][3] = 1;
  for(j=1;j<=3;j++)
    grid->structure[j][4] = 3;

  grid->noboundaries = 3;
  grid->boundint[0] = 1;
  grid->boundint[1] = 1;
  grid->boundint[2] = 1;

  grid->boundext[0] = 2;
  grid->boundext[1] = 3;
  grid->boundext[2] = 0;

  grid->boundtype[0] = 1;
  grid->boundtype[1] = 2;
  grid->boundtype[2] = 3;

  grid->boundsolid[0] = 1;
  grid->boundsolid[1] = 1;
  grid->boundsolid[2] = 1;

  grid->mappings = 2;
  grid->mappingtype[0] = 1;
  grid->mappingtype[1] = 1;
  grid->mappingline[0] = 0;
  grid->mappingline[1] = 3;
  grid->mappinglimits[0] = grid->mappinglimits[1] = 0.5;
  grid->mappinglimits[2] = grid->mappinglimits[3] = 0.5;
  grid->mappingpoints[0] = grid->mappingpoints[1] = 4;
  grid->mappingparams[0] = Rvector(0,grid->mappingpoints[0]);
  grid->mappingparams[1] = Rvector(0,grid->mappingpoints[1]);
  grid->mappingparams[0][0] = 2.0;
  grid->mappingparams[0][1] = 0.0;
  grid->mappingparams[0][2] = 6.0;
  grid->mappingparams[0][3] = -0.2;
  grid->mappingparams[1][0] = 0.0;
  grid->mappingparams[1][1] = 0.0;
  grid->mappingparams[1][2] = 6.0;
  grid->mappingparams[1][3] = 0.3;
  
  if(info) printf("A simple 2D example mesh was created\n");
}

  
void ExampleGrid3D(struct GridType **grids,int *nogrids,int info) 
/* Creates an example grid that might be used to analyze 
   a simple accelaration sensor. */
{
  int i,j;
  struct GridType *grid;

  (*nogrids) = 1;
  (*grids) = (struct GridType*)
    malloc((size_t) (*nogrids)*sizeof(struct GridType)); 

  grid = grids[0];

  InitGrid(grid);

  grid->dimension = 3;
  grid->coordsystem = COORD_CART3;
  grid->layered = TRUE;

  grid->xcells = 4;
  grid->ycells = 5;
  grid->zcells = 5;

  grid->wantedelems = 1000;
  grid->firstmaterial = 1;
  grid->lastmaterial = 3;
  grid->autoratio = 1;
  grid->coordsystem = COORD_CART3;
  grid->numbering = NUMBER_XY;

  grid->x[0] = 0.0;
  grid->x[1] = 0.1;
  grid->x[2] = 0.4;
  grid->x[3] = 0.5;
  grid->x[4] = 0.6;

  grid->y[0] = 0.0;
  grid->y[1] = 0.1;
  grid->y[2] = 0.2;
  grid->y[3] = 0.4;
  grid->y[4] = 0.5;
  grid->y[5] = 0.6;

  grid->z[0] = 0.0;
  grid->z[1] = 0.1;
  grid->z[2] = 0.2;
  grid->z[3] = 0.4;
  grid->z[4] = 0.5;
  grid->z[5] = 0.6;

  for(j=1;j<=5;j++)
    for(i=1;i<=4;i++)
      grid->structure[j][i] = 1;

  grid->structure[2][2] = 3;
  grid->structure[2][3] = 3;
  grid->structure[3][3] = 3;
  grid->structure[4][3] = 3;
  grid->structure[4][2] = 3;

  grid->structure[3][2] = 2;

  grid->zlastmaterial[5] = 3;
  grid->zlastmaterial[2] = 1;
  grid->zlastmaterial[3] = 2;
  grid->zlastmaterial[4] = 1;
  grid->zlastmaterial[5] = 3;

  grid->zmaterial[1] = 1;
  grid->zmaterial[2] = 2;
  grid->zmaterial[3] = 3;
  grid->zmaterial[4] = 4;
  grid->zmaterial[5] = 5;

  grid->noboundaries = 4;
  grid->boundint[0] = 1;
  grid->boundint[1] = 1;
  grid->boundint[2] = 1;
  grid->boundint[2] = 2;

  grid->boundext[0] = 0;
  grid->boundext[1] = 2;
  grid->boundext[2] = 3;
  grid->boundext[3] = 3;

  grid->boundtype[0] = 1;
  grid->boundtype[1] = 2;
  grid->boundtype[2] = 3;
  grid->boundtype[3] = 4;

  grid->boundsolid[0] = 1;
  grid->boundsolid[1] = 1;
  grid->boundsolid[2] = 1;
  grid->boundsolid[3] = 1;

  if(info) printf("A simple 3D example mesh was created\n");
}


void SetElementDivision(struct GridType *grid,Real relh,int info)
/* Given the densities and ratios in each cell finds the 
   optimum way to devide the mesh into elements. 
   The procedure is the following:
   For each subcell set the minimum number of elements 
   then add one element at a time till the number of 
   elements is the desired one. The added element
   is always set to the subcell having the sparsest mesh.
   Also numbers the cells taking into consideration only 
   materials that have indeces in interval [firstmat,lastmat]. 
   */
{
  int i,j,nx,ny,nxmax = 0,nymax = 0;
  int sumxelems,sumyelems,sumxyelems;
  int wantedelems,wantedelemsx,wantedelemsy;
  Real ratio,linearlimit;
  Real dxmax = 0,dymax = 0,dx = 0,dy = 0,dxlimit = 0;

  if(0) printf("SetElementDivision\n");

  if(grid->dimension == 1) 
    grid->numbering = NUMBER_1D;

  nx = grid->xcells;
  ny = grid->ycells;
  linearlimit = 0.001;

  /* Lets number the cells from left to right and up to down. */
  grid->nocells = 0;
  for(j=1;j<=ny;j++)
    for(i=1;i<=nx;i++) {
      if (grid->structure[j][i] >= grid->firstmaterial  &&  
	  grid->structure[j][i] <= grid->lastmaterial)   
        grid->numbered[j][i] = ++grid->nocells; 
    }
  if(0) printf("The mesh is devided into %d separate subcells.\n",grid->nocells);
  
  /* Put the linearity flags. */
  for(i=1; i<= nx ;i++) {
    if (fabs(1.-grid->xexpand[i]) < linearlimit) 
      grid->xlinear[i] = TRUE; 
    else if (fabs(1.+grid->xexpand[i]) < linearlimit) 
      grid->xlinear[i] = TRUE; 
    else 
      grid->xlinear[i] = FALSE;
  }

  for(i=1; i <= ny ;i++) {
    if(fabs(1.-grid->yexpand[i]) < linearlimit) 
      grid->ylinear[i] = TRUE;
    else if(fabs(1.+grid->yexpand[i]) < linearlimit) 
      grid->ylinear[i] = TRUE;
    else 
      grid->ylinear[i] = FALSE;
  }


 /* If there are no materials no elements either.
     Otherwise at least grid->minelems elments 
     If you want to number elements even if they are 
     not there change this initialization to minelems. */
  if(grid->autoratio) {
    for(i=1;i<=nx;i++)  
      grid->xelems[i] = grid->minxelems;
    for(i=1;i<=ny;i++)  
      grid->yelems[i] = grid->minyelems;
  }    
  else {
    for(i=1;i<=nx;i++)  
      if(grid->xelems[i] < grid->minxelems) grid->xelems[i] = grid->minxelems;
    for(i=1;i<=ny;i++)  
      if(grid->yelems[i] < grid->minyelems) grid->yelems[i] = grid->minyelems;
  }

  sumxelems = 0;
  for(i=1;i<=nx;i++) {
    if(grid->dimension == 1 && !grid->numbered[1][i]) continue;
    sumxelems += grid->xelems[i];
  }
  sumyelems = 0;
  for(i=1;i<=ny;i++)  
    sumyelems += grid->yelems[i];

  if(grid->dimension == 1) {
    grid->autoratio = 2;
    grid->totxelems = grid->wantedelems;
    grid->totyelems = 1;
  }

  /* Allocate elements for both axis separately */
  if(grid->autoratio == 2) {

    wantedelemsx = (int) (1.0*grid->totxelems / relh);
    wantedelemsy = (int) (1.0*grid->totyelems / relh);

    if(sumxelems > wantedelemsx) {
      printf("SetElementDivision: %d is too few elements in x-direction\n",grid->totxelems);
      wantedelemsx = sumxelems+1;
    }      
    if(sumyelems > wantedelemsy ) {
      printf("SetElementDivision: %d is too few elements in y-direction\n",grid->totyelems);
      wantedelemsy = sumyelems+1;
    }   
    
    for(;sumxelems < wantedelemsx;) {
      dxmax = 0.0;
      for(i=1;i<=nx;i++) {
	if(grid->xelems[i] == 0) continue;
	
	/* Don't put elements to passive subcells */
	if(grid->dimension == 1 && !grid->numbered[1][i]) continue; 
	
	if(grid->xlinear[i] == TRUE || grid->xelems[i]==1) 
	  dx = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
	else {
	  if(grid->xexpand[i] > 0.0) {
	    ratio = pow(grid->xexpand[i],1./(grid->xelems[i]-1.));
	    dx = (grid->x[i] - grid->x[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i])));
	    if(ratio < 1.)   
	      dx *= grid->xexpand[i];
	  }
	  else {	    
	    if(grid->xelems[i]==2) {
	      dx = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
	    } 
	    else if(grid->xelems[i]%2 == 0) {
	      ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2-1.));
	      dx = 0.5 * (grid->x[i] - grid->x[i-1]) * 
		(1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i]/2)));
	    }
	    else if(grid->xelems[i]%2 == 1) {
	      ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2));
	      dx = (grid->x[i] - grid->x[i-1]) / 
		(2.0*(1.-pow(ratio,(Real)(grid->xelems[i]/2)))/
		 (1-ratio) + pow(ratio,(Real)(grid->xelems[i]/2+0.5)));
	    }
	  }
	}
	dx *= grid->xdens[i];
	if(dx > dxmax) {
	  dxmax = dx;
	  nxmax = i;
	}
      }      
      grid->xelems[nxmax] += 1;
      sumxelems++;
    }
  

    if(grid->dimension > 1) {
      for(;sumyelems < wantedelemsy;) {
	dymax = 0.0;
	for(i=1;i<=ny;i++) {
	  if(grid->yelems[i] == 0) continue;
	  if(grid->ylinear[i] || grid->yelems[i]==1) 
	    dy = (grid->y[i] - grid->y[i-1])/grid->yelems[i];
	  else {
	    ratio = pow(grid->yexpand[i],1./(grid->yelems[i]-1));
	    dy = (grid->y[i] - grid->y[i-1]) * 
	      (1.-ratio)/(1.-pow(ratio,(Real)(grid->yelems[i])));
	    if(ratio < 1.)   
	      dy *= grid->yexpand[i];
	  }
	  dy *= grid->ydens[i];
	  if(dy > dymax) {
	    dymax = dy;
	    nymax = i;
	  }
	}      
	grid->yelems[nymax] += 1;
	sumyelems++;
      }
    }
  }   


  /* Both axis dependently */
  if(grid->autoratio == 1)  {
    
    wantedelems = (int) (1.0*grid->wantedelems / (relh*relh));

    sumxyelems = 0;
    for(;;) {
      dxmax = 0.0;

      for(i=1;i<=nx;i++) {
	
	if(grid->xelems[i] == 0) continue;
	if(grid->xlinear[i] || grid->xelems[i]==1) 
	  dx = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
	else {
	  if(grid->xexpand[i] > 0.0) {
	    ratio = pow(grid->xexpand[i],1./(grid->xelems[i]-1.));
	    dx = (grid->x[i] - grid->x[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i])));
	    if(ratio < 1.)   
	      dx *= grid->xexpand[i];
	  }
	  else if(grid->xelems[i]==2) {
	    dx = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
	  } 
	  else if(grid->xelems[i]%2 == 0) {
	    ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2-1.));
	    dx = 0.5 * (grid->x[i] - grid->x[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i]/2)));
	  }
	  else if(grid->xelems[i]%2 == 1) {
	    ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2));
	    dx = (grid->x[i] - grid->x[i-1]) / 
	      (2.0*(1.-pow(ratio,(Real)(grid->xelems[i]/2)))/
	       (1-ratio) + pow(ratio,(Real)(grid->xelems[i]/2+0.5)));
	  }
	}

	dx *= grid->xdens[i];
	if(dx > dxmax) {
	  dxmax = dx;
	  nxmax = i;
	}
      }      


      dymax = 0.0;
      for(i=1;i<=ny;i++) {

	if(grid->yelems[i] == 0) continue;
	if(grid->ylinear[i] || grid->yelems[i]==1) 
	  dy = (grid->y[i] - grid->y[i-1])/grid->yelems[i];
	else {
	  if(grid->yexpand[i] > 0.0) {
	    ratio = pow(grid->yexpand[i],1./(grid->yelems[i]-1));
	    dy = (grid->y[i] - grid->y[i-1]) * 
	      (1.-ratio)/(1.-pow(ratio,(Real)(grid->yelems[i])));
	    if(ratio < 1.)   
	      dy *= grid->yexpand[i];
	  }
	  else if(grid->yelems[i]==2) {
	    dy = (grid->y[i] - grid->y[i-1])/grid->yelems[i];
	  } 
	  else if(grid->yelems[i]%2 == 0) {
	    ratio = pow(-grid->yexpand[i],1./(grid->yelems[i]/2-1.));
	    dy = 0.5 * (grid->y[i] - grid->y[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->yelems[i]/2)));
	  }
	  else if(grid->yelems[i]%2 == 1) {
	    ratio = pow(-grid->yexpand[i],1./(grid->yelems[i]/2));
	    dy = (grid->y[i] - grid->y[i-1]) / 
	      (2.0*(1.-pow(ratio,(Real)(grid->yelems[i]/2)))/
	       (1-ratio) + pow(ratio,(Real)(grid->yelems[i]/2+0.5)));
	  }
	}
	dy *= grid->ydens[i];
	if(dy > dymax) {
	  dymax = dy;
	  nymax = i;
	}
      }      
      dymax /= grid->xyratio;

      if(dxmax > dymax) {
	grid->xelems[nxmax] += 1;
	sumxelems++;
	dxlimit = dxmax;
      }
      else {
	grid->yelems[nymax] += 1;
	sumyelems++;
	dxlimit = dymax;
      }
      
      sumxyelems = 0;
      for(j=1;j<=grid->ycells;j++)
	for(i=1;i<=grid->xcells;i++) 
	  if(grid->numbered[j][i]) sumxyelems += grid->xelems[i] * grid->yelems[j];

      if(wantedelems <= sumxyelems) break;
    }
  }    



  if(grid->autoratio == 3 || grid->limitdxverify)  {
    
    if(grid->autoratio == 3) {
      dxlimit = relh * grid->limitdx;
      dxmax = dymax = dxlimit;
    }

    for(i=1;i<=nx;i++) {

      for(;;) {       
	if(grid->xlinear[i] || grid->xelems[i]==0) 
	  dx = (grid->x[i] - grid->x[i-1])/(grid->xelems[i]+1);
	else {
	  if(grid->xexpand[i] > 0.0) {
	    ratio = pow(grid->xexpand[i],1./grid->xelems[i]);
	    dx = (grid->x[i] - grid->x[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i]+1)));
	    if(ratio < 1.)   
	      dx *= grid->xexpand[i];
	  }
	  else if(grid->xelems[i]==1) {
	    dx = (grid->x[i] - grid->x[i-1])/(grid->xelems[i]+1);
	  } 
	  else if((grid->xelems[i]+1)%2 == 0) {
	    ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2));
	    dx = 0.5 * (grid->x[i] - grid->x[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i]/2+1)));
	  }
	  else if((grid->xelems[i]+1)%2 == 1) {
	    ratio = pow(-grid->xexpand[i],1./((grid->xelems[i]+1)/2));
	    dx = (grid->x[i] - grid->x[i-1]) / 
	      (2.0*(1.-pow(ratio,(Real)((grid->xelems[i]+1)/2)))/
	       (1-ratio) + pow(ratio,(Real)((grid->xelems[i]+1)/2+0.5)));
	  }
	}
	dx *= grid->xdens[i];
	
	/* choose the best fit for desired density */
	if(fabs(dx-dxlimit) < fabs( dx*(1+1.0/grid->xelems[i]) -dxlimit) )
	  grid->xelems[i] += 1;
	else
	  break;
      }
      sumxelems += grid->xelems[i];
    }      


    for(i=1;i<=ny;i++) {
      
      for(;;) {
	if(grid->ylinear[i] || grid->yelems[i]==0) 
	  dy = (grid->y[i] - grid->y[i-1])/(grid->yelems[i]+1);
	else {
	  if(grid->yexpand[i] > 0.0) {
	    ratio = pow(grid->yexpand[i],1./grid->yelems[i]);
	    dy = (grid->y[i] - grid->y[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->yelems[i]+1)));
	    if(ratio < 1.)   
	      dy *= grid->yexpand[i];
	  }
	  else if(grid->yelems[i]==1) {
	    dy = (grid->y[i] - grid->y[i-1])/(grid->yelems[i]+1);
	  } 
	  else if((grid->yelems[i]+1)%2 == 0) {
	    ratio = pow(-grid->yexpand[i],1./(grid->yelems[i]/2));
	    dy = 0.5 * (grid->y[i] - grid->y[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->yelems[i]/2+1)));
	  }
	  else if((grid->yelems[i]+1)%2 == 1) {
	    ratio = pow(-grid->yexpand[i],1./((grid->yelems[i]+1)/2));
	    dy = (grid->y[i] - grid->y[i-1]) / 
	      (2.0*(1.-pow(ratio,(Real)((grid->yelems[i]+1)/2)))/
	       (1-ratio) + pow(ratio,(Real)((grid->yelems[i]+1)/2+0.5)));
	  }
	}
	
	dy *= grid->ydens[i] / grid->xyratio;
	/* choose the best fit for desired density */
	if(fabs(dy-dxlimit) < fabs( dy*(1+1.0/grid->yelems[i]) -dxlimit) )
	  grid->yelems[i] += 1;
	else
	  break; 
      }
      sumyelems += grid->yelems[i];
    }      
    
    sumxyelems = 0;
    for(j=1;j<=grid->ycells;j++)
      for(i=1;i<=grid->xcells;i++) 
	if(grid->numbered[j][i]) sumxyelems += grid->xelems[i] * grid->yelems[j];
  }


  /* Put the linearity flags if there is only one division. */
  for(i=1; i<= nx ;i++) 
    if (grid->xelems[i] == 1) 
      grid->xlinear[i] = TRUE; 
  for(i=1; i <= ny ;i++) 
    if(grid->yelems[i] == 1) 
      grid->ylinear[i] = TRUE;
 

  /* Set the size of the first element within each subcell */
  for(i=1;i<=nx;i++) {

    if(grid->xlinear[i] == TRUE) 
      grid->dx[i] = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
    else {
      if(grid->xexpand[i] > 0.0) {
	ratio = pow(grid->xexpand[i],1./(grid->xelems[i]-1.));
	grid->xratios[i] = ratio;
	grid->dx[i] = (grid->x[i] - grid->x[i-1]) * 
	  (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i])));
      }
      else if(grid->xelems[i] == 2) {
	grid->dx[i] = (grid->x[i] - grid->x[i-1])/grid->xelems[i];
      }
      else if(grid->xelems[i]%2 == 0) {
	ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2-1.));
	grid->xratios[i] = ratio;
	grid->dx[i] = 0.5 * (grid->x[i] - grid->x[i-1]) * 
	  (1.-ratio) / (1.-pow(ratio,(Real)(grid->xelems[i]/2)));
      }
      else if(grid->xelems[i]%2 == 1) {
	ratio = pow(-grid->xexpand[i],1./(grid->xelems[i]/2));
	grid->xratios[i] = ratio;
	grid->dx[i] = (grid->x[i] - grid->x[i-1]) / 
	  (2.0*(1.-pow(ratio,(Real)(grid->xelems[i]/2)))/
	   (1-ratio) + pow(ratio,(Real)(grid->xelems[i]/2+0.5)));
      }
    }	
  }

  for(i=1;i<=ny;i++) {

    if(grid->ylinear[i] == TRUE) 
      grid->dy[i] = (grid->y[i] - grid->y[i-1])/grid->yelems[i];
    else {
      if(grid->yexpand[i] > 0.0) {
      ratio = pow(grid->yexpand[i],1./(grid->yelems[i]-1.));
      grid->yratios[i] = ratio;
      grid->dy[i] = (grid->y[i] - grid->y[i-1]) * 
	(1.-ratio)/(1.-pow(ratio,(Real)(grid->yelems[i])));
      }
      else if(grid->yelems[i] == 2) {
	grid->dy[i] = (grid->y[i] - grid->y[i-1])/grid->yelems[i];
      }
      else if(grid->yelems[i]%2 == 0) {
	ratio = pow(-grid->yexpand[i],1./(grid->yelems[i]/2-1.));
	grid->yratios[i] = ratio;
	grid->dy[i] = 0.5 * (grid->y[i] - grid->y[i-1]) * 
	  (1.-ratio) / (1.-pow(ratio,(Real)(grid->yelems[i]/2)));
      }
      else if(grid->yelems[i]%2 == 1) {
	ratio = pow(-grid->yexpand[i],1./(grid->yelems[i]/2));
	grid->yratios[i] = ratio;
	grid->dy[i] = (grid->y[i] - grid->y[i-1]) / 
	  (2.0*(1.-pow(ratio,(Real)(grid->yelems[i]/2)))/
	   (1-ratio) + pow(ratio,(Real)(grid->yelems[i]/2+0.5)));
      }
    }
  }

  /* Calculate the total number of elements */
  sumxyelems = 0;
  for(j=1;j<=grid->ycells;j++)
    for(i=1;i<=grid->xcells;i++) 
      if(grid->numbered[j][i]) sumxyelems += grid->xelems[i] * grid->yelems[j];

  grid->noelements = sumxyelems;

  if(0) printf("Created a total of %d elements\n",sumxyelems);

  grid->dx0 = dxmax;
  grid->dy0 = dymax;
}



void SetCellData(struct GridType *grid,struct CellType *cell,int info)
/* Sets the data that can directly be derived from type GridType. 
 */
{
  int i,j,cnew=0;

  for(j=1;j<= grid->ycells ;j++)                   /* cells direction up    */
    for(i=1;i<= grid->xcells; i++)                 /* cells direction right */      
      if( cnew = grid->numbered[j][i] ) {          /* if cell is occupied   */

        /* Initialize the numbering to zero  */
        cell[cnew].left1st = 0;
        cell[cnew].left2nd = 0;
        cell[cnew].leftlast = 0;
        cell[cnew].levelwidth = 0;
        cell[cnew].levelwidthcenter = 0;
	cell[cnew].leftcenter = 0;
	cell[cnew].elemwidth = 0;
	cell[cnew].elem1st = 0;
	
	/* Set the element type */
	cell[cnew].nonodes = grid->nonodes;
	cell[cnew].numbering = grid->numbering;
	
	/* Set the cell coordinates */
        cell[cnew].xcorner[TOPLEFT] = cell[cnew].xcorner[BOTLEFT] = grid->x[i-1];
	cell[cnew].xcorner[TOPRIGHT] = cell[cnew].xcorner[BOTRIGHT] = grid->x[i];
        cell[cnew].ycorner[BOTRIGHT] = cell[cnew].ycorner[BOTLEFT] = grid->y[j-1];
        cell[cnew].ycorner[TOPRIGHT] = cell[cnew].ycorner[TOPLEFT] = grid->y[j];
	
	/* Set the cell width in both directions */	
	cell[cnew].xwidth = grid->x[i] - grid->x[i-1];
	cell[cnew].ywidth = grid->y[j] - grid->y[j-1];
	
	/* Set the number of elements in a subcell */
	cell[cnew].xelem = grid->xelems[i];
	cell[cnew].yelem = grid->yelems[j];
	
	/* Set the the ratio of elements when going left to right and down to up */
	cell[cnew].xratio = grid->xratios[i];
	cell[cnew].yratio = grid->yratios[j];
	cell[cnew].xlinear = grid->xlinear[i];
	cell[cnew].ylinear = grid->ylinear[j];
	cell[cnew].xlinear = grid->xlinear[i];
	cell[cnew].ylinear = grid->ylinear[j];
	if(grid->xexpand[i] < 0.0 && grid->xlinear[i] == FALSE) {
	  if(grid->xelems[i] == 2) cell[cnew].xlinear = 1;
	  else cell[cnew].xlinear = 2;
	}
	if(grid->yexpand[j] < 0.0 && grid->ylinear[j] == FALSE) {
	  if(grid->yelems[j] == 2) cell[cnew].ylinear = 1;
	  else cell[cnew].ylinear = 2;
	}
	
	/* Set the length of the first element in both directions  */
	cell[cnew].dx1 = grid->dx[i];
	cell[cnew].dy1 = grid->dy[j];
	
	/* Set the material in question */
	cell[cnew].material = grid->structure[j][i];
	
	/* Set the boundary types for sides and corners */
	cell[cnew].boundary[LEFT] = grid->structure[j][i-1];
	cell[cnew].boundary[DOWN] = grid->structure[j-1][i];
	cell[cnew].boundary[RIGHT]= grid->structure[j][i+1];  
	cell[cnew].boundary[UP]   = grid->structure[j+1][i];
	cell[cnew].boundary[4] = grid->structure[j-1][i-1];
	cell[cnew].boundary[5] = grid->structure[j-1][i+1];
	cell[cnew].boundary[6] = grid->structure[j+1][i+1];
	cell[cnew].boundary[7] = grid->structure[j+1][i-1];

	/* Set the neighbour cell indices for sides and corners */ 
	cell[cnew].neighbour[LEFT] = grid->numbered[j][i-1];
	cell[cnew].neighbour[DOWN] = grid->numbered[j-1][i];
	cell[cnew].neighbour[RIGHT]= grid->numbered[j][i+1];  
	cell[cnew].neighbour[UP]   = grid->numbered[j+1][i];
	cell[cnew].neighbour[4] = grid->numbered[j-1][i-1];
	cell[cnew].neighbour[5] = grid->numbered[j-1][i+1];
	cell[cnew].neighbour[6] = grid->numbered[j+1][i+1];
	cell[cnew].neighbour[7] = grid->numbered[j+1][i-1];
      }

  if(info) printf("%d cells were created.\n",grid->nocells);
}



int SetCellKnots(struct GridType *grid, struct CellType *cell,int info)
/* Uses given mesh to number the knots present in the cells.
   The knots are numbered independently of the cells from left 
   to right and up to down. Only the numbers of four knots at the 
   left side of each cell are saved for later use. The number of 
   each knot can later be easily recalled using simple functions. 
   The subroutine was initially written for ordering the knots
   from left to right and down to up. However, this does not always
   produce reasonably small bandwidth and therefore a numbering 
   scheme for the other order was later added. Therefore the algorithm 
   may not be that clear for this other scheme.
   Numbers the knots in rectangular elements. There may be 
   4, 5, 8, 9, 12 or 16 nodes in each elements.
   */
{
  int i,j,level,center;
  int degree,centernodes,sidenodes,nonodes;
  int cnew=0,cup=0,cleft=0,cleftup=0;
  int elemno,knotno;
  int maxwidth,width,numbering;
  int xcells,ycells,*yelems=NULL,*xelems=NULL;

  numbering = grid->numbering;
  nonodes = grid->nonodes;
  knotno  = 0;
  elemno  = 0;

  if(numbering == NUMBER_XY) {
    xcells = grid->xcells;
    ycells = grid->ycells;
    xelems = grid->xelems;
    yelems = grid->yelems;
  }
  else if(numbering == NUMBER_YX) {
    xcells = grid->ycells;
    ycells = grid->xcells;
    xelems = grid->yelems;
    yelems = grid->xelems;
  }
  else { 
    printf("No %d numbering scheme exists!\n",numbering); 
    return(1);
  }

  switch (nonodes) {
  case 4:
    centernodes = FALSE;
    sidenodes = FALSE;
    degree = 1;
    break;
  case 5:
    centernodes = TRUE;
    sidenodes = FALSE;
    degree = 2;
    break;
  case 8:
    centernodes = FALSE;
    sidenodes = TRUE;
    degree = 2;
    break;
  case 9:
    centernodes = TRUE;
    sidenodes = TRUE;
    degree = 2;
    break;
  case 12:
    centernodes = FALSE;
    sidenodes = TRUE;
    degree = 3;
    break;
  case 16:
    centernodes = TRUE;
    sidenodes = TRUE;
    degree = 3;
    break;
  default:
    printf("CreateCells: No numbering scheme for %d-node elements.\n",
	   grid->nonodes);
    return(2);
  }

  for(j=1;j<= ycells ;j++)                     /* cells direction up     */
    for(level=0; level <= yelems[j] ;level++)  /* lines inside cells     */
      for(center=0; center < degree; center++) 
      for(i=1;i<= xcells; i++)                 /* cells direction right  */
	{        
	  if(numbering == NUMBER_XY) {	  
	    cnew = grid->numbered[j][i];
	    cleft= grid->numbered[j][i-1];
	    cup  = grid->numbered[j+1][i];
	    cleftup = grid->numbered[j+1][i-1];
	    if(cnew) {
	      cell[cnew].xind = i;
	      cell[cnew].yind = j;	      
	    }
	  }	  
	  else if(numbering == NUMBER_YX) {
	    cnew = grid->numbered[i][j];
	    cleft= grid->numbered[i-1][j];
	    cup  = grid->numbered[i][j+1];
	    cleftup = grid->numbered[i-1][j+1];
	    if(cnew) {
	      cell[cnew].xind = j;
	      cell[cnew].yind = i;	      
	    }
	  }
	  
	  if(center == 0) {
	    /* the first line of an occupied cell is unnumbered, 
	       this happens only in the bottom */
	    if (cnew && level==0 && !cell[cnew].left1st) { 
	      if(!cleft) knotno++;
	      cell[cnew].left1st = knotno;
	      if(sidenodes) 
		knotno += degree * xelems[i];
	      else
		knotno += xelems[i];
	    } /* level=0 */
	    
	    /* the n:th line of an occupied cell */
	    else if (cnew && level > 0 && level < yelems[j]) {   
	      if(!cleft) knotno++;
	      if(level==1) 
		cell[cnew].left2nd = knotno;
	      if(level==2) 
		cell[cnew].levelwidth = knotno - cell[cnew].left2nd; 	      
	      if(sidenodes) 
		knotno += degree * xelems[i];
	      else 
		knotno += xelems[i];
	    } /* 1 <= level < n */	  
	  
	    /* last line of this cell or first line of upper cell */
	    else if ((cnew || cup) && level == yelems[j]) { 
	      if(!cleft && !cleftup) 
		knotno++;
	      if(cnew)
		cell[cnew].leftlast = knotno;
	      if(level==2) 
		cell[cnew].levelwidth = knotno - cell[cnew].left2nd; 	      
	      if(yelems[j] == 1) 
		cell[cnew].left2nd = cell[cnew].leftlast;
	      if(cup)
		cell[cup].left1st = knotno;
	      if(sidenodes) 
		knotno += degree * xelems[i];
	      else 
		knotno += xelems[i];
	    } /* level=n */

	    /* Number the elements */
	    if(cnew && level > 0) {
	      if(level==1)  
		cell[cnew].elem1st = elemno+1;	      
	      if(level==2) 
		cell[cnew].elemwidth  = (elemno+1) - cell[cnew].elem1st;
	      elemno += xelems[i];	    
	    }
	  }

	  if(center && level < yelems[j]) {
	    if (cnew) { 
	      if(!cleft && sidenodes) 
		knotno++;
	      if(level==0 && center==1) 
		cell[cnew].leftcenter = knotno;
	      if(level==0 && center==2)
		cell[cnew].left2center = knotno;		
	      if(level==1 && center==1)
		cell[cnew].levelwidthcenter = knotno - cell[cnew].leftcenter;
	      if(centernodes && sidenodes)
		knotno += degree * xelems[i];
	      else 
		knotno += xelems[i];
	    } 
	  }
	
	} /* x-cell loop */
  

  maxwidth = 0;
  for(j=1;j<= ycells ;j++) 
    for(i=1;i<= xcells; i++) {
      if(numbering == NUMBER_XY)
	cnew = grid->numbered[j][i];
      else if(numbering == NUMBER_YX)
	cnew = grid->numbered[i][j];      
      if(cnew) {
	width = cell[cnew].left2nd - cell[cnew].left1st;
	if(width > maxwidth) 
	  maxwidth = width;
	width = cell[cnew].levelwidth;
	if(width > maxwidth) 
	  maxwidth = width;
	width = cell[cnew].leftlast-cell[cnew].left2nd
	  -(yelems[j]-2)*cell[cnew].levelwidth;
	if(width > maxwidth) 
	  maxwidth = width;
      }  
    }
  maxwidth += 2;

  grid->maxwidth = maxwidth;
  grid->noknots = knotno;
  grid->noelements = elemno;

  if(info) {
    printf("Numbered %d knots in %d %d-node elements.\n",knotno,elemno,nonodes);
    if(numbering == NUMBER_XY) 
      printf("Numbering order was <x><y> and max levelwidth %d.\n",
	     maxwidth);
    else if(numbering == NUMBER_YX) 
      printf("Numbering order was <y><x> and max levelwidth %d.\n",
	     maxwidth);
  }

  return(0);
}



int SetCellKnots1D(struct GridType *grid, struct CellType *cell,int info)
{
  int i;
  int degree,nonodes;
  int cnew,cleft;
  int elemno,knotno;
  int maxwidth;
  int xcells,*xelems;

  nonodes = grid->nonodes;
  knotno  = 0;
  elemno  = 0;

  xcells = grid->xcells;
  xelems = grid->xelems;

  switch (nonodes) {
  case 2:
    degree = 1;
    break;
  case 3:
    degree = 2;
    break;
  case 4:
    degree = 3;
    break;

  default:
    printf("CreateCells: No numbering scheme for %d-node elements.\n",
	   grid->nonodes);
    return(2);
  }

  for(i=1;i<= xcells; i++) {        
    
    cnew = grid->numbered[1][i];
    cleft= grid->numbered[1][i-1];
    if(cnew) cell[cnew].xind = i;
    
    if (cnew) { 
      /* Number the nodes */
      if(!cleft) knotno++;
      cell[cnew].left1st = knotno;
      knotno += degree * xelems[i];
      
      /* Number the elements */
      cell[cnew].elem1st = elemno+1;	      
      elemno += xelems[i];	    
    }
  } /* x-cell loop */
  
  maxwidth = degree;
  
  grid->maxwidth = maxwidth;
  grid->noknots = knotno;
  grid->noelements = elemno;
  
  if(info) {
    printf("Numbered %d knots in %d %d-node elements.\n",knotno,elemno,nonodes);
  }

  return(0);
}



void CreateCells(struct GridType *grid,struct CellType **cell,int info)
{
  (*cell) = (struct CellType*)
    malloc((size_t) (grid->nocells+1)*sizeof(struct CellType)); 

  SetCellData(grid,*cell,info);

  if(grid->dimension == 1) 
    SetCellKnots1D(grid,*cell,info);
  else
    SetCellKnots(grid,*cell,info);
}


void DestroyCells(struct CellType **cell)
{
  free(cell);
}



int GetKnotIndex(struct CellType *cell,int i,int j)
/* Given the cell and knot indices gives the corresponding 
   global knot index. The indices are expected to be in the 
   range [0..n] and [0..m]. Requires only the structure CellType. 
   */
{
  int ind,aid,maxj;

  if(cell->numbering == NUMBER_1D) {
    ind = cell->left1st;
    if(cell->nonodes == 2) 
      ind += i;
    else if(cell->nonodes == 3) 
      ind += 2*i;
    else if(cell->nonodes == 4) 
      ind += 3*i;
    return(ind);
  }

  if(cell->numbering == NUMBER_XY) 
    maxj = cell->yelem;
  else if(cell->numbering == NUMBER_YX) {
    aid = j; j = i; i = aid;
    maxj = cell->xelem;
  }
  else {
    maxj = 0;
    bigerror("GetKnotIndex: Unknown numbering scheme!");
  }

  if(j == 0) 
    ind = cell->left1st;
  else if (j == maxj)
    ind = cell->leftlast;
  else
    ind = cell->left2nd + (j-1) * cell->levelwidth;
  
  if(cell->nonodes == 4) 
    ind += i;
  else if(cell->nonodes == 5)
    ind += i;
  else if(cell->nonodes == 8 || cell->nonodes == 9) 
    ind += 2*i;
  else if(cell->nonodes == 12 || cell->nonodes == 16) 
    ind += 3*i;

  return(ind);
}


int GetKnotCoordinate(struct CellType *cell,int i,int j,Real *x,Real *y)
/* Given the cell and element indices inside the cell gives the 
   corresponding global knot numbers. The indices are expected 
   to be in the range [0..n] and [0..m]. Requires only the 
   structure CellType.   
   */
{
  int ind;

  if(cell->xlinear == 1)
    (*x) = cell->xcorner[BOTLEFT] + cell->dx1 * i;
  else if(cell->xlinear == 0)
    (*x) = cell->xcorner[BOTLEFT] + cell->dx1 * 
    (1.- pow(cell->xratio,(Real)(i))) / (1.-cell->xratio);
  else if(cell->xlinear == 2) {
    if(i<=cell->xelem/2) {
      (*x) = cell->xcorner[BOTLEFT] + cell->dx1 * 
	(1.- pow(cell->xratio,(Real)(i))) / (1.-cell->xratio);
    }
    else {
      (*x) = cell->xcorner[BOTRIGHT] - cell->dx1 * 
	(1.- pow(cell->xratio,(Real)(cell->xelem-i))) / (1.-cell->xratio);
    }
  }

  if(cell->ylinear == 1)
    (*y) = cell->ycorner[BOTLEFT] + cell->dy1 * j;
  else if(cell->ylinear == 0) 
    (*y) = cell->ycorner[BOTLEFT] + cell->dy1 * 
           (1.- pow(cell->yratio,(Real)(j))) / (1.-cell->yratio);
  else if(cell->ylinear == 2) {
    if(j<=cell->yelem/2) {
      (*y) = cell->ycorner[BOTLEFT] + cell->dy1 * 
	(1.- pow(cell->yratio,(Real)(j))) / (1.-cell->yratio);
    }
    else {
      (*y) = cell->ycorner[TOPLEFT] - cell->dy1 * 
	(1.- pow(cell->yratio,(Real)(cell->yelem-j))) / (1.-cell->yratio);
    }
  }

  ind = GetKnotIndex(cell,i,j);

  return(ind);
}



int GetElementIndices(struct CellType *cell,int i,int j,int *ind)
/* For given element gives the coordinates and index for each knot.
   The indices i and j are expected to range from [1..n] and [1..m]. 
   requires only the structure CellType.
   */
{
  int nonodes,numbering,elemind=0;
  
  nonodes = cell->nonodes;
  numbering = cell->numbering;

  if(numbering != NUMBER_1D) {
    ind[TOPRIGHT] = GetKnotIndex(cell,i,j);
    ind[BOTRIGHT] = GetKnotIndex(cell,i,j-1);
    ind[TOPLEFT]  = GetKnotIndex(cell,i-1,j);
    ind[BOTLEFT]  = GetKnotIndex(cell,i-1,j-1);
  }    

  if(numbering == NUMBER_XY) {
    elemind = cell->elem1st+(i-1) + (j-1)*cell->elemwidth;
    if(nonodes == 4) return(elemind);
        
    if(nonodes == 5) {
      ind[4] = cell->leftcenter + (j-1) * cell->levelwidthcenter + i;
    }
    else if(nonodes == 8) {
      ind[4] = ind[0]+1;
      ind[6] = ind[3]+1;
      ind[5] = cell->leftcenter + (j-1) * cell->levelwidthcenter + i;
      ind[7] = ind[5]-1;
    }
    else if(nonodes == 9) {
      ind[4] = ind[0]+1;
      ind[6] = ind[3]+1;
      ind[5] = cell->leftcenter + (j-1) * cell->levelwidthcenter + 2*i;
      ind[7] = ind[5]-2;
      ind[8] = ind[5]-1;
    }
    else if(nonodes == 12) {
      ind[4]  = ind[0]+1;
      ind[5]  = ind[0]+2;
      ind[9]  = ind[3]+1;
      ind[8]  = ind[3]+2;
      ind[6]  = cell->leftcenter + (j-1) * cell->levelwidthcenter + i;
      ind[11] = ind[6]-1;
      ind[7]  = cell->left2center + (j-1) * cell->levelwidthcenter + i;
      ind[10] = ind[7]-1;
    }
    else if(nonodes == 16) {
      ind[4]  = ind[0]+1;
      ind[5]  = ind[0]+2;
      ind[9]  = ind[3]+1;
      ind[8]  = ind[3]+2;
      ind[6]  = cell->leftcenter + (j-1) * cell->levelwidthcenter + 3*i;
      ind[11] = ind[6]-3;
      ind[7]  = cell->left2center + (j-1) * cell->levelwidthcenter + 3*i;
      ind[10] = ind[7]-3;
      ind[13] = ind[6]-1;
      ind[12] = ind[6]-2;
      ind[14] = ind[7]-1;
      ind[15] = ind[7]-2;
    }
    else 
      printf("GetElementIndices: not implemented for %d nodes.\n",nonodes);    
  }

  else if(numbering == NUMBER_YX) { 
    elemind = cell->elem1st+(j-1) + (i-1)*cell->elemwidth;
    
    if(nonodes == 4) return(elemind);

    if(nonodes == 5) {
      ind[4] = cell->leftcenter + (i-1) * cell->levelwidthcenter + j;
    }
    else if (nonodes==8) {
      ind[7] = ind[0]+1;
      ind[5] = ind[1]+1;
      ind[6] = cell->leftcenter + (i-1) * cell->levelwidthcenter + j;
      ind[4] = ind[6]-1;
    }
    else if(nonodes == 9) {
      ind[7] = ind[0]+1;
      ind[5] = ind[1]+1;
      ind[6] = cell->leftcenter + (i-1) * cell->levelwidthcenter + 2*j;
      ind[4] = ind[6]-2;
      ind[8] = ind[6]-1;
    }
    else if(nonodes == 12) {
      ind[11]= ind[0]+1;
      ind[10]= ind[0]+2;
      ind[6] = ind[1]+1;
      ind[7] = ind[1]+2;
      ind[9] = cell->leftcenter + (i-1) * cell->levelwidthcenter + j;
      ind[4] = ind[9]-1;
      ind[8] = cell->left2center + (i-1) * cell->levelwidthcenter + j;
      ind[5] = ind[8]-1;
    }
    else if(nonodes == 16) {
      ind[11]= ind[0]+1;
      ind[10]= ind[0]+2;
      ind[6] = ind[1]+1;
      ind[7] = ind[1]+2;
      ind[9] = cell->leftcenter + (i-1) * cell->levelwidthcenter + 3*j;
      ind[4] = ind[9]-3;
      ind[15]= ind[9]-1;
      ind[12]= ind[9]-2;
      ind[8] = cell->left2center + (i-1) * cell->levelwidthcenter + 3*j;
      ind[5] = ind[8]-3;
      ind[14]= ind[8]-1;
      ind[13]= ind[8]-2;
    }
    else 
      printf("GetElementIndices: not implemented for %d nodes.\n",nonodes);
  }

  else if(numbering == NUMBER_1D) {
    elemind = cell->elem1st+(i-1);
    ind[0] = GetKnotIndex(cell,i-1,1);
    if(nonodes == 2) {
      ind[1] = ind[0] + 1;
    }
    else if(nonodes == 3) {
      ind[2] = ind[0] + 1;
      ind[1] = ind[0] + 2;
    }
    else if(nonodes == 4) {
      ind[2] = ind[0] + 1;
      ind[3] = ind[0] + 2;
      ind[1] = ind[0] + 3;
    }
  }  
  return(elemind);
}


int GetElementIndex(struct CellType *cell,int i,int j)
/* For given element gives the element index. 
   The indices i and j are expected to range from [1..n] and [1..m]. 
   requires only the structure CellType.
   */
{
  int elemind=0;
 
  if(cell->numbering == NUMBER_XY) 
    elemind = cell->elem1st+(i-1) + (j-1)*cell->elemwidth;
  else if(cell->numbering == NUMBER_YX) 
    elemind = cell->elem1st+(j-1) + (i-1)*cell->elemwidth;

  return(elemind);
}



int GetElementCoordinates(struct CellType *cell,int i,int j,
			  Real *globalcoord,int *ind)
/* For given element gives the coordinates and index for each knot.
   The indices i and j are expected to range from [1..n] and [1..m]. 
   requires only the structure CellType
   Note that this subroutine assumes that the elements are
   rectangular.
   */
{
  int k,nonodes,numbering,elemind=0;
  Real xrat,yrat;

  k = nonodes = cell->nonodes;
  numbering = cell->numbering;

  if(numbering == NUMBER_1D) {
    elemind = cell->elem1st+(i-1);    
    ind[0] = GetKnotCoordinate(cell,i-1,j-1,&globalcoord[0],
			       &globalcoord[nonodes]);
    ind[1] = GetKnotCoordinate(cell,i,j-1,&globalcoord[1],
			       &globalcoord[nonodes+1]);    
    
    if(nonodes == 3) {
      globalcoord[2] = (globalcoord[0]+globalcoord[1])/2.0;
      globalcoord[5] = (globalcoord[3]+globalcoord[4])/2.0;
      ind[2] = ind[0] + 1;
    }
    else if(nonodes == 4) {
      globalcoord[2] = (2.0*globalcoord[0]+globalcoord[1])/3.0;
      globalcoord[6] = (2.0*globalcoord[4]+globalcoord[5])/3.0;
      ind[2] = ind[0] + 1;
      globalcoord[3] = (globalcoord[0]+2.0*globalcoord[1])/3.0;
      globalcoord[7] = (globalcoord[4]+2.0*globalcoord[5])/3.0;
      ind[3] = ind[0] + 2;
    }

    return(elemind);
  }
  else if(numbering == NUMBER_XY) 
    elemind = cell->elem1st+(i-1) + (j-1)*cell->elemwidth;
  else if(numbering == NUMBER_YX)
    elemind = cell->elem1st+(j-1) + (i-1)*cell->elemwidth;


  ind[TOPRIGHT] = GetKnotCoordinate(cell,i,j,&globalcoord[TOPRIGHT],
				    &globalcoord[TOPRIGHT+nonodes]);
  ind[BOTRIGHT] = GetKnotCoordinate(cell,i,j-1,&globalcoord[BOTRIGHT],
				    &globalcoord[BOTRIGHT+nonodes]);
  ind[TOPLEFT] = GetKnotCoordinate(cell,i-1,j,&globalcoord[TOPLEFT],
				    &globalcoord[TOPLEFT+nonodes]);
  ind[BOTLEFT] = GetKnotCoordinate(cell,i-1,j-1,&globalcoord[BOTLEFT],
				    &globalcoord[BOTLEFT+nonodes]);
  if(nonodes == 4) return(elemind);

  GetElementIndices(cell,i,j,ind);

#if 0  
  if(cell->xlinear) 
    xrat = 1.0;
  else
    xrat = sqrt(cell->xratio);

  if(cell->ylinear) 
    yrat = 1.0;
  else 
    yrat = sqrt(cell->yratio);
#endif
  xrat = yrat = 1.0;

  if(nonodes == 5) {
    globalcoord[4] = 0.5*(globalcoord[0]+globalcoord[2]);
    globalcoord[4+k] = 0.5*(globalcoord[k]+globalcoord[2+k]);
  }
  else if(nonodes == 8 || nonodes == 9) {
    globalcoord[4] = (xrat*globalcoord[0]+globalcoord[1])/(1+xrat);
    globalcoord[4+k] = globalcoord[k];    
    globalcoord[5] = globalcoord[1];
    globalcoord[5+k] = 
      (yrat*globalcoord[1+k]+globalcoord[2+k])/(1+yrat);    
    globalcoord[6] = globalcoord[4];
    globalcoord[6+k] = globalcoord[2+k];
    globalcoord[7] = globalcoord[3];
    globalcoord[7+k] = globalcoord[5+k];
    if(nonodes == 9) {
      globalcoord[8] = globalcoord[4];
      globalcoord[8+k] = globalcoord[5+k];
    }
  }
  else if(nonodes == 12 || nonodes == 16) {
    globalcoord[4] = (2.*globalcoord[0]+globalcoord[1])/3.0;
    globalcoord[4+k] = (2.*globalcoord[k]+globalcoord[1+k])/3.0;
    globalcoord[5] = (2.*globalcoord[1]+globalcoord[0])/3.0;
    globalcoord[5+k] = (2.*globalcoord[1+k]+globalcoord[k])/3.0;
    globalcoord[6] = (2.*globalcoord[1]+globalcoord[2])/3.0;
    globalcoord[6+k] = (2.*globalcoord[1+k]+globalcoord[2+k])/3.0;
    globalcoord[7] = (2.*globalcoord[2]+globalcoord[1])/3.0;
    globalcoord[7+k] = (2.*globalcoord[2+k]+globalcoord[1+k])/3.0;
    globalcoord[8] = (2.*globalcoord[2]+globalcoord[3])/3.0;
    globalcoord[8+k] = (2.*globalcoord[2+k]+globalcoord[3+k])/3.0;
    globalcoord[9] = (2.*globalcoord[3]+globalcoord[2])/3.0;
    globalcoord[9+k] = (2.*globalcoord[3+k]+globalcoord[2+k])/3.0;
    globalcoord[10] = (2.*globalcoord[3]+globalcoord[0])/3.0;
    globalcoord[10+k] = (2.*globalcoord[3+k]+globalcoord[k])/3.0;
    globalcoord[11] = (2.*globalcoord[0]+globalcoord[3])/3.0;
    globalcoord[11+k] = (2.*globalcoord[k]+globalcoord[3+k])/3.0;
    if(nonodes == 16) {
      globalcoord[12] = (2.*globalcoord[11]+globalcoord[6])/3.0;
      globalcoord[12+k] = (2.*globalcoord[11+k]+globalcoord[6+k])/3.0;
      globalcoord[13] = (2.*globalcoord[6]+globalcoord[11])/3.0;
      globalcoord[13+k] = (2.*globalcoord[6+k]+globalcoord[11+k])/3.0;
      globalcoord[14] = (2.*globalcoord[7]+globalcoord[10])/3.0;
      globalcoord[14+k] = (2.*globalcoord[7+k]+globalcoord[10+k])/3.0;
      globalcoord[15] = (2.*globalcoord[10]+globalcoord[7])/3.0;
      globalcoord[15+k] = (2.*globalcoord[10+k]+globalcoord[7+k])/3.0;      
    }
  }

#if 0
  for(i=0;i<k;i++) 
    printf("ind[%d]=%d  x=%.4lg  y=%.4lg\n",i,ind[i],
	   globalcoord[i],globalcoord[i+k]);
#endif

  return(elemind);
}


int GetSideInfo(struct CellType *cell,int cellno,int side,int element,
		int *elemind)
/* Given the side and element numbers returns the indices of 
   the side element. When the end of the side is reached the function 
   returns FALSE, otherwise TRUE.
   */
{
  int more,sideno;                 
  int ind[MAXNODESD2];

  more = TRUE;

  if(cell[cellno].numbering == NUMBER_1D) {

    switch(side) {
    case 0:
      more = FALSE;
      elemind[0] = GetElementIndices(&(cell[cellno]),1,element,&(ind[0]));
      if(sideno = cell[cellno].neighbour[LEFT])
	elemind[1] = GetElementIndices(&(cell[sideno]),cell[sideno].xelem,element,&(ind[0]));
      else 
	elemind[1] = 0;
      break;
     
      
    case 1:
      more = FALSE; 
      elemind[0] = GetElementIndices(&(cell)[cellno],cell[cellno].xelem,element,&(ind[0]));
      if(sideno = cell[cellno].neighbour[RIGHT])
	elemind[1] = GetElementIndices(&(cell[sideno]),1,element,&(ind[0]));
      else 
	elemind[1] = 0;
      break;
    }

    return(more);
  }


  switch(side) {

  case LEFT:
    if(element == cell[cellno].yelem) more = FALSE;
    elemind[0] = GetElementIndices(&(cell[cellno]),1,element,&(ind[0]));
    if(sideno = cell[cellno].neighbour[LEFT])
      elemind[1] = GetElementIndices(&(cell[sideno]),cell[sideno].xelem,element,&(ind[0]));
    else 
      elemind[1] = 0;
    break;

  case RIGHT:
    if(element == cell[cellno].yelem) more = FALSE; 
    elemind[0] = GetElementIndices(&(cell)[cellno],cell[cellno].xelem,element,&(ind[0]));
    if(sideno = cell[cellno].neighbour[RIGHT])
      elemind[1] = GetElementIndices(&(cell[sideno]),1,element,&(ind[0]));
    else 
      elemind[1] = 0;
    break;

  case DOWN:
    if(element == cell[cellno].xelem) more = FALSE;
    elemind[0] = GetElementIndices(&(cell)[cellno],element,1,&(ind[0]));
    if(sideno = cell[cellno].neighbour[DOWN])
      elemind[1] = GetElementIndices(&(cell[sideno]),element,cell[sideno].yelem,&(ind[0]));
    else 
      elemind[1] = 0;
    break;

  case UP:
    if(element == cell[cellno].xelem) more = FALSE; 
    elemind[0] = GetElementIndices(&(cell)[cellno],element,cell[cellno].yelem,&(ind[0]));
    if(sideno = cell[cellno].neighbour[UP])
      elemind[1] = GetElementIndices(&(cell[sideno]),element,1,&(ind[0]));
    else 
      elemind[1] = 0;
    break;
    
  default:
    printf("Impossible option in GetSideInfo.\n");
  }

  return(more);
}



void SetElementDivisionExtruded(struct GridType *grid,int info)
{
  int i,nzmax = 0,sumzelems;
  Real ratio,linearlimit;
  Real dzmax = 0,dz = 0;
  
  linearlimit = 0.001;

  if(grid->autoratio) {
    for(i=1;i<=grid->zcells;i++)  
      grid->zelems[i] = grid->minzelems;
  }
  else {
    for(i=1;i<=grid->zcells;i++)  
      if(grid->zelems[i] < grid->minzelems) grid->zelems[i] = grid->minzelems;
  }

  sumzelems = grid->zcells * grid->minzelems;
  if(sumzelems > grid->totzelems) {
#if 0
    printf("SetElementDivision: %d is too few elements in z-direction (min. %d)\n",
	   grid->totzelems,sumzelems);
#endif
    grid->totzelems = sumzelems;
  }      

  /* Put the linearity flags. */
  for(i=1; i<= grid->zcells ;i++) {
    if (fabs(1.-grid->zexpand[i]) < linearlimit) 
      grid->zlinear[i] = TRUE; 
    else 
      grid->zlinear[i] = FALSE;
  }

  if(grid->autoratio) {
    int active;
    for(;;) {
      dzmax = 0.0;
      active = FALSE;

      for(i=1;i<=grid->zcells;i++) {
	if(grid->zelems[i] == 0) continue;
	if(grid->zlinear[i] == TRUE)
	  dz = (grid->z[i] - grid->z[i-1])/(grid->zelems[i]+1);
	else {
	  if(grid->zexpand[i] > 0.0) {
	    ratio = pow(grid->zexpand[i],1./(1.*grid->zelems[i]));
	    dz = (grid->z[i] - grid->z[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)(grid->zelems[i]+1)));
	    if(ratio < 1.)   
	      dz *= grid->zexpand[i];
	  }
	  else if(grid->zelems[i]==1) {
	    dz = (grid->z[i] - grid->z[i-1])/(grid->zelems[i]+1);
	  } 
	  else if((grid->zelems[i]+1)%2 == 0) {
	    ratio = pow(-grid->zexpand[i],1./((grid->zelems[i]+1)/2-1.));
	    dz = 0.5 * (grid->z[i] - grid->z[i-1]) * 
	      (1.-ratio) / (1.-pow(ratio,(Real)((grid->zelems[i]+1)/2)));
	  }
	  else if((grid->zelems[i]+1)%2 == 1) {
	    ratio = pow(-grid->zexpand[i],1./((grid->zelems[i]+1)/2));
	    dz = (grid->z[i] - grid->z[i-1]) / 
	      (2.0*(1.-pow(ratio,(Real)((grid->zelems[i]+1)/2)))/
	       (1-ratio) + pow(ratio,(Real)((grid->zelems[i]+1)/2+0.5)));
	  }
	}
	dz *= grid->zdens[i] / grid->xzratio;

	if(dz > dzmax) {
	  dzmax = dz;
	  nzmax = i;
	}

	if(grid->autoratio) {
	  if(fabs(dz - grid->dx0) < fabs( dz*(1.0/(1.0-1.0/grid->zelems[i])) - grid->dx0) ) {
	    grid->zelems[i] += 1;
	    sumzelems++;
	    active = TRUE;
	  }
	}
      }

      if(grid->autoratio) {
	if(!active) break;
      }
      else {
	grid->zelems[nzmax] += 1;
	sumzelems++;
	if(sumzelems >= grid->totzelems) break;
      }
    }
  }

  /* Set the size of the first element within each subcell */
  grid->totzelems = 0;
  for(i=1;i<=grid->zcells;i++) {
    grid->totzelems += grid->zelems[i];
    if(grid->zlinear[i] == TRUE || grid->zelems[i]==1) 
      grid->dz[i] = (grid->z[i] - grid->z[i-1])/grid->zelems[i];
    else if(grid->zexpand[i] > 0.0) {
      ratio = pow(grid->zexpand[i],1./(grid->zelems[i]-1.));
      grid->zratios[i] = ratio;
      grid->dz[i] = (grid->z[i] - grid->z[i-1]) * 
	(1.-ratio) / (1.-pow(ratio,(Real)(grid->zelems[i])));
    }	
    else if(grid->zelems[i] == 2) {
      grid->dz[i] = (grid->z[i] - grid->z[i-1])/grid->zelems[i];
    }
    else if(grid->zelems[i]%2 == 0) {
      ratio = pow(-grid->zexpand[i],1./(grid->zelems[i]/2-1.));
      grid->zratios[i] = ratio;
	grid->dz[i] = 0.5 * (grid->z[i] - grid->z[i-1]) * 
	  (1.-ratio) / (1.-pow(ratio,(Real)(grid->zelems[i]/2)));
    }
    else if(grid->zelems[i]%2 == 1) {
      ratio = pow(-grid->zexpand[i],1./(grid->zelems[i]/2));
      grid->zratios[i] = ratio;
      grid->dz[i] = (grid->z[i] - grid->z[i-1]) / 
	(2.0*(1.-pow(ratio,(Real)(grid->zelems[i]/2)))/
	 (1-ratio) + pow(ratio,(Real)(grid->zelems[i]/2+0.5)));
    }
  }

  if(info) printf("Created %d extruded divisions.\n",
		  grid->totzelems);
  grid->dz0 = dzmax;
}



void SetElementDivisionCylinder(struct GridType *grid,int info)
{
  int i,k;
  Real ratio,eps;
  Real dzmax;
  
  eps = 1.0e-8;

  grid->zcells = 2*grid->rotateblocks;
  grid->z[0] = 0.0;
  for(i=0;grid->x[i]<eps;i++); k=i;  
  grid->rotateradius1 = grid->x[k];

  if(grid->rotateradius2 < 0.0) {
    grid->rotatecartesian = TRUE;
    grid->rotateradius2 = 1.0e6;
  }
  if(grid->rotateradius2 <= sqrt(2.0)*grid->rotateradius1) {
    if(k+1 <= grid->xcells) 
      grid->rotateradius2 = grid->x[k+1];
    grid->rotateradius2 = MAX(sqrt(2.0)*grid->rotateradius1,
			      grid->rotateradius2);
  }

  if(!grid->xlinear[k]) 
    printf("SetElementDivisionCylinder: The division must be linear!\n");

  for(i=1;i<=grid->zcells;i++) {
    grid->z[i] = i*(2.0*FM_PI)/8.0; 
    grid->zelems[i]  = grid->xelems[k];
    grid->zlinear[i] = grid->xlinear[k];
    grid->zratios[i] = grid->xratios[k];
    grid->zexpand[i] = grid->xexpand[k];
    grid->zmaterial[i] = 0;
  }

  grid->totzelems =  grid->zcells * grid->xelems[k];

  dzmax = 0.0;
  for(i=1;i<=grid->zcells;i++) {
    if(grid->zlinear[i] == TRUE || grid->zelems[i]==1) 
      grid->dz[i] = (grid->z[i] - grid->z[i-1])/grid->zelems[i];
    else if(grid->zexpand[i] > 0.0) {
      ratio = pow(grid->zexpand[i],1./(grid->zelems[i]-1.));
      grid->zratios[i] = ratio;
      grid->dz[i] = (grid->z[i] - grid->z[i-1]) * 
	(1.-ratio) / (1.-pow(ratio,(Real)(grid->zelems[i])));
    }	
    else if(grid->zelems[i] == 2) {
      grid->dz[i] = (grid->z[i] - grid->z[i-1])/grid->zelems[i];
    }
    else if(grid->zelems[i]%2 == 0) {
      ratio = pow(-grid->zexpand[i],1./(grid->zelems[i]/2-1.));
      grid->zratios[i] = ratio;
	grid->dz[i] = 0.5 * (grid->z[i] - grid->z[i-1]) * 
	  (1.-ratio) / (1.-pow(ratio,(Real)(grid->zelems[i]/2)));
    }
    else if(grid->zelems[i]%2 == 1) {
      ratio = pow(-grid->zexpand[i],1./(grid->zelems[i]/2));
      grid->zratios[i] = ratio;
      grid->dz[i] = (grid->z[i] - grid->z[i-1]) / 
	(2.0*(1.-pow(ratio,(Real)(grid->zelems[i]/2)))/
	 (1-ratio) + pow(ratio,(Real)(grid->zelems[i]/2+0.5)));
    }

    if(dzmax < grid->dz[i]) dzmax = grid->dz[i];
  }

  if(info) printf("Created %d divisions in %d cells for rotation [%.2g  %.2g].\n",
		  grid->totzelems,grid->zcells,
		  grid->rotateradius1,grid->rotateradius2);
  grid->dz0 = dzmax;
}


int InspectElement(struct FemType *data,int idx)
{
  int elemtype,nonodes,i,j,k;
  Real *x,*y,*z;
  Real ds,minds,xc,yc,zc;
  Real x0,y0,z0;
  int ii,jj,mini,minj,kk;
  
  elemtype = data->elementtypes[idx];
  printf("Inspecting element %d of type %d\n",idx,elemtype);

  nonodes = elemtype % 100;
  x = data->x;
  y = data->y;
  z = data->z;

  for(k=0;k<nonodes;k++) {
    kk = data->topology[idx][k];
    x0 = x[kk];
    y0 = y[kk];
    z0 = z[kk];
    mini = -1;
    minj = -1;
    minds = 1.0e20;

    for(i=0;i<nonodes;i++) {
      ii = data->topology[idx][i];
      for(j=i+1;j<nonodes;j++) {
	jj = data->topology[idx][j];
	xc = (x[ii]+x[jj])/2;
	yc = (y[ii]+y[jj])/2;
	zc = (z[ii]+z[jj])/2;
	ds = sqrt( pow(x0-xc,2) + pow(y0-yc,2) + pow(z0-zc,2) );
	if( mini == -1 || ds < minds) {
	  mini = i;
	  minj = j;
	  minds = ds;
	}
      }
    }      

    printf("k=%d mini=%d minj=%d ds=%.3e\n",k+1,mini+1,minj+1,minds);
  }

  return(0);
}



/* These functions provide the user of the program information 
   about how the mesh was created and what the resulting sparse 
   matrix will be alike and also present the results of the 
   calculations in various ways. These subroutines don't affect 
   the operation and results of the program and can thus be 
   omitted if not needed. 
   */
   

int matcactive=FALSE, iodebug=FALSE;

int MAXINMETHODS = 15;
char *InMethods[] = {
  /*0*/ "EG",
  /*1*/ "ELMERGRID",
  /*2*/ "ELMERSOLVER",
  /*3*/ "ELMERPOST",
  /*4*/ "ANSYS",
  /*5*/ "IDEAS",
  /*6*/ "ABAQUS",
  /*7*/ "FIDAP",
  /*8*/ "UNV",
  /*9*/ "COMSOL",
  /*10*/ "FIELDVIEW",
  /*11*/ "TRIANGLE",
  /*12*/ "MEDIT",
  /*13*/ "GID",
  /*14*/ "GMSH",
  /*15*/ "PARTITIONED",
};

int MAXOUTMETHODS = 5;
char *OutMethods[] = {
  /*0*/ "EG",
  /*1*/ "ELMERGRID",
  /*2*/ "ELMERSOLVER",
  /*3*/ "ELMERPOST",
  /*4*/ "GMSH",
  /*5*/ "VTU",
};



#ifndef DISABLE_MATC
char *mtc_domath(const char *);
void mtc_init(FILE * input, FILE * output, FILE * error);
#endif

static int Getline(char *line1,FILE *io) 
{
  int i,isend;
  char line0[MAXLINESIZE],*charend,*matcpntr,*matcpntr0;

  for(i=0;i<MAXLINESIZE;i++) 
    line0[i] = ' ';

 newline:

  charend = fgets(line0,MAXLINESIZE,io);
  isend = (charend == NULL);

  if(isend) return(1);

  if(line0[0] == '#' || line0[0] == '%' || line0[0] == '!') goto newline;
  if(!matcactive && line0[0] == '*') goto newline;

#ifndef DISABLE_MATC
  if(matcactive) {
    matcpntr0 = strchr(line0,'$');
    if(matcpntr0) {
      matcpntr = mtc_domath(&matcpntr0[1]);
      if(matcpntr) {
	strcpy(matcpntr0, matcpntr);
	if(0) printf("A: %s\n%s\n",matcpntr0,matcpntr);
      }
    }
  }
#endif 

  if(strstr(line0,"subcell boundaries")) goto newline;
  if(strstr(line0,"material structure")) goto newline;
  if(strstr(line0,"mode")) goto newline;
  if(strstr(line0,"type")) goto newline;

  for(i=0;i<MAXLINESIZE;i++) 
    line1[i] = toupper(line0[i]);

  if(iodebug) {
    printf("line: ");
    for(i=0;i<40;i++) printf("%c",line1[i]);
    printf("\n");
  }

  return(0);
}


static int GetCommand(char *line1,char *line2,FILE *io) 
/* Line1 for commands and line2 for arguments. */
{
  int i,j,isend;
  char line0[MAXLINESIZE],*charend,*matcpntr0,*matcpntr;
  int gotlinefeed;

 newline:

  for(i=0;i<MAXLINESIZE;i++) 
    line2[i] = line1[i] = line0[i] = ' ';

  charend = fgets(line0,MAXLINESIZE,io);
  isend = (charend == NULL);

  if(isend) return(1);

  if(strlen(line0)<=strspn(line0," \t\r\n")) goto newline;

  if(line0[0] == '#' || line0[0] == '%' || line0[0] == '!' || line0[0] == '\n') goto newline;
  if(!matcactive && line0[0] == '*') goto newline;

#ifndef DISABLE_MATC
  if(matcactive) {
    matcpntr0 = strchr(line0,'$');
    if(matcpntr0) {
      matcpntr = mtc_domath(&matcpntr0[1]);
      if(matcpntr) {
	strcpy(matcpntr0, matcpntr);
	if(0) printf("B: %s\n%s\n",matcpntr0,matcpntr);
      }
      else {
	if(0) printf("B0: %s\n",matcpntr0);
	goto newline;
      }
    }
  }
#endif 
  
  gotlinefeed = FALSE;
  j = 0;
  for(i=0;i<MAXLINESIZE;i++) {
    if(line0[i] == '\n' ) {
      gotlinefeed = TRUE;
      break;
    }
    if(line0[i] == '=') {
      j = i;
      break;
    }
    line1[i] = toupper(line0[i]);
  }

  /* After these commands there will be no nextline even though there is no equality sign */
  if(strstr(line1,"END")) return(0);
  if(strstr(line1,"NEW MESH")) return(0);

  if(j) { /* Arguments are actually on the same line after '=' */
    for(i=j+1;i<MAXLINESIZE;i++) {
      line2[i-j-1] = line0[i];    
      if( line0[i] == '\n' ) {
	gotlinefeed = TRUE;
	break;
      }
    }  
    if(!gotlinefeed) {
      printf("There is a risk that somethings was missed in line:\n");
      printf("%s\n",line0);
      smallerror("Check your output line length!\n");
    }
  }
  else { /* rguments are on the next line */
  newline2:
    charend = fgets(line2,MAXLINESIZE,io);
    isend = (charend == NULL);
    if(isend) return(2);
    if(line2[0] == '#' || line2[0] == '%' || line2[0] == '!') goto newline2;
    if(!matcactive && line2[0] == '*') goto newline2;

    gotlinefeed = FALSE;
    for(i=0;i<MAXLINESIZE;i++) {
      if(line2[i] == '\n' ) {
	gotlinefeed = TRUE;
	break;
      }
    }
    if(!gotlinefeed) {
      printf("There is a risk that somethings was missed in line:\n");
      printf("%s\n",line2);
      smallerror("Check your output line length!\n");
    }

#ifndef DISABLE_MATC
    if(matcactive) {
      matcpntr0 = strchr(line2,'$');
      if(matcpntr0) {
	matcpntr = mtc_domath(&matcpntr0[1]);
	if(matcpntr) {
	  strcpy(matcpntr0, matcpntr);
	  if(0) printf("C: %s\n%s\n",matcpntr0,matcpntr);
	}
      }
    }
#endif 
  }

  if(iodebug) {
    printf("command: ");
    for(i=0;i<40;i++) printf("%c",line1[i]);
    printf("\nparams: ");
    for(i=0;i<40;i++) printf("%c",line2[i]);
    printf("\n");
  }
  
  return(0);
}



void InitParameters(struct ElmergridType *eg)
{
  int i;

  eg->relh = 1.0;
  eg->silent = FALSE;
  eg->inmethod = 0;
  eg->outmethod = 0;
  eg->nofilesin = 1;
  eg->unitemeshes = FALSE;
  eg->triangles = FALSE;
  eg->triangleangle = 0.0;
  eg->rotate = FALSE;
  eg->polar = FALSE;
  eg->cylinder = FALSE;
  eg->usenames = TRUE;
  eg->layers = 0;
  eg->layereps = 0.0;
  eg->layermove = 0;
  eg->partitions = 0;
  eg->elements3d = 0;
  eg->nodes3d = 0;
  eg->metis = 0;
  eg->metiscontig = FALSE;
  eg->partopt = 0;
  eg->partoptim = FALSE;
  eg->partbcoptim = TRUE;
  eg->partjoin = 0;
  eg->partitionhalo = 0;
  eg->partitionindirect = FALSE;
  eg->reduce = FALSE;
  eg->increase = FALSE;
  eg->translate = FALSE;
  eg->isoparam = FALSE;
  eg->removelowdim = FALSE;
  eg->removeintbcs = FALSE;
  eg->removeunused = FALSE;
  eg->dim = 3;
  eg->center = FALSE;
  eg->scale = FALSE;
  eg->order = FALSE;
  eg->boundbounds = 0;
  eg->saveinterval[0] = eg->saveinterval[1] = eg->saveinterval[2] = 0;
  eg->bulkbounds = 0;
  eg->partorder = FALSE;
  eg->findsides = FALSE;
  eg->parthypre = FALSE;
  eg->partdual = FALSE;
  eg->partbcz = 0;
  eg->partbcr = 0;
  eg->partbclayers = 1;
  eg->partbcmetis = 0;
  eg->partbw = FALSE;
  eg->saveboundaries = TRUE;
  eg->vtuone = FALSE;
  eg->timeron = FALSE;
  eg->nosave = FALSE;
  eg->nooverwrite = FALSE;
  eg->merge = FALSE;
  eg->bcoffset = FALSE;
  eg->periodic = 0;
  eg->periodicdim[0] = 0;
  eg->periodicdim[1] = 0;
  eg->periodicdim[2] = 0;
  eg->bulkorder = FALSE;
  eg->boundorder = FALSE;
  eg->sidemappings = 0;
  eg->bulkmappings = 0;
  eg->coordinatemap[0] = eg->coordinatemap[1] = eg->coordinatemap[2] = 0;
  eg->clone[0] = eg->clone[1] = eg->clone[2] = 0;
  eg->mirror[0] = eg->mirror[1] = eg->mirror[2] = 0;
  eg->cloneinds = FALSE;
  eg->mirrorbc = 0;
  eg->decimals = 12;
  eg->discont = 0;
  eg->connect = 0;
  eg->connectboundsnosets = 0;

  eg->rotatecurve = FALSE;
  eg->curverad = 0.5;
  eg->curveangle = 90.0;
  eg->curvezet = 0.0;
  
  for(i=0;i<MAXSIDEBULK;i++) 
    eg->sidebulk[i] = 0;
}




int InlineParameters(struct ElmergridType *eg,int argc,char *argv[])
{
  int arg,i,dim;
  char command[MAXLINESIZE];
  
  dim = eg->dim;

  printf("Elmergrid reading in-line arguments\n");

  /* Type of input file */
  strcpy(command,argv[1]);
  for(i=0;i<MAXLINESIZE;i++) command[i] = toupper(command[i]);
  for(i=0;i<=MAXINMETHODS;i++) {
    if(strstr(command,InMethods[i])) {
      eg->inmethod = i;
      break;
    }
  }
  if(i>MAXINMETHODS) eg->inmethod = atoi(argv[1]);


  /* Type of output file (fewer options) */
  strcpy(command,argv[2]);
  for(i=0;i<MAXLINESIZE;i++) command[i] = toupper(command[i]);
  for(i=1;i<=MAXOUTMETHODS;i++) {
    if(strstr(command,OutMethods[i])) {
      eg->outmethod = i;
      break;
    }
  }
  if(i>MAXOUTMETHODS) eg->outmethod = atoi(argv[2]);
 

  /* Default names of output file are derived from input file name */
  strcpy(eg->filesin[0],argv[3]);
  strcpy(eg->filesout[0],eg->filesin[0]);
  strcpy(eg->infofile,eg->filesin[0]);


  /* The optional inline parameters */

  for(arg=4;arg <argc; arg++) {



    if(strcmp(argv[arg],"-in") ==0 ) {
      if(arg+1 >= argc) {
	printf("The secondary input file name is required as a parameter\n");
	return(1);
      }
      else {
	strcpy(eg->filesin[eg->nofilesin],argv[arg+1]);
	printf("A secondary input file %s will be loaded.\n",eg->filesin[eg->nofilesin]);
	eg->nofilesin++;
      }
    }

    if(strcmp(argv[arg],"-out") == 0) {
      if(arg+1 >= argc) {
	printf("The output name is required as a parameter\n");
	return(2);
      }
      else {
	strcpy(eg->filesout[0],argv[arg+1]);
      }
    }

 
    if(strcmp(argv[arg],"-decimals") == 0) {
      eg->decimals = atoi(argv[arg+1]);
    }

    if(strcmp(argv[arg],"-triangles") ==0) {
      eg->triangles = TRUE;
      printf("The rectangles will be split to triangles.\n");
      if(arg+1 < argc) {
	if(strcmp(argv[arg+1],"-")) {
	  eg->triangleangle = atof(argv[arg+1]);
	}
      }
    }

    if(strcmp(argv[arg],"-merge") == 0) {
      if(arg+1 >= argc) {
	printf("Give a parameter for critical distance.\n");
	return(3);
      }
      else {
	eg->merge = TRUE;
	eg->cmerge = atof(argv[arg+1]);
      }
    }

    if(strcmp(argv[arg],"-relh") == 0) {
      if(arg+1 >= argc) {
	printf("Give a relative mesh density related to the specifications\n");
	return(3);
      }
      else {
	eg->relh = atof(argv[arg+1]);
      }
    }

    if(strcmp(argv[arg],"-order") == 0) {
      if(arg+dim >= argc) {
	printf("Give %d parameters for the order vector.\n",dim);
 	return(4);
      }
      else {
	eg->order = TRUE;
	eg->corder[0] = atof(argv[arg+1]);
	eg->corder[1] = atof(argv[arg+2]);
	eg->corder[2] = atof(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-autoorder") == 0) {
      eg->order = 2;
    }

    if(strcmp(argv[arg],"-halo") == 0) {
      eg->partitionhalo = 1;
    }
    if(strcmp(argv[arg],"-halobc") == 0) {
      eg->partitionhalo = 2;
    }
    if(strcmp(argv[arg],"-haloz") == 0) {
      eg->partitionhalo = 3;
    }
    if(strcmp(argv[arg],"-halor") == 0) {
      eg->partitionhalo = 3;
    }
    if(strcmp(argv[arg],"-indirect") == 0) {
      eg->partitionindirect = TRUE;
    }
    if(strcmp(argv[arg],"-metisorder") == 0) {
      eg->order = 3;
    }
    if(strcmp(argv[arg],"-centralize") == 0) {
      eg->center = TRUE;
    }
    if(strcmp(argv[arg],"-scale") == 0) {
      if(arg+dim >= argc) {
	printf("Give %d parameters for the scaling.\n",dim);
 	return(5);
     }
      else {
	eg->scale = TRUE;
	eg->cscale[0] = atof(argv[arg+1]);
	eg->cscale[1] = atof(argv[arg+2]);
	eg->cscale[2] = atof(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-translate") == 0) {
      if(arg+dim >= argc) {
	printf("Give %d parameters for the translate vector.\n",dim);
	return(6);
      }
      else {
	eg->translate = TRUE;
	eg->ctranslate[0] = atof(argv[arg+1]);
	eg->ctranslate[1] = atof(argv[arg+2]);
	eg->ctranslate[2] = atof(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-saveinterval") == 0) {
      if(arg+dim >= argc) {
	printf("Give min, max and step for the interval.\n");
	return(7);
      }
      else {
	eg->saveinterval[0] = atoi(argv[arg+1]);
	eg->saveinterval[1] = atoi(argv[arg+2]);
	eg->saveinterval[2] = atoi(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-rotate") == 0 || strcmp(argv[arg],"-rotate") == 0) {
      if(arg+dim >= argc) {
	printf("Give three parameters for the rotation angles.\n");
	return(8);
      }
      else {
	eg->rotate = TRUE;
	eg->crotate[0] = atof(argv[arg+1]);
	eg->crotate[1] = atof(argv[arg+2]);
	eg->crotate[2] = atof(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-clone") == 0) {
      if(arg+dim >= argc) {
	printf("Give the number of clones in each %d directions.\n",dim);
 	return(9);
     }
      else {
	eg->clone[0] = atoi(argv[arg+1]);
	eg->clone[1] = atoi(argv[arg+2]);
	if(dim == 3) eg->clone[2] = atoi(argv[arg+3]);
      }
    }
    if(strcmp(argv[arg],"-clonesize") == 0) {
      if(arg+dim >= argc) {
	printf("Give the clone size in each %d directions.\n",dim);
 	return(10);
      }
      else {
	eg->clonesize[0] = atof(argv[arg+1]);
	eg->clonesize[1] = atof(argv[arg+2]);
	if(dim == 3) eg->clonesize[2] = atof(argv[arg+3]);
      }
    }
    if(strcmp(argv[arg],"-cloneinds") == 0) {
      eg->cloneinds = TRUE;
    }
    if(strcmp(argv[arg],"-mirror") == 0) {
      if(arg+dim >= argc) {
	printf("Give the symmetry of the coordinate directions, eg. 1 1 0\n");
      }
      else {
	eg->mirror[0] = atoi(argv[arg+1]);
	eg->mirror[1] = atoi(argv[arg+2]);
	if(dim == 3) eg->mirror[2] = atoi(argv[arg+3]);
      }
    }
    if(strcmp(argv[arg],"-mirrorbc") == 0) {
      if(arg+1 >= argc) {
	printf("Give the number of symmetry BC.\n");
 	return(11);
      }
      else {
	eg->mirrorbc = atoi(argv[arg+1]);
      }
    }

    if(strcmp(argv[arg],"-unite") == 0) {
      eg->unitemeshes = TRUE;
      printf("The meshes will be united.\n");
    }   

    if(strcmp(argv[arg],"-nonames") == 0) {
      eg->usenames = FALSE;
      printf("Names will be omitted even if they would exist\n");
    }   

    if(strcmp(argv[arg],"-removelowdim") == 0) {
      eg->removelowdim = TRUE;
      printf("Lower dimensional boundaries will be removed\n");
    }   

    if(strcmp(argv[arg],"-removeintbcs") == 0) {
      eg->removeintbcs = TRUE;
      printf("Lower dimensional boundaries will be removed\n");
    }   

    if(strcmp(argv[arg],"-removeunused") == 0) {
      eg->removeunused = TRUE;
      printf("Nodes that do not appear in any element will be removed\n");
    }   

    if(strcmp(argv[arg],"-autoclean") == 0) {
      eg->removelowdim = TRUE;
      eg->bulkorder = TRUE;
      eg->boundorder = TRUE;
      eg->removeunused = TRUE;
      printf("Lower dimensional boundaries will be removed\n");
      printf("Materials and boundaries will be renumbered\n");
      printf("Nodes that do not appear in any element will be removed\n");
    }   

    if(strcmp(argv[arg],"-polar") == 0) {
      eg->polar = TRUE;
      printf("Making transformation to polar coordinates.\n");
      if(arg+1 >= argc) {
	printf("The preferred radius is required as a parameter\n");
	eg->polarradius = 1.0;
      }
      else {
	eg->polarradius = atoi(argv[arg+1]);
      }
    }

    if(strcmp(argv[arg],"-cylinder") == 0) {
      eg->cylinder = TRUE;
      printf("Making transformation from cylindrical to cartesian coordinates.\n");
    }

    if(strcmp(argv[arg],"-reduce") == 0) {
      if(arg+2 >= argc) {
	printf("Give two material for the interval.\n");
 	return(12);
      }
      else {
	eg->reduce = TRUE;      
	eg->reducemat1 = atoi(argv[arg+1]);
	eg->reducemat2 = atoi(argv[arg+2]);
      }
    }
    if(strcmp(argv[arg],"-increase") == 0) {
      eg->increase = TRUE;
    }
    if(strcmp(argv[arg],"-bulkorder") == 0) {
      eg->bulkorder = TRUE;
    }
    if(strcmp(argv[arg],"-boundorder") == 0) {
      eg->boundorder = TRUE;
    }
    if(strcmp(argv[arg],"-partition") == 0  ||
       strcmp(argv[arg],"-partcell") == 0  || 
       strcmp(argv[arg],"-partcyl") == 0 ) {
      if(arg+dim >= argc) {
	printf("The number of partitions in %d dims is required as parameters.\n",dim);
	return(13);
      }
      else {
	eg->partitions = 1;
	eg->partdim[0] = atoi(argv[arg+1]);
	eg->partdim[1] = atoi(argv[arg+2]);
	if(dim == 3) eg->partdim[2] = atoi(argv[arg+3]);
	eg->partitions = 1;
	for(i=0;i<3;i++) {
	  if(eg->partdim[i] == 0) eg->partdim[i] = 1;
	  eg->partitions *= eg->partdim[i];
	}
	eg->partopt = -1;
	if( strcmp(argv[arg],"-partition") == 0 ) {
	  if(arg+4 < argc) 
	    if(argv[arg+4][0] != '-') eg->partopt = atoi(argv[arg+4]);
	}
	else if( strcmp( argv[arg],"-partcell") == 0 )  {
	  eg->partopt = 2;
	} else if( strcmp( argv[arg],"-partcyl") == 0 ) {
	  eg->partopt = 3;
	}
	  
	printf("The mesh will be partitioned geometrically to %d partitions.\n",
	       eg->partitions);
      }
    }
    if(strcmp(argv[arg],"-partorder") == 0) {
      if(arg+dim >= argc) {
	printf("Give %d parameters for the order vector.\n",dim);
 	return(14);
      }
      else {
	eg->partorder = 1;
	eg->partcorder[0] = atof(argv[arg+1]);
	eg->partcorder[1] = atof(argv[arg+2]);
	if(dim==3) eg->partcorder[2] = atof(argv[arg+3]);
      }
    }
    if(strcmp(argv[arg],"-partoptim") == 0) {
      eg->partoptim = TRUE;
      printf("Aggressive optimization will be applied to node sharing.\n");
    }
    if(strcmp(argv[arg],"-partnobcoptim") == 0) {
      eg->partbcoptim = FALSE;
      printf("Aggressive optimization will not be applied to parent element sharing.\n");
    }
    if(strcmp(argv[arg],"-partbw") == 0) {
      eg->partbw = TRUE;
      printf("Bandwidth will be optimized for partitions.\n");
    }
    if(strcmp(argv[arg],"-parthypre") == 0) {
      eg->parthypre = TRUE;
      printf("Numbering of partitions will be made continous.\n");
    }
    if(strcmp(argv[arg],"-partdual") == 0) {
      eg->partdual = TRUE;
      printf("Using dual (elemental) graph in partitioning.\n");
    }

    if(strcmp(argv[arg],"-metis") == 0 ||
       strcmp(argv[arg],"-metisrec") == 0 ||
       strcmp(argv[arg],"-metiskway") == 0 ) {
#if PARTMETIS
      if(arg+1 >= argc) {
	printf("The number of partitions is required as a parameter\n");
	return(15);
      }
      else {
	eg->metis = atoi(argv[arg+1]);
	printf("The mesh will be partitioned with Metis to %d partitions.\n",eg->metis);
	eg->partopt = 0;
	if(strcmp(argv[arg],"-metisrec") == 0)
	  eg->partopt = 2;
	else if(strcmp(argv[arg],"-metiskway") == 0 )
	  eg->partopt = 3;
	else if(arg+2 < argc) 
	  if(argv[arg+2][0] != '-') eg->partopt = atoi(argv[arg+2]);
      }    
#else
      printf("This version of ElmerGrid was compiled without Metis library!\n");
#endif     
    }

    if(strcmp(argv[arg],"-partjoin") == 0) {
      if(arg+1 >= argc) {
	printf("The number of partitions is required as a parameter!\n");
	return(15);
      }
      else {
	eg->partjoin = atoi(argv[arg+1]);
	printf("The results will joined using %d partitions.\n",eg->partjoin);
      }
    }

    if(strcmp(argv[arg],"-partconnect") == 0 || strcmp(argv[arg],"-partzbc") == 0 ) {
      if(arg+1 >= argc) {
	printf("The number of 1D partitions is required as a parameter!\n");
	return(15);
      }
      else {
	eg->partbcz = atoi(argv[arg+1]);
	printf("The connected BCs will be partitioned to %d partitions in Z.\n",eg->partbcz);
      }
    }

    if(strcmp(argv[arg],"-partrbc") == 0 ) {
      if(arg+1 >= argc) {
	printf("The number of 1D partitions is required as a parameter!\n");
	return(15);
      }
      else {
	eg->partbcr = atoi(argv[arg+1]);
	printf("The connected BCs will be partitioned to %d partitions in R.\n",eg->partbcr);
      }
    }

    if(strcmp(argv[arg],"-partlayers") == 0) {
      if(arg+1 >= argc) {
	printf("The number of layers to be extended is required as a parameter\n");
	return(15);
      }
      else {
	eg->partbclayers = atoi(argv[arg+1]);
	printf("The boundary partitioning will be extended by %d layers.\n",eg->partbclayers);
      }
    }

    if(strcmp(argv[arg],"-metiscontig") == 0 ) {
      eg->metiscontig = TRUE;
    }
    
    if(strcmp(argv[arg],"-metisconnect") == 0 || strcmp(argv[arg],"-metisbc") == 0 ) {
      if(arg+1 >= argc) {
	printf("The number of Metis partitions is required as a parameter\n");
	return(15);
      }
      else {
	eg->partbcmetis = atoi(argv[arg+1]);
	printf("The connected BCs will be partitioned to %d partitions by Metis.\n",eg->partbcmetis);
      }
    }

    if(strcmp(argv[arg],"-periodic") == 0) {
      if(arg+dim >= argc) {
	printf("Give the periodic coordinate directions (e.g. 1 1 0)\n");
 	return(16);
      }
      else {
	eg->periodicdim[0] = atoi(argv[arg+1]);
	eg->periodicdim[1] = atoi(argv[arg+2]);
	if(dim == 3) eg->periodicdim[2] = atoi(argv[arg+3]);
      }
    }

    if(strcmp(argv[arg],"-discont") == 0) {
      if(arg+1 >= argc) {
	printf("Give the discontinuous boundary conditions.\n");
 	return(17);
      }
      else {
	eg->discontbounds[eg->discont] = atoi(argv[arg+1]);
	eg->discont++;
      }
    }

    if(strcmp(argv[arg],"-connect") == 0) {
      if(arg+1 >= argc) {
	printf("Give the connected boundary conditions.\n");
 	return(10);
      }
      else {
	eg->connectboundsnosets += 1;
	for(i=arg+1;i<argc && strncmp(argv[i],"-",1); i++) { 
	  eg->connectbounds[eg->connect] = atoi(argv[i]);
	  eg->connectboundsset[eg->connect] = eg->connectboundsnosets;
	  eg->connect++;
	}
      }
    } 

    if(strcmp(argv[arg],"-connectall") == 0) {
      eg->connectboundsnosets += 1;
      eg->connectbounds[eg->connect] = -1;
      eg->connectboundsset[eg->connect] = eg->connectboundsnosets;
      eg->connect++;
    }

    if(strcmp(argv[arg],"-connectint") == 0) {
      eg->connectboundsnosets += 1;
      eg->connectbounds[eg->connect] = -2;
      eg->connectboundsset[eg->connect] = eg->connectboundsnosets;
      eg->connect++;
    }

    if(strcmp(argv[arg],"-connectfree") == 0) {
      eg->connectboundsnosets += 1;
      eg->connectbounds[eg->connect] = -3;
      eg->connectboundsset[eg->connect] = eg->connectboundsnosets;
      eg->connect++;
    }
 
    if(strcmp(argv[arg],"-boundbound") == 0) {
      for(i=arg+1;i<=arg+3 && i<argc; i++) {
	eg->boundbound[3*eg->boundbounds+i-(1+arg)] = atoi(argv[i]);
	if((i-arg)%3 == 0) eg->boundbounds++;
      }
    } 
    if(strcmp(argv[arg],"-bulkbound") == 0) {
      for(i=arg+1;i<=arg+3 && i<argc; i++) {
	eg->bulkbound[3*eg->bulkbounds+i-(1+arg)] = atoi(argv[i]);
	if((i-arg)%3 == 0) eg->bulkbounds++;
      }
    } 
    if(strcmp(argv[arg],"-boundtype") == 0) {
      for(i=arg+1;i<argc && strncmp(argv[i],"-",1); i++) 
	eg->sidemap[3*eg->sidemappings+i-1-arg] = atoi(argv[i]);
      eg->sidemappings++;
    } 
    if(strcmp(argv[arg],"-bulktype") == 0) {
      for(i=arg+1;i<argc && strncmp(argv[i],"-",1); i++) 
	eg->bulkmap[3*eg->bulkmappings+i-1-arg] = atoi(argv[i]);
      eg->bulkmappings++;
    } 
    if(strcmp(argv[arg],"-coordinatemap") == 0) {
      if( arg+3 >= argc ) {
	printf("Give three parameters for the index permutation\n");
	return(18);
      }
      else {
	for(i=0;i<3;i++) 
	  eg->coordinatemap[i] = atoi(argv[arg+1+i]);
      }
    } 
    if(strcmp(argv[arg],"-layer") == 0) {
      if(arg+4 >= argc) {
	printf("Give four parameters for the layer: boundary, elements, thickness, ratio.\n");
	return(18);
      }
      else if(eg->layers == MAXBOUNDARIES) {
	printf("There can only be %d layers, sorry.\n",MAXBOUNDARIES);
	return(19);
      }
      else {
	eg->layerbounds[eg->layers] = atoi(argv[arg+1]);
	eg->layernumber[eg->layers] = atoi(argv[arg+2]);
	eg->layerthickness[eg->layers] = atof(argv[arg+3]);
	eg->layerratios[eg->layers] = atof(argv[arg+4]);
	eg->layerparents[eg->layers] = 0;
	eg->layers++;
      }
    }
    
    if(strcmp(argv[arg],"-layermove") == 0) {
      if(arg+1 >= argc) {
	printf("Give maximum number of Jacobi filters.\n");
 	return(20);
      }
      else {
	eg->layermove = atoi(argv[arg+1]);
      }
    }

    /* This uses a very dirty trick where the variables related to argument -layer are used 
       with a negative indexing */ 
    if(strcmp(argv[arg],"-divlayer") == 0) {
      if(arg+4 >= argc) {
	printf("Give four parameters for the layer: boundary, elements, relative thickness, ratio.\n");
	return(21);
      }
      else if(abs(eg->layers) == MAXBOUNDARIES) {
	printf("There can only be %d layers, sorry.\n",MAXBOUNDARIES);
	return(22);
      }
      else {
	eg->layerbounds[abs(eg->layers)] = atoi(argv[arg+1]);
	eg->layernumber[abs(eg->layers)] = atoi(argv[arg+2]);
	eg->layerthickness[abs(eg->layers)] = atof(argv[arg+3]);
	eg->layerratios[abs(eg->layers)] = atof(argv[arg+4]);
	eg->layerparents[abs(eg->layers)] = 0;
	eg->layers--;
      }
    }

    if(strcmp(argv[arg],"-3d") == 0) {
      eg->dim = dim = 3;
    }
    if(strcmp(argv[arg],"-2d") == 0) {
      eg->dim = dim = 2;
    }
    if(strcmp(argv[arg],"-1d") == 0) {
      eg->dim = dim = 1;
    }

    if(strcmp(argv[arg],"-isoparam") == 0) {
      eg->isoparam = TRUE;
    }
    if(strcmp(argv[arg],"-nobound") == 0) {
      eg->saveboundaries = FALSE;
    }
    if(strcmp(argv[arg],"-vtuone") == 0) {
      eg->vtuone = TRUE;
    }
    if(strcmp(argv[arg],"-nosave") == 0) {
      eg->nosave = TRUE;
    }
    if(strcmp(argv[arg],"-nooverwrite") == 0) {
      eg->nooverwrite = TRUE;
    }
    if(strcmp(argv[arg],"-timer") == 0) {
      eg->timeron = TRUE;
    }

    if(strcmp(argv[arg],"-infofile") == 0) {
      eg->timeron = TRUE;
      if(arg+1 >= argc) {
	printf("The output name is required as a parameter\n");
	return(2);
      }
      else {
	strcpy(eg->infofile,argv[arg+1]);
      }
    }
    

    /* The following keywords are not actively used */

    if(strcmp(argv[arg],"-bcoffset") == 0) {
      eg->bcoffset = atoi(argv[arg+1]);
    }
    if(strcmp(argv[arg],"-noelements") == 0) {
      eg->elements3d = atoi(argv[arg+1]);
    }
    if(strcmp(argv[arg],"-nonodes") == 0) {
      eg->nodes3d = atoi(argv[arg+1]);
    }

    if(strcmp(argv[arg],"-sidefind") == 0) {
      eg->findsides = 0;
      for(i=arg+1;i<argc && strncmp(argv[i],"-",1); i++) {
	eg->sidebulk[i-1-arg] = atoi(argv[i]);
	eg->findsides++;
      }
    } 
    if(strcmp(argv[arg],"-findbound") == 0) {
      eg->findsides = 0;
      for(i=arg+1;i+1<argc && strncmp(argv[i],"-",1); i += 2) {
	eg->sidebulk[i-1-arg] = atoi(argv[i]);
	eg->sidebulk[i-arg] = atoi(argv[i+1]);
	eg->findsides++;
      }
    } 
  }

  {
    int badpoint;
    char *ptr1,*ptr2;
    ptr1 = strrchr(eg->filesout[0], '.');
    if(ptr1) {
      badpoint=FALSE;
      ptr2 = strrchr(eg->filesout[0], '/');
      if(ptr2 && ptr2 > ptr1) badpoint = TRUE;
      if(!badpoint) *ptr1 = '\0';
    }
  }

  printf("Output will be saved to file %s.\n",eg->filesout[0]);

  return(0);
}




int LoadCommands(char *prefix,struct ElmergridType *eg,
		 struct GridType *grid, int mode,int info) 
{
  char filename[MAXFILESIZE];
  char command[MAXLINESIZE],params[MAXLINESIZE],*cp;

  FILE *in=NULL;
  int i,j;

  iodebug = FALSE;

  if( mode == 0) {  
    if (in = fopen("ELMERGRID_STARTINFO","r")) {
      fscanf(in,"%s",filename);
      fclose(in);
      printf("Using the file %s defined in ELMERGRID_STARTINFO\n",filename);
      if ((in = fopen(filename,"r")) == NULL) {
	printf("LoadCommands: opening of the file '%s' wasn't succesfull !\n",filename);
	return(1);
      }    
      else printf("Loading ElmerGrid commands from file '%s'.\n",filename);    
    }    
    else 
      return(2);
  }
  else if(mode == 1) { 
    AddExtension(prefix,filename,"eg");
    if ((in = fopen(filename,"r")) == NULL) {
      printf("LoadCommands: opening of the file '%s' wasn't succesfull !\n",filename);
      return(3);
    }    
    if(info) printf("Loading ElmerGrid commands from file '%s'.\n",filename);    
  }
  else if(mode == 2) {
    AddExtension(prefix,filename,"grd");
    if ((in = fopen(filename,"r")) == NULL) {
      printf("LoadCommands: opening of the file '%s' wasn't succesfull !\n",filename);
      return(4);
    }    
    if(info) printf("\nLoading ElmerGrid commands from file '%s'.\n",filename);
  }



  for(;;) {

    if(GetCommand(command,params,in)) {
      printf("Reached the end of command file\n");
      goto end;
    }    

    /* If the mode is the command file mode read also the file information from the command file. */

    if(mode <= 1) {
      if(strstr(command,"INPUT FILE")) {
	sscanf(params,"%s",eg->filesin[0]);
      }

      else if(strstr(command,"OUTPUT FILE")) {
	sscanf(params,"%s",eg->filesout[0]);
      }

      else if(strstr(command,"INPUT MODE")) {
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	
	for(i=0;i<=MAXINMETHODS;i++) {
	  if(strstr(params,InMethods[i])) {
	    eg->inmethod = i;
	    break;
	  }
	}
	if(i>MAXINMETHODS) sscanf(params,"%d",&eg->inmethod);
      }

      else if(strstr(command,"OUTPUT MODE")) {
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	
	/* Type of output file (fewer options) */
	for(i=1;i<=MAXOUTMETHODS;i++) {
	  if(strstr(params,OutMethods[i])) {
	    eg->outmethod = i;
	    break;
	  }
	}
	if(i>MAXOUTMETHODS) sscanf(params,"%d",&eg->outmethod);	
      }
    }    
    /* End of command file specific part */


    if(strstr(command,"DECIMALS")) {
      sscanf(params,"%d",&eg->decimals);
    }
    else if(strstr(command,"BOUNDARY OFFSET")) {
      sscanf(params,"%d",&eg->bcoffset);
    }
    else if(strstr(command,"TRIANGLES CRITICAL ANGLE")) {
      sscanf(params,"%le",&eg->triangleangle);
    }
    else if(strstr(command,"TRIANGLES")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->triangles = TRUE;      
    }
    else if(strstr(command,"MERGE NODES")) {
      eg->merge = TRUE;
      sscanf(params,"%le",&eg->cmerge);
    }
    else if(strstr(command,"UNITE")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->unitemeshes = TRUE;      
    }
    else if(strstr(command,"ORDER NODES")) {
      eg->order = TRUE;
      if(eg->dim == 1) 
	sscanf(params,"%le",&eg->corder[0]);
      else if(eg->dim == 2) 
	sscanf(params,"%le%le",&eg->corder[0],&eg->corder[1]);
      else if(eg->dim == 3) 
	sscanf(params,"%le%le%le",&eg->corder[0],&eg->corder[1],&eg->corder[2]);
    }
    else if(strstr(command,"SCALE")) {
      eg->scale = TRUE;
      if(eg->dim == 1) 
	sscanf(params,"%le",&eg->cscale[0]);
      else if(eg->dim == 2) 
	sscanf(params,"%le%le",&eg->cscale[0],&eg->cscale[1]);
      else if(eg->dim == 3) 
	sscanf(params,"%le%le%le",&eg->cscale[0],&eg->cscale[1],&eg->cscale[2]);
    }
    else if(strstr(command,"CENTRALIZE")) {
      eg->center = TRUE;
    }
    else if(strstr(command,"TRANSLATE")) {
      eg->translate = TRUE;
      if(eg->dim == 1) 
	sscanf(params,"%le",&eg->ctranslate[0]);
      else if(eg->dim == 2) 
	sscanf(params,"%le%le",&eg->ctranslate[0],&eg->ctranslate[1]);
      else if(eg->dim == 3) 
	sscanf(params,"%le%le%le",&eg->ctranslate[0],&eg->ctranslate[1],&eg->ctranslate[2]);
    }
    else if(strstr(command,"ROTATE MESH")) {
      eg->rotate = TRUE;
      sscanf(params,"%le%le%le",&eg->crotate[0],&eg->crotate[1],&eg->crotate[2]);
    }
    else if(strstr(command,"CLONE")) {
      if(strstr(command,"CLONE SIZE")) {
	if(eg->dim == 1) 
	  sscanf(params,"%le",&eg->clonesize[0]);
	else if(eg->dim == 2) 
	  sscanf(params,"%le%le",&eg->clonesize[0],&eg->clonesize[1]);
	else if(eg->dim == 3) 
	  sscanf(params,"%le%le%le",&eg->clonesize[0],&eg->clonesize[1],&eg->clonesize[2]);	
      }
      else {
	if(eg->dim == 1) 
	  sscanf(params,"%d",&eg->clone[0]);
	else if(eg->dim == 2) 
	  sscanf(params,"%d%d",&eg->clone[0],&eg->clone[1]);
	else if(eg->dim == 3) 
	  sscanf(params,"%d%d%d",&eg->clone[0],&eg->clone[1],&eg->clone[2]);
      }
    }
    else if(strstr(command,"MERGE")) {
      eg->merge = TRUE;
      sscanf(params,"%le",&eg->cmerge);
    }
    else if(strstr(command,"MIRROR")) {
      if(eg->dim == 1) 
	sscanf(params,"%d",&eg->mirror[0]);
      else if(eg->dim == 2) 
	sscanf(params,"%d%d",&eg->mirror[0],&eg->mirror[1]);
      else if(eg->dim == 3) 
	sscanf(params,"%d%d%d",&eg->mirror[0],&eg->mirror[1],&eg->mirror[2]);
    }
    else if(strstr(command,"POLAR RADIUS")) {
      eg->polar = TRUE;
      sscanf(params,"%le",&eg->polarradius);
    }
    else if(strstr(command,"CYLINDER")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->cylinder = TRUE;      
    }
    else if(strstr(command,"REDUCE DEGREE")) {
      eg->reduce = TRUE;
      sscanf(params,"%d%d",&eg->reducemat1,&eg->reducemat2);
    }
    else if(strstr(command,"INCREASE DEGREE")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->increase = TRUE;      
    }
    else if(strstr(command,"METIS OPTION")) {
#if PARTMETIS
      sscanf(params,"%d",&eg->partopt);
#else
      printf("This version of ElmerGrid was compiled without Metis library!\n");
#endif
    }
    else if(strstr(command,"METIS")) {
#if PARTMETIS
      sscanf(params,"%d",&eg->metis);
#else
      printf("This version of ElmerGrid was compiled without Metis library!\n");
#endif
    }
    else if(strstr(command,"METISKWAY")) {
#if PARTMETIS
      sscanf(params,"%d",&eg->metis);
      eg->partopt = 3;
#else
      printf("This version of ElmerGrid was compiled without Metis library!\n");
#endif
    }
    else if(strstr(command,"METISREC")) {
#if PARTMETIS
      sscanf(params,"%d",&eg->metis);
      eg->partopt = 2;
#else
      printf("This version of ElmerGrid was compiled without Metis library!\n");
#endif
    }
    else if(strstr(command,"PARTITION DUAL")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partdual = TRUE;      
    }
    else if(strstr(command,"PARTJOIN")) {
      sscanf(params,"%d",&eg->partjoin);
    }
    else if(strstr(command,"PARTITION ORDER")) {
      eg->partorder = 1;
      if(eg->dim == 2) sscanf(params,"%le%le",&eg->partcorder[0],&eg->partcorder[1]);
      if(eg->dim == 3) sscanf(params,"%le%le%le",&eg->partcorder[0],
			      &eg->partcorder[1],&eg->partcorder[2]);      
    }
    else if(strstr(command,"PARTITION") || strstr(command,"PARTCYL") || strstr(command,"PARTCELL")) {
      if(eg->dim == 2) sscanf(params,"%d%d",&eg->partdim[0],&eg->partdim[1]);
      if(eg->dim == 3) sscanf(params,"%d%d%d",&eg->partdim[0],&eg->partdim[1],&eg->partdim[2]);
      eg->partitions = 1;
      for(i=0;i<eg->dim;i++) {
	if(eg->partdim[i] < 1) eg->partdim[i] = 1;
	eg->partitions *= eg->partdim[i];
      }
      if( strstr(command,"PARTCYL") ) eg->partopt = 3;
      if( strstr(command,"PARTCCELL") ) eg->partopt = 2;
    }
    else if(strstr(command,"PERIODIC")) {
      if(eg->dim == 2) sscanf(params,"%d%d",&eg->periodicdim[0],&eg->periodicdim[1]);
      if(eg->dim == 3) sscanf(params,"%d%d%d",&eg->periodicdim[0],
			      &eg->periodicdim[1],&eg->periodicdim[2]);
    }
    else if(strstr(command,"HALO")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partitionhalo = 1;      
    }
    else if(strstr(command,"BOUNDARY HALO")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partitionhalo = 2;
    }
    else if(strstr(command,"EXTRUDED HALO")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partitionhalo = 3;
    }
    else if(strstr(command,"PARTBW")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partbw = TRUE;      
    }
    else if(strstr(command,"PARTHYPRE")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->parthypre = TRUE;      
    }
    else if(strstr(command,"INDIRECT")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->partitionindirect = TRUE;      
    }
    else if(strstr(command,"BOUNDARY TYPE MAPPINGS")) {
      for(i=0;i<MAXMAPPINGS;i++) {
	if(i>0) Getline(params,in);
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->sidemap[3*i],&eg->sidemap[3*i+1],&eg->sidemap[3*i+2]);
      }
      printf("Found %d boundary type mappings\n",i);
      eg->sidemappings = i;
    }
    else if(strstr(command,"BULK TYPE MAPPINGS")) {
      for(i=0;i<MAXMAPPINGS;i++) {
	if(i>0) Getline(params,in);
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->bulkmap[3*i],&eg->bulkmap[3*i+1],&eg->bulkmap[3*i+2]);
      }
      printf("Found %d bulk type mappings\n",i);
      eg->bulkmappings = i;
    }
    else if(strstr(command,"COORDINATE MAPPING")) {
      sscanf(params,"%d%d%d",&eg->coordinatemap[0],&eg->coordinatemap[1],&eg->coordinatemap[2]);
    }	
    else if(strstr(command,"BOUNDARY BOUNDARY")) {
      for(i=0;i<MAXBOUNDARIES;i++) {
	if(i>0) Getline(params,in);
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->boundbound[3*i+2],&eg->boundbound[3*i],&eg->boundbound[3*i+1]);
      }
      printf("Found %d boundary boundary definitions\n",i);
      eg->boundbounds = i;
    }
    else if(strstr(command,"MATERIAL BOUNDARY")) {
      for(i=0;i<MAXBOUNDARIES;i++) {
	if(i>0) Getline(params,in);
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->bulkbound[3*i+2],&eg->bulkbound[3*i],&eg->bulkbound[3*i+1]);
      }
      printf("Found %d material boundary definitions\n",i);
      eg->bulkbounds = i;
    }

    else if(strstr(command,"RENUMBER BOUNDARY")) {
      for(i=0;i<MAXBOUNDARIES;i++) {
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->sidemap[3*i],&eg->sidemap[3*i+1],&eg->sidemap[3*i+2]);
      }
      printf("Found %d boundary mappings\n",i);
      eg->sidemappings = i;
    }
    else if(strstr(command,"RENUMBER MATERIAL")) {
      for(i=0;i<MAXBOUNDARIES;i++) {
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	cp = params;      
	sscanf(params,"%d%d%d",&eg->bulkmap[3*i],&eg->bulkmap[3*i+1],&eg->bulkmap[3*i+2]);
      }
      printf("Found %d material mappings\n",i);
      eg->bulkmappings = i;
    }

    else if(strstr(command,"BOUNDARY LAYER")) {
      if(strstr(command,"BOUNDARY LAYER MOVE")) {
	sscanf(params,"%d",&eg->layermove);
      }
      else if(strstr(command,"BOUNDARY LAYER EPSILON")) {
	sscanf(params,"%le",&eg->layereps);
      }
      else {
	for(i=0;i<MAXBOUNDARIES;i++) {
	  if(i>0) Getline(params,in);
	  for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	  cp = params;      

	  if(strstr(params,"END") || strstr(params,"End") ) break;
	  eg->layerbounds[i] = next_int(&cp);
	  eg->layernumber[i] = next_int(&cp);
	  eg->layerthickness[i] = next_real(&cp);
	  eg->layerratios[i] = next_real(&cp);
	  eg->layerparents[i] = next_int(&cp);	  
	}
	printf("Found %d boundary layers\n",i);
	eg->layers = i;
      }
    }
    else if(strstr(command,"REMOVE LOWER DIMENSIONAL BOUNDARIES")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->removelowdim = TRUE; 
    }
    else if(strstr(command,"REMOVE INTERNAL BOUNDARIES")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->removeintbcs = TRUE; 
    }
    else if(strstr(command,"REMOVE UNUSED NODES")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->removeunused = TRUE; 
    }
    else if(strstr(command,"NO MESH NAMES")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->usenames = FALSE; 
    }
    else if(strstr(command,"REORDER MATERIAL")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->bulkorder = TRUE; 
    }
    else if(strstr(command,"REORDER BOUNDARY")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->boundorder = TRUE; 
    }
    else if(strstr(command,"DIMENSION")) {
      sscanf(params,"%d",&eg->dim);
    }
    else if(strstr(command,"ISOPARAMETRIC")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->isoparam = TRUE;
    }
    else if(strstr(command,"NO BOUNDARY")) {
      for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
      if(strstr(params,"TRUE")) eg->saveboundaries = FALSE;
    }
    else if(strstr(command,"LAYERED BOUNDARIES")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"TRUE")) grid->layeredbc = 1;
      if(strstr(params,"FALSE")) grid->layeredbc = 0;
    }
    else if(strstr(command,"EXTRUDED")) {
      grid->dimension = 3;

      if(strstr(command,"EXTRUDED DIVISIONS")) {
	sscanf(params,"%d",&grid->zcells);		
      }
      if(strstr(command,"EXTRUDED BC OFFSET")) {
	sscanf(params,"%d",&grid->layerbcoffset);		
      }
      else if(strstr(command,"EXTRUDED LIMITS")) {
	cp = params;
	for(i=0;i<=grid->zcells;i++ ) { 
	  grid->z[i] = next_real(&cp);
	  if(i > 0 && grid->z[i] < grid->z[i-1]) {
	    printf("Extruded limits %d: %12.6le %12.6le\n",i,grid->z[i],grid->z[i-1]);
	    bigerror("Values for limits should be a growing series, existing\n");
	  }
	}
      }
      else if(strstr(command,"EXTRUDED SIZES")) {
	cp = params;
	for(i=1;i<=grid->zcells;i++) grid->z[i] = next_real(&cp);
	for(i=1;i<=grid->zcells;i++) grid->z[i] = grid->z[i-1] + grid->z[i];
      }     
      else if(strstr(command,"EXTRUDED ELEMENTS")) {
	cp = params;
	for(i=1;i<=grid->zcells;i++) grid->zelems[i] = next_int(&cp);
	grid->autoratio = FALSE;    
      } 
      else if(strstr(command,"EXTRUDED RATIOS")) {
	cp = params;
	for(i=1;i<=grid->zcells;i++) grid->zexpand[i] = next_real(&cp);
      }
      else if(strstr(command,"EXTRUDED DENSITIES")) {
	cp = params;
	for(i=1;i<=grid->zcells;i++) grid->zdens[i] = next_real(&cp);
      }
      else if(strstr(command,"EXTRUDED STRUCTURE")) {
	for(i=1;i<= grid->zcells;i++) {
	  if(i>1) Getline(params,in);
	  sscanf(params,"%d %d %d\n",
		 &grid->zfirstmaterial[i],&grid->zlastmaterial[i],&grid->zmaterial[i]); 
	}
      }
      else if(strstr(command,"EXTRUDED MAX MATERIAL")) {
	sscanf(params,"%d",&grid->maxmaterial);		
      }
      else if(strstr(command,"EXTRUDED MATERIAL MAPPINGS")) {
	grid->zmaterialmap = Imatrix(1,grid->zcells,1,grid->maxmaterial);
	for(i=1;i<=grid->zcells;i++) {
	  if(i>1) Getline(params,in); 
	  cp = params;
	  for(j=1;j<=grid->maxmaterial;j++)
	    grid->zmaterialmap[i][j] = next_int(&cp);
	}
	grid->zmaterialmapexists = TRUE;
      }
      else if(strstr(command,"EXTRUDED HELICITY")) {
	sscanf(params,"%le",&grid->zhelicity);			
	grid->zhelicityexists = TRUE;
      }

    }
  }

end:
  printf("Read commands from a file\n");

  return(0);
}



int SaveCellInfo(struct GridType *grid,struct CellType *cell,
		 char *prefix,int info)
/* Save some (int) values for each cell in structure CellType. 
   The resulting matrix may be used to check that the division to 
   subcells is as wanted.
   */
{
  int i; 
  FILE *out;
  char filename[MAXFILESIZE];

  AddExtension(prefix,filename,"cell");
  out = fopen(filename,"w");

  fprintf(out,"%-6s %-6s %-6s %-6s %-6s %-6s %-6s %-6s %-6s\n",
	  "1st","2nd","last","level","center","mat","xlin","ylin","num");
  for(i=1;i<=grid->nocells;i++) {
    fprintf(out,"%-6d %-6d %-6d %-6d %-6d %-6d %-6d %-6d %-6d\n",
	    cell[i].left1st,cell[i].left2nd,cell[i].leftlast,
	    cell[i].levelwidth,cell[i].leftcenter,
	    cell[i].material,cell[i].xlinear,cell[i].ylinear,cell[i].numbering);
  }
  fclose(out);

  if(info) printf("The cell information was saved to file %s.\n",filename);

  return(0);
}



int SaveBoundary(struct FemType *data,struct BoundaryType *bound,
		 char *prefix,int info)
/* This function saves the given boundary to an ascii-file.
   The data is saved in format [x1 x2 y1 y2 v1 v2].
   */
{
  int i,k,sideelemtype;
  FILE *out;
  char filename[MAXFILESIZE];
  int sideind[MAXNODESD1]; 

  if(!bound->created) {
    printf("SaveBoundary: You tried to save a nonexisting boundary.\n");
    return(1);
  }
  if(bound->nosides == 0) return(0);

  AddExtension(prefix,filename,"bound");
  out = fopen(filename,"w");

  for(i=1; i <= bound->nosides; i++) {

    GetElementSide(bound->parent[i],bound->side[i],bound->normal[i],data,sideind,&sideelemtype);

    fprintf(out,"%-12.4e %-12.4e %-12.4e %-12.4e ",
	    data->x[sideind[0]],data->x[sideind[1]],
	    data->y[sideind[0]],data->y[sideind[1]]);
    for(k=0;k<MAXVARS;k++) 
      if(bound->evars[k]) {
	if(bound->points[k] == 1) 
	  fprintf(out,"%-10.4e ",bound->vars[k][i]);
      }		
    fprintf(out,"\n");
  }

  fclose(out);

  if(info) printf("Boundary information was saved to file %s.\n",filename);

  return(0);
}


int SaveBoundariesChain(struct FemType *data,struct BoundaryType *bound,
			char *prefix,int info)
/* This function saves the given boundary to an ascii-file.
   The data is saved in format [x y v].
   This may be used particularly for boundaries that 
   are continues i.e. the element sides constitute a full chain. 
   */
{
  int i,j,k,ind,length,col;
  FILE *out;
  char filename[MAXFILESIZE];
  char filename2[MAXFILESIZE];

  for(j=0;j<MAXBOUNDARIES;j++) 
    if(bound[j].created && bound[j].nosides >0) {

      CreateBoundaryChain(data,&bound[j],info);
      length = bound[j].chainsize;
      if(length < 2) continue;

      sprintf(filename,"%s%d%s",prefix,j,".side");
      out = fopen(filename,"w");

      for(i=0;i<=length;i++) {
	ind = bound[j].chain[i];
	fprintf(out,"%-10.4e %-10.4e %-6d ",
		data->x[ind],data->y[ind],ind);
	for(k=0;k<MAXVARS;k++) 
	  if(bound[j].evars[k]) {
	    if(bound[j].points[k] == 0)
	      fprintf(out,"%-10.4e ",bound[j].vars[k][i]);
	    else if(bound[j].points[k] == 1) {
	      if(i==0)
		fprintf(out,"%-10.4e ",bound[j].vars[k][1]);		
	      else if(i==length)
		fprintf(out,"%-10.4e ",bound[j].vars[k][length]);		
	      else
		fprintf(out,"%-10.4e ",
			0.5*(bound[j].vars[k][i]+bound[j].vars[k][i+1]));	
	    }
	  }

	for(k=0;k<MAXDOFS;k++) {
	  if(data->edofs[k] == 1) 
	    fprintf(out,"%-10.4e  ",data->dofs[k][ind]);
	  if(data->edofs[k] == 2) 
	    fprintf(out,"%-10.4e  %-10.4e  ",
		    data->dofs[k][2*ind-1],data->dofs[k][2*ind]);
	}
	
	fprintf(out,"\n");
      }
      fclose(out);

      sprintf(filename2,"%s%d%s",prefix,j,".sidetxt");
      out = fopen(filename2,"w");
      fprintf(out,"Degrees of freedom in file %s are as follows:\n",filename);
      if(bound->coordsystem == COORD_CART2) {
	fprintf(out,"col1: X coordinate\n");
	fprintf(out,"col2: Y coordinate\n");
      }
      else if(bound->coordsystem == COORD_AXIS) {
	fprintf(out,"col1: R coordinate\n");
	fprintf(out,"col2: Z coordinate\n");
      }
      else if(bound->coordsystem == COORD_POLAR) {
	fprintf(out,"col1: R coordinate\n");
	fprintf(out,"col2: F coordinate\n");
      }
      fprintf(out,"col3: node indices\n");
      col = 3;
      for(k=0;k<MAXVARS;k++) 
	if(bound[j].evars[k] && bound[j].points[k] <= 1) 
	  fprintf(out,"col%d: %s\n",++col,bound[j].varname[k]);	  
      for(k=0;k<MAXDOFS;k++) {
	if(data->edofs[k] == 1) 
	  fprintf(out,"col%d: %s\n",++col,data->dofname[k]);	  
	if(data->edofs[k] == 2) {
	  fprintf(out,"col%d: %s1\n",++col,data->dofname[k]);	  
	  fprintf(out,"col%d: %s2\n",++col,data->dofname[k]);	  
	}
      }
      fclose(out);

      if(info) printf("Boundary info was saved to files %s and %s.\n",
		      filename,filename2);
    }

  return(0);
}



int SaveBoundaryForm(struct FemType *data,struct CellType *cell,
		     char* prefix,int info)
/* Saves the form of the boundary as given by function GetSideInfo 
   in form [x,y,index]. 
   */
{
  int sideknots,elemno,side,more,elemind[2],sideelemtype;
  int no,sideind[MAXNODESD1];
  FILE *out;
  char filename[MAXFILESIZE];

  if(data->created == FALSE) {
    printf("SaveBoundaryForm: stucture FemType not created\n");
    return(1);
  }

  sideknots = 0;
  more = FALSE;

  AddExtension(prefix,filename,"boundary");
  out = fopen(filename,"w");

  /* Go through all pairs of points and save them into a matrix. */
  for(no=1; no <= data->nocells; no++)
    for(side=0; side < 4; side++) 
      if(cell[no].material != cell[no].boundary[side]) {
        elemno = 0; 
        do { 
          elemno++;
	  sideknots++;
          more = GetSideInfo(cell,no,side,elemno,elemind);
	  GetElementSide(elemind[0],side,1,data,sideind,&sideelemtype);
	  
	  fprintf(out,"%-12.4e %-12.4e %-12.4e %-12.4e\n",
		  data->x[sideind[0]],data->x[sideind[1]],
		  data->y[sideind[0]],data->y[sideind[1]]);
        } while(more);
      }

  fclose(out);

  if(info) printf("%d boundaries between materials were saved to file %s.\n",
		  sideknots,filename);
  return(0);
}


int SaveBoundaryLine(struct FemType *data,int direction,
		     Real c0,char* prefix,int info)
/* Saves the nodes forming a vertical or a horizontal line. 
   The format is [x y v1 v2 v3 ...].
   */
{
  int k,no,points;
  FILE *out;
  char filename[MAXFILESIZE];
  Real c,c1,eps;

  if(data->created == FALSE) {
    printf("SaveBoundaryLine: stucture FemType not created\n");
    return(1);
  }

  eps = 1.0e-6;
  points = 0;

  AddExtension(prefix,filename,"line");
  out = fopen(filename,"w");

  /* Go through all pairs of points and save them into amatrix. */
  
  c1 = data->x[1];
  for(no=1; no <= data->noknots; no++) {
    if(direction > 0) 
      c = data->x[no];
    else
      c = data->y[no];
    if(fabs(c-c0) < fabs(c1-c0))
      c1 = c;
  }
  for(no=1; no <= data->noknots; no++) {
    if(direction > 0) 
      c = data->x[no];
    else
      c = data->y[no];
    if(fabs(c-c1) < eps) {
      if(direction > 0) 
	fprintf(out,"%-12.7e %-12.7e ",c,data->y[no]);
      else 
	fprintf(out,"%-12.7e %-12.7e ",data->x[no],c);	
      for(k=0;k<MAXDOFS;k++) {
	if(data->edofs[k] == 1) 
	  fprintf(out,"%-12.7e ",data->dofs[k][no]);
	if(data->edofs[k] == 2) 
	  fprintf(out,"%-12.7e %-12.7e ",
		  data->dofs[k][2*no-1],data->dofs[k][2*no]);
      }	
      fprintf(out,"\n");
      points++;
    }
  }
  if(info) printf("Line (c=%.3g) with %d nodes was saved to file %s.\n",
		  c1,points,filename);
  
  fclose(out);
  
  return(0);
}


int SaveSubcellForm(struct FemType *data,struct CellType *cell, 
		    char* prefix,int info)
/* Saves the form of the boundary as given by function GetSideInfo 
   in form [x,y,index]. 
   */
{
  int more,sideknots,elemno,elemind[2],side,i;
  int no,sideind[MAXNODESD1],nosidenodes,sidelemtype;
  FILE *out;
  char filename[MAXFILESIZE];

  sideknots = 0;
  more = FALSE;

  if(data->created == FALSE) {
    printf("SaveSubcellForm: stucture FemType not created\n");
    return(1);
  }

  AddExtension(prefix,filename,"subcell");
  out = fopen(filename,"w");

  /* Go through all pairs of points and save them into amatrix. */
  for(no=1; no <= data->nocells; no++)
    for(side=0; side < 4; side++) 
      if(cell[no].boundary[side]) {
        elemno = 0; 
        do { 
          elemno++;
	  sideknots++;
          more = GetSideInfo(cell,no,side,elemno,elemind);
	  GetElementSide(elemind[0],side,1,data,sideind,&sidelemtype);
	  nosidenodes = sidelemtype%100;
	  for(i=0;i<nosidenodes;i++) 
	    fprintf(out,"%-12.4e %-12.4e %-8d\n",
		    data->x[sideind[i]],data->y[sideind[i]],sideind[i]);
        } while(more);
      }
  fclose(out);

  if(info) printf("There are %d sideknots in the elements.\n",sideknots);
  if(info) printf("The positions of the sideknots were saved in file %s.\n",filename);

  return(0);
}



int SaveElmergrid(struct GridType *grid,int nogrids,char *prefix,int info)
{
  int sameline,maxsameline;
  int i,j,dim;
  FILE *out;
  char filename[MAXFILESIZE];

  AddExtension(prefix,filename,"grd");
  out = fopen(filename,"w");
  dim = grid->dimension;
  if(grid->coordsystem == COORD_CART1) dim = 1;

  j = 0;
  sameline = TRUE;
  maxsameline = 6;
  if(grid->xcells > maxsameline) sameline = FALSE;
  if(dim >= 2 && grid->ycells > maxsameline) sameline = FALSE;
  if(dim >= 3 && grid->zcells > maxsameline) sameline = FALSE;
  
  fprintf(out,"#####  ElmerGrid input file for structured grid generation  ######\n");
  fprintf(out,"Version = 210903\n");

  fprintf(out,"Coordinate System = ");
  if(grid->coordsystem == COORD_AXIS)
    fprintf(out,"2D Axisymmetric\n");
  else if(grid->coordsystem == COORD_POLAR)
    fprintf(out,"2D Polar\n");
  else 
    fprintf(out,"Cartesian %dD\n",dim);
 
  fprintf(out,"Subcell Divisions in %dD = ",dim);
  if(dim >= 1) fprintf(out,"%d ",grid->xcells);
  if(dim >= 2) fprintf(out,"%d ",grid->ycells);
  if(dim >= 3) fprintf(out,"%d ",grid->zcells);
  fprintf(out,"\n");

  fprintf(out,"Subcell Limits 1 %s",sameline ? "= ":"\n  ");
  for(i=0;i <= grid->xcells;i++) 
    fprintf(out,"%.5g ",grid->x[i]); 
  fprintf(out,"\n");
    
  if(dim >= 2) {
    fprintf(out,"Subcell Limits 2 %s",sameline ? "= ":"\n  ");
    for(i=0;i <= grid->ycells;i++) 
      fprintf(out,"%.5g ",grid->y[i]); 
    fprintf(out,"\n");
  }
  
  if(dim >= 3) {
    fprintf(out,"Subcell Limits 3 %s",sameline ? "= ":"\n  ");
    for(i=0;i <= grid->zcells;i++) 
      fprintf(out,"%.5g ",grid->z[i]); 
    fprintf(out,"\n");
  }  

  fprintf(out,"Material Structure in %dD\n",dim==1 ? 1:2);  
  for(j=grid->ycells;j>=1;j--) {
    fprintf(out,"  ");
    for(i=1;i<=grid->xcells;i++) 
      fprintf(out,"%-5d",grid->structure[j][i]);
    fprintf(out,"\n");
  }
  fprintf(out,"End\n");

  if(grid->mappings > 0) {
    fprintf(out,"Geometry Mappings\n");
    fprintf(out,"# mode  line  limits(2)   Np  params(Np)\n");
    for(i=0;i<grid->mappings;i++) {
      fprintf(out,"  %-5d %-5d %-7.5g %-7.5g %-3d ",
	      grid->mappingtype[i],grid->mappingline[i],
	      grid->mappinglimits[2*i],grid->mappinglimits[2*i+1],
	      grid->mappingpoints[i]);
      for(j=0;j<grid->mappingpoints[i];j++) 
	fprintf(out,"%.4g ",grid->mappingparams[i][j]);
      fprintf(out,"\n");
    }
    fprintf(out,"End\n");
  }

  j = 0;
  if(grid[j].rotate) {
    fprintf(out,"Revolve Blocks = %d\n",grid[j].rotateblocks);
    fprintf(out,"Revolve Radius = %-8.3g\n",grid[j].rotateradius2);
    if(fabs(grid[j].rotateimprove-1.0) > 1.0e-10)
      fprintf(out,"Revolve Improve = %-8.3g\n",grid[j].rotateimprove);
    
  }
  if(grid[j].rotatecurve) {
    fprintf(out,"Revolve Curve Direct = %-8.3g\n",grid[j].curvezet);
    fprintf(out,"Revolve Curve Radius = %-8.3g\n",grid[j].curverad);
    fprintf(out,"Revolve Curve Angle = %-8.3g\n",grid[j].curveangle);
  }

  if(grid[j].coordsystem == COORD_POLAR) {
    fprintf(out,"Polar Radius = %.3g\n",grid[j].polarradius);
  } 

  for(j=0;j<nogrids;j++) {
    
    if(j>0) fprintf(out,"\nStart New Mesh\n");
  
    fprintf(out,"Materials Interval = %d %d\n",
	    grid[j].firstmaterial,grid[j].lastmaterial);
  
    if(dim == 3) {
      fprintf(out,"Extruded Structure\n");
      fprintf(out,"# %-8s %-8s %-8s\n","1stmat", "lastmat","newmat");
      for(i=1;i<=grid[j].zcells;i++) 
	fprintf(out,"  %-8d %-8d %-8d\n",
		grid[j].zfirstmaterial[i],grid[j].zlastmaterial[i],
		grid[j].zmaterial[i]); 
      fprintf(out,"End\n");    
    }

    if(grid[j].noboundaries > 0) {
      fprintf(out,"Boundary Definitions\n");
      fprintf(out,"# %-8s %-8s %-8s\n","type","out","int"); 
      for(i=0;i<grid[j].noboundaries;i++)
	fprintf(out,"  %-8d %-8d %-8d %-8d\n",
		grid[j].boundtype[i],grid[j].boundext[i],
		grid[j].boundint[i], grid[j].boundsolid[i]);
      fprintf(out,"End\n");
    }

    if(grid->numbering == NUMBER_XY)
      fprintf(out,"Numbering = Horizontal\n");
    if(grid->numbering == NUMBER_YX)
      fprintf(out,"Numbering = Vertical\n");
        
    fprintf(out,"Element Degree = %d\n",grid[j].elemorder);
    fprintf(out,"Element Innernodes = %s\n",grid[j].elemmidpoints ? "True" : "False");
    fprintf(out,"Triangles = %s\n",grid[j].triangles ? "True" : "False");
    if(grid[j].autoratio) 
      fprintf(out,"Surface Elements = %d\n",grid[j].wantedelems);
    if(dim == 3 && grid[j].wantedelems3d) 
      fprintf(out,"Volume Elements = %d\n",grid[j].wantedelems3d);
    if(dim == 3 && grid[j].wantednodes3d) 
      fprintf(out,"Volume Nodes = %d\n",grid[j].wantednodes3d);

    if(dim == 2)
      fprintf(out,"Coordinate Ratios = %-8.3g\n",grid[j].xyratio);
    if(dim == 3)
      fprintf(out,"Coordinate Ratios = %-8.3g %-8.3g\n",
	      grid[j].xyratio,grid[j].xzratio);
 
    fprintf(out,"Minimum Element Divisions = %d",grid[j].minxelems);
    if(dim >= 2) fprintf(out," %d",grid[j].minyelems);
    if(dim >= 3) fprintf(out," %d",grid[j].minzelems);
    fprintf(out,"\n");

    fprintf(out,"Element Ratios 1 %s",sameline ? "= ":"\n  ");
    for(i=1;i<=grid[j].xcells;i++) 
      fprintf(out,"%.3g ",grid[j].xexpand[i]); 
    fprintf(out,"\n");
    if(dim >= 2) {
      fprintf(out,"Element Ratios 2 %s",sameline ? "= ":"\n  ");
      for(i=1;i<=grid[j].ycells;i++) 
	fprintf(out,"%.3g ",grid[j].yexpand[i]); 
      fprintf(out,"\n");
    }
    if(dim >= 3) {
      fprintf(out,"Element Ratios 3 %s",sameline ? "= ":"\n  ");
      for(i=1;i<=grid[j].zcells;i++) 
	fprintf(out,"%.3g ",grid[j].zexpand[i]); 
      fprintf(out,"\n");
    }

    if(grid[j].autoratio) {
      fprintf(out,"Element Densities 1 %s",sameline ? "= ":"\n  ");
      for(i=1;i<=grid[j].xcells;i++) 
	fprintf(out,"%.3g ",grid[j].xdens[i]); 
      fprintf(out,"\n");
      if(dim >= 2) {
	fprintf(out,"Element Densities 2 %s",sameline ? "= ":"\n  ");
	for(i=1;i<=grid[j].ycells;i++) 
	  fprintf(out,"%.3g ",grid[j].ydens[i]); 
	fprintf(out,"\n");
      }
      if(dim >= 3) {       
	fprintf(out,"Element Densities 3 %s",sameline ? "= ":"\n  ");
	for(i=1;i<=grid[j].zcells;i++) 
	  fprintf(out,"%.3g ",grid[j].zdens[i]); 
	fprintf(out,"\n");
      }
    }
    else {
      fprintf(out,"Element Divisions 1 %s",sameline ? "= ":"\n  ");
      for(i=1;i<=grid[j].xcells;i++) 
	fprintf(out,"%d ",grid[j].xelems[i]); 
      fprintf(out,"\n");
      if(dim >= 2) {
	fprintf(out,"Element Divisions 2 %s",sameline ? "= ":"\n  ");
	for(i=1;i<=grid[j].ycells;i++) 
	  fprintf(out,"%d ",grid[j].yelems[i]); 
	fprintf(out,"\n");
      }
      if(dim >= 3) {       
	fprintf(out,"Element Divisions 3 %s",sameline ? "= ":"\n  ");
	for(i=1;i<=grid[j].zcells;i++) 
	  fprintf(out,"%d ",grid[j].zelems[i]); 
	fprintf(out,"\n");
      }
    }
    

  }

  if(info) printf("The Elmergrid input was saved to file %s.\n",filename);
  fclose(out);

  return(0);
}




int LoadElmergridOld(struct GridType **grid,int *nogrids,char *prefix,int info) 
{
  char filename[MAXFILESIZE];
  FILE *in;
  int i,j,k,l,error=0;
  Real scaling;
  char *cp;
  int mode,noknots,noelements,dim,axisymmetric;
  int elemcode,maxnodes,totelems,nogrids0;
  int minmat,maxmat;
  char line[MAXLINESIZE];


  AddExtension(prefix,filename,"grd");
  if ((in = fopen(filename,"r")) == NULL) {
    printf("LoadElmergrid: opening of the file '%s' wasn't succesfull !\n",filename);
    return(1);
  }

  if(info) printf("Loading the geometry from file '%s'.\n",filename);

  InitGrid(grid[*nogrids]);
  k = *nogrids;
  nogrids0 = *nogrids;

  mode = 0;
  noknots = 0;
  noelements = 0;
  dim = 0;
  axisymmetric = FALSE;
  elemcode = 0;
  maxnodes = 4;
  totelems = 0;
  scaling = 1.0;



  Getline(line,in);
  for(;;) {
    if(Getline(line,in)) goto end;
    if(line[0]=='\0') goto end;

    if(strstr(line,"END")) goto end;
    if(strstr(line,"RESULTS")) goto end;

    /* Control information */
    if(strstr(line,"VERSION")) mode = 1;
    else if(strstr(line,"GEOMETRY")) mode = 2;
    else if(strstr(line,"MAPPINGS IN")) mode = 31;
    else if(strstr(line,"MAPPINGS OUT")) mode = 32;
    else if(strstr(line,"MAPPINGS")) mode = 3;
    else if(strstr(line,"NUMBERING")) mode = 4;
    else if(strstr(line,"MESHING")) mode = 5;
    else if(strstr(line,"ELEMENTS")) mode = 6;
    else if(strstr(line,"ELEMENT NUMBER")) mode = 29;
    else if(strstr(line,"NODES")) mode = 7;
    else if(strstr(line,"TRIANGLE")) mode = 8;
    else if(strstr(line,"SQUARE")) mode = 17;
    else if(strstr(line,"COORDINATE RATIO"))  mode = 10;
    else if(strstr(line,"MATERIALS")) mode = 11;
    else if(strstr(line,"LAYERED ST")) mode = 12;
    else if(strstr(line,"ELEMENT RAT")) mode = 13;
    else if(strstr(line,"ELEMENT DENS")) mode = 14;
    else if(strstr(line,"ELEMENT MINIMUM")) mode = 27;
    else if(strstr(line,"BOUNDARY COND")) mode = 15;
    else if(strstr(line,"ELEMENTTYPE") || strstr(line,"ELEMENTCODE")) mode = 16;
    else if(strstr(line,"ROTATE")) mode = 20;
    else if(strstr(line,"ROTRAD")) mode = 21;
    else if(strstr(line,"ROTBLOCK")) mode = 22;
    else if(strstr(line,"ROTIMP")) mode = 24;
    else if(strstr(line,"ROTCURVE")) mode = 25;
    else if(strstr(line,"REDUCE ELEMENT")) mode = 26;
    else if(strstr(line,"SCALING")) mode = 23;
    else if(strstr(line,"LAYERED BO")) mode = 28;
    else if(strstr(line,"POLAR RADIUS")) mode = 30;


    switch (mode) {
    case 1: 
      printf("Loading Elmergrid file: %s\n",line);
      mode = 0;
      break;
      
    case 2:
      grid[k]->dimension = 2;
      if(strstr(line,"CARTES") && strstr(line,"1D")) {
	grid[k]->coordsystem = COORD_CART1;
	grid[k]->dimension = 1;
      }
      else if(strstr(line,"CARTES") && strstr(line,"2D")) 
	grid[k]->coordsystem = COORD_CART2;
      else if(strstr(line,"AXIS") && strstr(line,"2D")) 
	grid[k]->coordsystem = COORD_AXIS;
      else if(strstr(line,"POLAR") && strstr(line,"2D")) 
	grid[k]->coordsystem = COORD_POLAR;
      else if(strstr(line,"CARTES") && strstr(line,"3D")) {
	grid[k]->coordsystem = COORD_CART3;
	grid[k]->dimension = 3;
      }
      else if(strstr(line,"CYLINDRICAL")) {
	grid[k]->coordsystem = COORD_CYL;
	grid[k]->dimension = 3;
      }
      else printf("Unknown coordinate system: %s\n",line);
      printf("Defining the coordinate system (%d-DIM).\n",grid[k]->dimension);

      Getline(line,in);

      if(grid[k]->dimension == 1) {
	sscanf(line,"%d",&(*grid)[k].xcells);
	grid[k]->ycells = 1;	
      }
      if(grid[k]->dimension == 2) 
	sscanf(line,"%d %d",&(*grid)[k].xcells,&(*grid)[k].ycells);
      if(grid[k]->dimension == 3) 
	sscanf(line,"%d %d %d",&(*grid)[k].xcells,&(*grid)[k].ycells,&(*grid)[k].zcells);      
      if(grid[k]->xcells >= MAXCELLS || grid[k]->ycells >= MAXCELLS || 
	 grid[k]->zcells >= MAXCELLS) {
	printf("LoadGrid: Too many subcells [%d %d %d] vs. %d:\n",
	       grid[k]->xcells,grid[k]->ycells,grid[k]->zcells,MAXCELLS);
      }
      
      if(grid[k]->dimension == 1) {
	printf("Loading [%d] subcell intervals in 1D\n",
	       grid[k]->xcells);
      }
      else if(grid[k]->dimension == 2) {
	printf("Loading [%d %d] subcell intervals in 2D\n",
	       grid[k]->xcells,grid[k]->ycells);   
      } else {
	printf("Loading [%d %d %d] subcell intervals in 3D\n",
	       grid[k]->xcells,grid[k]->ycells,grid[k]->zcells);   
      }


      for(j=1;j<=grid[k]->dimension;j++) {
	Getline(line,in);
	cp=line;

	if(j==1) for(i=0;i<=grid[k]->xcells;i++) grid[k]->x[i] = next_real(&cp);
	if(j==2) for(i=0;i<=grid[k]->ycells;i++) grid[k]->y[i] = next_real(&cp);
	if(j==3) for(i=0;i<=grid[k]->zcells;i++) grid[k]->z[i] = next_real(&cp);
      }

      printf("Loading material structure\n");

      for(j=grid[k]->ycells;j>=1;j--) {
	
	Getline(line,in);
	cp=line;
	
	for(i=1;i<=grid[k]->xcells;i++) 
	  grid[k]->structure[j][i] = next_int(&cp);
      }

      minmat = maxmat = grid[k]->structure[1][1];
      for(j=grid[k]->ycells;j>=1;j--) 
	for(i=1;i<=grid[k]->xcells;i++) {
	  if(minmat > grid[k]->structure[j][i])
	    minmat = grid[k]->structure[j][i];
	  if(maxmat < grid[k]->structure[j][i])
	    maxmat = grid[k]->structure[j][i];
	}      
      if(minmat < 0) 
	printf("LoadElmergrid: please use positive material indices.\n");

      mode = 0;
      break;

    case 3:
    case 31:
    case 32:

      /* I dont know how to set this, luckily this piece of code should be obsolite */
      l = 1;
      for(i=grid[k]->mappings;i<grid[k]->mappings+l;i++) {
	Getline(line,in);
	cp=line; 

	grid[k]->mappingtype[i] = next_int(&cp);
	if(mode == 32) grid[k]->mappingtype[i] += 50*SGN(grid[k]->mappingtype[i]);

	grid[k]->mappingline[i] = next_int(&cp);
	grid[k]->mappinglimits[2*i] = next_real(&cp);
	grid[k]->mappinglimits[2*i+1] = next_real(&cp);
	grid[k]->mappingpoints[i] = next_int(&cp);
	grid[k]->mappingparams[i] = Rvector(0,grid[k]->mappingpoints[i]);
	for(j=0;j<grid[k]->mappingpoints[i];j++) 
	  grid[k]->mappingparams[i][j] = next_real(&cp);
      }
      
      printf("Loaded %d geometry mappings\n",l);
      grid[k]->mappings += l;

      mode = 0;
      break;
      
    case 4: /* NUMBERING */
      if(strstr(line,"HORIZ")) grid[k]->numbering = NUMBER_XY;
      if(strstr(line,"VERTI")) grid[k]->numbering = NUMBER_YX;
      mode = 0;
      break;

    case 5: /* MESHING */
      if((*nogrids) >= MAXCASES) {
	printf("There are more grids than was allocated for!\n"); 
	printf("Ignoring meshes starting from %d\n.",(*nogrids)+1);
	goto end;
      }
      (*nogrids)++;
      printf("Loading element meshing no %d\n",*nogrids);
      k = *nogrids - 1;	           
      if(k > nogrids0) (*grid)[k] = (*grid)[k-1];	 
      mode = 0;
      break;

    case 6: /* ELEMENTS */
      sscanf(line,"%d",&(*grid)[k].wantedelems);
      mode = 0;
      break;

    case 7: /* NODES */
      sscanf(line,"%d",&(*grid)[k].nonodes);      
      
      (*grid)[k].elemmidpoints = FALSE;
      if((*grid)[k].nonodes == 4) 
	(*grid)[k].elemorder = 1;
      if((*grid)[k].nonodes == 8) 
	(*grid)[k].elemorder = 2;
      if((*grid)[k].nonodes == 16) 
	(*grid)[k].elemorder = 3;

      if((*grid)[k].nonodes == 9) { 
	(*grid)[k].elemorder = 2;
	(*grid)[k].elemmidpoints = TRUE;
      }
      if((*grid)[k].nonodes == 12) { 
	(*grid)[k].elemorder = 3;
	(*grid)[k].elemmidpoints = TRUE;
      }


      mode = 0;
      break;

    case 8: /* TRIANGLES */
      (*grid)[k].triangles = TRUE;
      mode = 0;
      break;

    case 17: /* SQUARES */
      (*grid)[k].triangles = FALSE;
      mode = 0;
      break;

    case 16: /* ELEMENTTYPE and ELEMENTCODE */
      sscanf(line,"%d",&elemcode);
      if(elemcode/100 == 2) {
	(*grid)[k].triangles = FALSE;      
	(*grid)[k].nonodes = elemcode%100;
      }
      else if(elemcode/100 == 4) {
	(*grid)[k].triangles = FALSE;      
	(*grid)[k].nonodes = elemcode%100;
      }
      else if(elemcode/100 == 3) {  
	(*grid)[k].triangles = TRUE;      
	if(elemcode%100 == 3)       (*grid)[k].nonodes = 4;
	else if(elemcode%100 == 6)  (*grid)[k].nonodes = 9;
	else if(elemcode%100 == 10) (*grid)[k].nonodes = 16;	
      }

      (*grid)[k].elemmidpoints = FALSE;
      if((*grid)[k].nonodes == 4) 
	(*grid)[k].elemorder = 1;
      if((*grid)[k].nonodes == 8) 
	(*grid)[k].elemorder = 2;
      if((*grid)[k].nonodes == 16) 
	(*grid)[k].elemorder = 3;

      if((*grid)[k].nonodes == 9) { 
	(*grid)[k].elemorder = 2;
	(*grid)[k].elemmidpoints = TRUE;
      }
      if((*grid)[k].nonodes == 12) { 
	(*grid)[k].elemorder = 3;
	(*grid)[k].elemmidpoints = TRUE;
      }

      mode = 0;
      break;

    case 10: /* COORDINATE RATIO */
      if((*grid)[k].dimension == 2) 
	sscanf(line,"%le",&(*grid)[k].xyratio);
      if((*grid)[k].dimension == 3) 
	sscanf(line,"%le %le",&(*grid)[k].xyratio,&(*grid)[k].xzratio);      
      mode = 0;
      break;

    case 11: /* MATERIALS */
      sscanf(line,"%d %d",&(*grid)[k].firstmaterial,&(*grid)[k].lastmaterial);      
      mode = 0;
      break;

    case 12: /* LAYERES */
      for(i=1;i<=(*grid)[k].zcells;i++) {
	Getline(line,in);
	sscanf(line,"%d %d %d\n",
		&(*grid)[k].zfirstmaterial[i],&(*grid)[k].zlastmaterial[i],&(*grid)[k].zmaterial[i]); 
      }
      mode = 0;
      break;

    case 13: /* ELEMENT RATIOS */
      printf("Loading element ratios\n");

      for (j=1;j<=(*grid)[k].dimension;j++) {
	Getline(line,in);
	cp = line;

	if(j==1) for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xexpand[i] = next_real(&cp);
	if(j==2) for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].yexpand[i] = next_real(&cp);
	if(j==3) for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zexpand[i] = next_real(&cp);
      }
      mode = 0;
      break;

    case 29: /* ELEMENT NUMBER */
      printf("Loading element numbers\n");

      for (j=1;j<=(*grid)[k].dimension;j++) {
	Getline(line,in);
	cp = line;
	if(j==1) for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xelems[i] = next_int(&cp);
	if(j==2) for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].yelems[i] = next_int(&cp);
	if(j==3) for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zelems[i] = next_int(&cp);
      }
      (*grid)[k].autoratio = 0;
      mode = 0;
      break;

    case 27: /* ELEMENT MINIMUM */
      printf("Loading minimum number of elements\n");
      if((*grid)[k].dimension == 1) 
	sscanf(line,"%d",&(*grid)[k].minxelems);
      if((*grid)[k].dimension == 2) 
	sscanf(line,"%d %d",&(*grid)[k].minxelems,&(*grid)[k].minyelems);
      if((*grid)[k].dimension == 3) 
	sscanf(line,"%d %d %d",&(*grid)[k].minxelems,&(*grid)[k].minyelems,&(*grid)[k].minzelems);
      mode = 0;
      break;

    case 14: /* ELEMENT DENSITIES */
      printf("Loading element densities\n");
      for (j=1;j<=(*grid)[k].dimension;j++) {
	Getline(line,in);
	cp = line;

	if(j==1) for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xdens[i] = next_real(&cp);
	if(j==2) for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].ydens[i] = next_real(&cp);
	if(j==3) for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zdens[i] = next_real(&cp);
      }
      mode = 0;
      break;

    case 15: /* BOUNDARY CONDITIONS */
      sscanf(line,"%d",&(*grid)[k].noboundaries);
      printf("Loading %d boundary conditions\n",(*grid)[k].noboundaries);

      for(i=0;i<(*grid)[k].noboundaries;i++) {
	Getline(line,in);
	sscanf(line,"%d %d %d %d",
	       &(*grid)[k].boundtype[i],&(*grid)[k].boundext[i],
	       &(*grid)[k].boundint[i],&(*grid)[k].boundsolid[i]);
      }  
      mode = 0;
      break;

    case 20: /* ROTATE */
      (*grid)[k].rotate = TRUE;
      mode = 0;
      break;

    case 21: /* ROTRAD */
      sscanf(line,"%le",&(*grid)[k].rotateradius2);
      mode = 0;
      break;

    case 22: /* ROTBLOCK */
      sscanf(line,"%d",&(*grid)[k].rotateblocks);
      if(0) printf("Reading blocks %d\n",(*grid)[k].rotateblocks);
      mode = 0;
      break;

    case 24: /* ROTIMP */
      sscanf(line,"%le",&(*grid)[k].rotateimprove);
      mode = 0;
      break;

    case 30: /* POLAR RADIUS */
      sscanf(line,"%le",&(*grid)[k].polarradius);
      mode = 0;
      break;

    case 25: /* ROTCURVE */
      (*grid)[k].rotatecurve = TRUE;
      sscanf(line,"%le%le%le",&(*grid)[k].curvezet,
	     &(*grid)[k].curverad,&(*grid)[k].curveangle);
      mode = 0;
      break;

    case 26: /* REDUCE ELEMENT */
      sscanf(line,"%d%d",&(*grid)[k].reduceordermatmin,
	     &(*grid)[k].reduceordermatmax);
      mode = 0;
      break;

    case 28: /* LAYERED BO */
      sscanf(line,"%d",&(*grid)[k].layeredbc);
      mode = 0;
      break;

    case 23: /* SCALING */
      sscanf(line,"%le",&scaling);
      for(i=0;i<=(*grid)[k].xcells;i++) (*grid)[k].x[i] *= scaling;
      if((*grid)[k].dimension > 1) 
	for(i=0;i<=(*grid)[k].ycells;i++) (*grid)[k].y[i] *= scaling;
      if((*grid)[k].dimension == 3) 
	for(i=0;i<=(*grid)[k].ycells;i++) (*grid)[k].z[i] *= scaling;

      (*grid)[k].rotateradius2 *= scaling;
      (*grid)[k].curverad *= scaling;
      (*grid)[k].curvezet *= scaling;
      mode = 0;
      break;

    default:
      if(0) printf("Unknown case: %s",line);
    }

  }

end:

  if(info) printf("Found %d divisions for grid\n",*nogrids);

  for(k=nogrids0;k < (*nogrids) && k<MAXCASES;k++) {
    SetElementDivision(&(*grid)[k],1.0,info);
  }


  fclose(in);
  return(error);
}



int LoadElmergrid(struct GridType **grid,int *nogrids,char *prefix,Real relh,int info) 
{
  char filename[MAXFILESIZE];
  char command[MAXLINESIZE],params[MAXLINESIZE];
  FILE *in;
  int i,j,k;
  char *cp;
  int noknots,noelements,dim,axisymmetric;
  int elemcode,maxnodes,totelems,nogrids0,minmat,maxmat;
  long code;
  Real raid;

  AddExtension(prefix,filename,"grd");
  if ((in = fopen(filename,"r")) == NULL) {
    printf("LoadElmergrid: opening of the file '%s' wasn't succesfull !\n",filename);
    return(1);
  }

  if(info) printf("Loading the geometry from file '%s'.\n",filename);

  InitGrid(grid[*nogrids]);
  k = *nogrids;
  nogrids0 = *nogrids;

  iodebug = FALSE;

  noknots = 0;
  noelements = 0;
  dim = 0;
  axisymmetric = FALSE;
  elemcode = 0;
  maxnodes = 4;
  totelems = 0;

  matcactive = FALSE;

  for(;;) {
    if(GetCommand(command,params,in)) {
      printf("Reached the end of command file\n");
      goto end;
    }    

    /* Control information */
    if(strstr(command,"VERSION")) {
      if(strstr(command,"080500")) {
	printf("Loading old version of Elmergrid file.\n");
	i = LoadElmergridOld(grid,nogrids,prefix,info);
	return(i);
      }
      else {
	sscanf(params,"%ld",&code);
	if(code == 210903) 
	  printf("Loading ElmerGrid file version: %ld\n",code);
	else {
	  printf("Unknown ElmerGrid file version: %ld\n",code);
	  return(2);
	}
      }
      *nogrids += 1;
    }      

    else if(strstr(command,"DEBUG IO")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"FALSE")) 
	iodebug = FALSE;
      else {
	iodebug = TRUE;
	printf("IO debugging activated\n");
      }
    }
 
    else if(strstr(command,"MATC")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"FALSE")) 
	matcactive = FALSE;
      else {
#ifndef DISABLE_MATC
	matcactive = TRUE;
	mtc_init(NULL, stdout, stderr);
	strcpy(command, "format( 12 )");	
	mtc_domath(command);	 
	printf("MATC language activated with 12 digit accuracy.\n");	
#else
        matcactive = FALSE;
        printf("Unable to activate matc as it is not even compiled.\n");
#endif 
      }
    }
    
    else if(strstr(command,"COORDINATE SYSTEM")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      grid[k]->dimension = 2;
      if(strstr(params,"CARTESIAN 1D")) {
	grid[k]->coordsystem = COORD_CART1;
	grid[k]->dimension = 1;
      }
      else if(strstr(params,"CARTESIAN 2D")) 
	grid[k]->coordsystem = COORD_CART2;
      else if(strstr(params,"AXISYMMETRIC")) 
	grid[k]->coordsystem = COORD_AXIS;
      else if(strstr(params,"POLAR"))
	grid[k]->coordsystem = COORD_POLAR;
      else if(strstr(params,"CARTESIAN 3D")) {
	grid[k]->coordsystem = COORD_CART3;
	grid[k]->dimension = 3;
      }
      else printf("Unknown coordinate system: %s\n",params);
      printf("Defining the coordinate system (%d-DIM).\n",grid[k]->dimension);
    }
    
    else if(strstr(command,"SUBCELL DIVISIONS")) {
      if(grid[k]->dimension == 1) {
	sscanf(params,"%d",&(*grid)[k].xcells);
	grid[k]->ycells = 1;	
      }
      else if(grid[k]->dimension == 2) 
	sscanf(params,"%d %d",&(*grid)[k].xcells,&(*grid)[k].ycells);
      else if(grid[k]->dimension == 3) 
	sscanf(params,"%d %d %d",&(*grid)[k].xcells,&(*grid)[k].ycells,&(*grid)[k].zcells);      

      if(grid[k]->xcells >= MAXCELLS || grid[k]->ycells >= MAXCELLS || grid[k]->zcells >= MAXCELLS) {
	printf("LoadElmergrid: Too many subcells [%d %d %d] vs. %d:\n",
	       grid[k]->xcells,grid[k]->ycells,grid[k]->zcells,MAXCELLS);
     }

      /* Initialize the default stucture with ones */
      for(j=grid[k]->ycells;j>=1;j--) 
	for(i=1;i<=grid[k]->xcells;i++) 
	  grid[k]->structure[j][i] = 1;
    }
    
    else if(strstr(command,"MINIMUM ELEMENT DIVISION")) {
      printf("Loading minimum number of elements\n");

      if((*grid)[k].dimension == 1) 
	sscanf(params,"%d",&(*grid)[k].minxelems); 

      if((*grid)[k].dimension == 2)  
	sscanf(params,"%d %d",&(*grid)[k].minxelems,&(*grid)[k].minyelems);

      if((*grid)[k].dimension == 3) 
	sscanf(params,"%d %d %d",&(*grid)[k].minxelems,
	       &(*grid)[k].minyelems,&(*grid)[k].minzelems);
    }      
    
    else if(strstr(command,"SUBCELL LIMITS 1")) {
      printf("Loading %d subcell limits in X-direction\n",grid[k]->xcells+1);
      cp = params;
      for(i=0;i<=grid[k]->xcells;i++) {
	grid[k]->x[i] = next_real(&cp);
	if(i > 0 && grid[k]->x[i] < grid[k]->x[i-1]) {
	  printf("Subcell limits 1(%d): %12.6le %12.6le\n",i,grid[k]->x[i],grid[k]->x[i-1]);
	  bigerror("Values for limits 1 should be a growing series, existing\n");
	}
      }
    }    
    else if(strstr(command,"SUBCELL LIMITS 2")) {
      printf("Loading %d subcell limits in Y-direction\n",grid[k]->ycells+1);
      cp = params;
      for(i=0;i<=grid[k]->ycells;i++) {
	grid[k]->y[i] = next_real(&cp);
	if(i > 0 && grid[k]->y[i] < grid[k]->y[i-1]) {
	  printf("Subcell limits 2(%d): %12.6le %12.6le\n",i,grid[k]->y[i],grid[k]->y[i-1]);
	  bigerror("Values for limits should be a growing series, existing\n");
	}
      }
    }      
    else if(strstr(command,"SUBCELL LIMITS 3")) {
      printf("Loading %d subcell limits in Z-direction\n",grid[k]->zcells+1);
      cp = params;
      for(i=0;i<=grid[k]->zcells;i++) {
	grid[k]->z[i] = next_real(&cp);
	if(i > 0 && grid[k]->z[i] < grid[k]->z[i-1]) {
	  printf("Subcell limits 3(%d): %12.6le %12.6le\n",i,grid[k]->z[i],grid[k]->z[i-1]);
	  bigerror("Values for limits should be a growing series, existing\n");
	}
      }
    }

    else if(strstr(command,"SUBCELL SIZES 1")) {
      printf("Loading %d subcell sizes in X-direction\n",grid[k]->xcells);
      cp = params;
      for(i=1;i<=grid[k]->xcells;i++) grid[k]->x[i] = next_real(&cp);
      for(i=1;i<=grid[k]->xcells;i++) grid[k]->x[i] = grid[k]->x[i-1] + grid[k]->x[i];
    }      
    else if(strstr(command,"SUBCELL SIZES 2")) {
      printf("Loading %d subcell sizes in Y-direction\n",grid[k]->ycells);
      cp = params;
      for(i=1;i<=grid[k]->ycells;i++) grid[k]->y[i] = next_real(&cp);
      for(i=1;i<=grid[k]->ycells;i++) grid[k]->y[i] = grid[k]->y[i-1] + grid[k]->y[i];
    }      
    else if(strstr(command,"SUBCELL SIZES 3")) {
      printf("Loading %d subcell sizes in Z-direction\n",grid[k]->zcells);
      cp = params;
      for(i=1;i<=grid[k]->zcells;i++) grid[k]->z[i] = next_real(&cp);
      for(i=1;i<=grid[k]->zcells;i++) grid[k]->z[i] = grid[k]->z[i-1] + grid[k]->z[i];
    }

    else if(strstr(command,"SUBCELL ORIGIN 1")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"CENTER")) {
	raid = 0.5 * (grid[k]->x[0] + grid[k]->x[grid[k]->xcells]);
      }
      else if(strstr(params,"LEFT") || strstr(params,"MIN") ) {
	raid = grid[k]->x[0];
      }
      else if(strstr(params,"RIGHT") || strstr(params,"MAX") ) {
	raid = grid[k]->x[grid[k]->xcells];
      }
      else {
	cp = params;
	raid = next_real(&cp);
      }
      for(i=0;i<=grid[k]->xcells;i++) grid[k]->x[i] -= raid;
    }
    else if(strstr(command,"SUBCELL ORIGIN 2")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"CENTER")) {
	raid = 0.5 * (grid[k]->y[0] + grid[k]->y[grid[k]->ycells]);
      }
      else if(strstr(params,"LEFT")) {
	raid = grid[k]->y[0];
      }
      else if(strstr(params,"RIGHT")) {
	raid = grid[k]->y[grid[k]->ycells];
      }
      else {
	cp = params;
	raid = next_real(&cp);
      }      
      for(i=0;i<=grid[k]->ycells;i++) grid[k]->y[i] -= raid;
    }
    else if(strstr(command,"SUBCELL ORIGIN 3")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"CENTER")) {
	raid = 0.5 * (grid[k]->z[0] + grid[k]->z[grid[k]->zcells]);
      }
      else if(strstr(params,"LEFT")) {
	raid = grid[k]->z[0];
      }
      else if(strstr(params,"RIGHT")) {
	raid = grid[k]->z[grid[k]->zcells];
      }
      else {
	cp = params;
	raid = next_real(&cp);
      }
      for(i=0;i<=grid[k]->zcells;i++) grid[k]->z[i] -= raid;      
    }

    else if(strstr(command,"MATERIAL STRUCTURE")) {
      printf("Loading material structure\n");

      /* Initialize the default stucture with zeros */
      for(j=grid[k]->ycells;j>=1;j--) 
	for(i=1;i<=grid[k]->xcells;i++) 
	  grid[k]->structure[j][i] = 0;
     
      for(j=grid[k]->ycells;j>=1;j--) {
	if(j < grid[k]->ycells) Getline(params,in);
	cp=params;
	for(i=1;i<=grid[k]->xcells;i++) 
	  grid[k]->structure[j][i] = next_int(&cp);
      }      
      minmat = maxmat = grid[k]->structure[1][1];
      for(j=grid[k]->ycells;j>=1;j--) 
	for(i=1;i<=grid[k]->xcells;i++) {
	  if(minmat > grid[k]->structure[j][i])
	    minmat = grid[k]->structure[j][i];
	  if(maxmat < grid[k]->structure[j][i])
	    maxmat = grid[k]->structure[j][i];
	}      
      if(minmat < 0) 
	printf("LoadElmergrid: please use positive material indices.\n");
      grid[k]->maxmaterial = maxmat;
    }
    else if(strstr(command,"MATERIALS INTERVAL")) {
      sscanf(params,"%d %d",&(*grid)[k].firstmaterial,&(*grid)[k].lastmaterial);      
    }
     
    else if(strstr(command,"REVOLVE")) {
      if(0) printf("revolve: %s %s\n",command,params);

      if(strstr(command,"REVOLVE RADIUS")) {
	(*grid)[k].rotate = TRUE;
	sscanf(params,"%le",&(*grid)[k].rotateradius2);
      }
      else if(strstr(command,"REVOLVE BLOCKS")) {
	(*grid)[k].rotate = TRUE;
	sscanf(params,"%d",&(*grid)[k].rotateblocks);
      }
      else if(strstr(command,"REVOLVE IMPROVE")) {
	(*grid)[k].rotate = TRUE;
	sscanf(params,"%le",&(*grid)[k].rotateimprove);
      }
      else if(strstr(command,"REVOLVE RADIUS")) {
	sscanf(params,"%le",&(*grid)[k].polarradius);
      }
      else if(strstr(command,"REVOLVE CURVE DIRECT")) {
	(*grid)[k].rotatecurve = TRUE;
	sscanf(params,"%le",&(*grid)[k].curvezet);
      }
      else if(strstr(command,"REVOLVE CURVE RADIUS")) {
	(*grid)[k].rotatecurve = TRUE;
	sscanf(params,"%le",&(*grid)[k].curverad);
      }
      else if(strstr(command,"REVOLVE CURVE ANGLE")) {
	(*grid)[k].rotatecurve = TRUE;
	sscanf(params,"%le",&(*grid)[k].curveangle);
      }
    }

    else if(strstr(command,"REDUCE ORDER INTERVAL")) {
      sscanf(params,"%d%d",&(*grid)[k].reduceordermatmin,
	     &(*grid)[k].reduceordermatmax);
    }
    
    else if(strstr(command,"BOUNDARY DEFINITION")) {
      printf("Loading boundary conditions\n");
      
      for(i=0;i<MAXBOUNDARIES;i++) {
	if(i>0) Getline(params,in);
	for(j=0;j<MAXLINESIZE;j++) params[j] = toupper(params[j]);
	if(strstr(params,"END")) break;
	sscanf(params,"%d %d %d %d",
	       &(*grid)[k].boundtype[i],&(*grid)[k].boundext[i],
	       &(*grid)[k].boundint[i],&(*grid)[k].boundsolid[i]);
      }  
      printf("Found %d boundaries\n",i);
      (*grid)[k].noboundaries = i;
    }
    
    /* else if(strstr(command,"LAYERED BOUNDARIES")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"TRUE")) (*grid)[k].layeredbc = 1;
      if(strstr(params,"FALSE")) (*grid)[k].layeredbc = 0;
    } */
    
    else if(strstr(command,"NUMBERING")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"HORIZONATAL")) (*grid)[k].numbering = NUMBER_XY;
      if(strstr(params,"VERTICAL")) (*grid)[k].numbering = NUMBER_YX;
    }
    
    else if(strstr(command,"ELEMENT DEGREE")) {
      sscanf(params,"%d",&(*grid)[k].elemorder);
    }
    
    else if(strstr(command,"ELEMENT INNERNODES")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"TRUE")) (*grid)[k].elemmidpoints = TRUE;
      if(strstr(params,"FALSE")) (*grid)[k].elemmidpoints = FALSE;
    }
    else if(strstr(command,"ELEMENTTYPE") || strstr(command,"ELEMENTCODE")) {
      sscanf(params,"%d",&elemcode);
    }
    
    else if(strstr(command,"TRIANGLES CRITICAL ANGLE")) {
      sscanf(params,"%le",&(*grid)[k].triangleangle);      
    }
    else if(strstr(command,"TRIANGLES")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"TRUE")) (*grid)[k].triangles = TRUE;
      if(strstr(params,"FALSE")) (*grid)[k].triangles = FALSE;
    }
    
    else if(strstr(command,"PLANE ELEMENTS")) {
      sscanf(params,"%d",&(*grid)[k].wantedelems);
    }
    else if(strstr(command,"SURFACE ELEMENTS")) {
      sscanf(params,"%d",&(*grid)[k].wantedelems);
    }
    else if(strstr(command,"REFERENCE DENSITY")) {
      sscanf(params,"%le",&(*grid)[k].limitdx);
      (*grid)[k].autoratio = 3;     
    }
    else if(strstr(command,"VERIFY DENSITY")) {
      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      if(strstr(params,"TRUE")) (*grid)[k].limitdxverify = TRUE;
      if(strstr(params,"FALSE")) (*grid)[k].limitdxverify = FALSE;
    }
    else if(strstr(command,"VOLUME ELEMENTS")) {
      sscanf(params,"%d",&(*grid)[k].wantedelems3d);
    }
    else if(strstr(command,"VOLUME NODES")) {
      sscanf(params,"%d",&(*grid)[k].wantednodes3d);
    }

    else if(strstr(command,"COORDINATE RATIO")) {
      if((*grid)[k].dimension == 2) 
	sscanf(params,"%le",&(*grid)[k].xyratio);
      if((*grid)[k].dimension == 3) 
	sscanf(params,"%le %le",&(*grid)[k].xyratio,&(*grid)[k].xzratio);      
    }
    
    else if(strstr(command,"ELEMENT RATIOS 1")) {
      cp = params;
      for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xexpand[i] = next_real(&cp);
    }
    else if(strstr(command,"ELEMENT RATIOS 2")) {
      cp = params;
      for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].yexpand[i] = next_real(&cp);
    }
    else if(strstr(command,"ELEMENT RATIOS 3")) {
      cp = params;
      for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zexpand[i] = next_real(&cp);
    }
    
    else if(strstr(command,"ELEMENT DENSITIES 1")) {
      cp = params;
      for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xdens[i] = next_real(&cp);
    }
    else if(strstr(command,"ELEMENT DENSITIES 2")) {
      cp = params;
      for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].ydens[i] = next_real(&cp);
    }
    else if(strstr(command,"ELEMENT DENSITIES 3")) {
      cp = params;
      for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zdens[i] = next_real(&cp);
    }
    
    else if(strstr(command,"ELEMENT DIVISIONS 1")) {
      cp = params;
      for(i=1;i<=(*grid)[k].xcells;i++) (*grid)[k].xelems[i] = next_int(&cp);
      (*grid)[k].autoratio = 0;
    }
    else if(strstr(command,"ELEMENT DIVISIONS 2")) {
      cp = params;
      for(i=1;i<=(*grid)[k].ycells;i++) (*grid)[k].yelems[i] = next_int(&cp);
      (*grid)[k].autoratio = 0;
    }
    else if(strstr(command,"ELEMENT DIVISIONS 3")) {
      cp = params;
      for(i=1;i<=(*grid)[k].zcells;i++) (*grid)[k].zelems[i] = next_int(&cp);
      (*grid)[k].autoratio = 0;
    }
    
    /* else if(strstr(command,"EXTRUDED STRUCTURE")) {
      for(i=1;i<=(*grid)[k].zcells;i++) {
	if(i>1) Getline(params,in);
	sscanf(params,"%d %d %d\n",
	       &(*grid)[k].zfirstmaterial[i],&(*grid)[k].zlastmaterial[i],&(*grid)[k].zmaterial[i]); 
      }
    } */
    
    else if(strstr(command,"GEOMETRY MAPPINGS")) {     
      if(k > 0) (*grid)[k].mappings = 0;

      for(i=0;i<MAXLINESIZE;i++) params[i] = toupper(params[i]);
      for(i=(*grid)[k].mappings;i<MAXMAPPINGS;i++) {
	if(i>(*grid)[k].mappings) Getline(params,in);

	if(strstr(params,"END")) break;
	cp=params; 
	(*grid)[k].mappingtype[i] = next_int(&cp);	
#if 0
	(*grid)[k].mappingtype[i] += 50*SGN((*grid)[k].mappingtype[i]);
#endif
	(*grid)[k].mappingline[i] = next_int(&cp);
	(*grid)[k].mappinglimits[2*i] = next_real(&cp);
	(*grid)[k].mappinglimits[2*i+1] = next_real(&cp);
	(*grid)[k].mappingpoints[i] = next_int(&cp);
	(*grid)[k].mappingparams[i] = Rvector(0,(*grid)[k].mappingpoints[i]);
	for(j=0;j<(*grid)[k].mappingpoints[i];j++) 
	  (*grid)[k].mappingparams[i][j] = next_real(&cp);
      }      
      printf("Loaded %d geometry mappings\n",i);
      (*grid)[k].mappings = i;      
    }

    else if(strstr(command,"END") ) {      
      if(0) printf("End of field\n");
    }
      
    else if(strstr(command,"START NEW MESH")) {
      if((*nogrids) >= MAXCASES) {
	printf("There are more grids than was allocated for!\n"); 
	printf("Ignoring meshes starting from %d\n.",(*nogrids)+1);
	goto end;
      }
      (*nogrids)++;
      printf("\nLoading element meshing no %d\n",*nogrids);
      k = *nogrids - 1;	           
      if(k > nogrids0) (*grid)[k] = (*grid)[k-1];	 
    }

    else {
      if(0) printf("Unknown command: %s",command);
    }
  }

end:

  if(info) printf("Found %d divisions for grid\n",*nogrids);
  
  for(k=nogrids0;k < (*nogrids) && k<MAXCASES;k++) {

    if(elemcode == 0) {
      if((*grid)[k].dimension == 1) {
	(*grid)[k].nonodes = (*grid)[k].elemorder + 1;
      }
      else if((*grid)[k].elemmidpoints == FALSE) {
	(*grid)[k].nonodes = 4 * (*grid)[k].elemorder;
      }					
      else {
	if((*grid)[k].elemorder == 2) (*grid)[k].nonodes = 9;
	if((*grid)[k].elemorder == 3) (*grid)[k].nonodes = 16;	
      }
    }
    else if(elemcode/100 == 2) {
      (*grid)[k].triangles = FALSE;      
      (*grid)[k].nonodes = elemcode%100;
    }
    else if(elemcode/100 == 4) {
      (*grid)[k].triangles = FALSE;      
      (*grid)[k].nonodes = elemcode%100;
    }
    else if(elemcode/100 == 3) {  
      (*grid)[k].triangles = TRUE;      
      if(elemcode%100 == 3)       (*grid)[k].nonodes = 4;
      else if(elemcode%100 == 6)  (*grid)[k].nonodes = 9;
      else if(elemcode%100 == 10) (*grid)[k].nonodes = 16;	
    }    
  }

  
  for(k=nogrids0;k < (*nogrids) && k<MAXCASES;k++) {
    SetElementDivision(&(*grid)[k],relh,info);
  }

  fclose(in);
  return(0);
}


int ShowCorners(struct FemType *data,int variable,Real offset)
{
  int i,ind,unknowns;
  Real *solution;

  if(data->nocorners < 1) 
    return(1);

  unknowns = data->edofs[variable];
  if(unknowns == 0) return(2);

  solution = data->dofs[variable];

  printf("Variable %s at free corners:\n",data->dofname[variable]);
  for(i=1;i<=data->nocorners;i++) {
    ind = data->topology[data->corners[2*i-1]][data->corners[2*i]];
    if(data->order[variable][(ind-1)*unknowns+1])
       printf("\t%-2d: %-7.1f  at (%.1f , %.1f )\n",
	      i,solution[(ind-1)*unknowns+1]-offset,
	      data->x[ind],data->y[ind]);
  }
  return(0);
}




