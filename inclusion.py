#!/usr/bin/env python
import math

# routine for performing the "point in polygon" inclusion test

# Copyright 2001, softSurfer (www.softsurfer.com)
# This code may be freely used and modified for any purpose
# providing that this copyright notice is included with it.
# SoftSurfer makes no warranty for this code, and cannot be held
# liable for any real or imagined damage resulting from its use.
# Users of this code must verify correctness for their application.

# translated to Python by Maciej Kalisiak <mac@dgp.toronto.edu>

#   a Point is represented as a tuple: (x,y)

#===================================================================

# is_left(): tests if a point is Left|On|Right of an infinite line.

#   Input: three points P0, P1, and P2
#   Return: >0 for P2 left of the line through P0 and P1
#           =0 for P2 on the line
#           <0 for P2 right of the line
#   See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"

def is_left(P0, P1, P2):
    return (P1[0] - P0[0]) * (P2[1] - P0[1]) - (P2[0] - P0[0]) * (P1[1] - P0[1])

#===================================================================

# cn_PnPoly(): crossing number test for a point in a polygon
#     Input:  P = a point,
#             V[] = vertex points of a polygon
#     Return: 0 = outside, 1 = inside
# This code is patterned after [Franklin, 2000]

def in_polygon(P, V):
    cn = 0    # the crossing number counter

    # repeat the first vertex at end
    V = tuple(V[:])+(V[0],)

    # loop through all edges of the polygon
    for i in range(len(V)-1):   # edge from V[i] to V[i+1]
      if ((V[i][1] <= P[1] and V[i+1][1] > P[1])   # an upward crossing
        or (V[i][1] > P[1] and V[i+1][1] <= P[1])):  # a downward crossing
        # compute the actual edge-ray intersect x-coordinate
        vt = (P[1] - V[i][1]) / float(V[i+1][1] - V[i][1])
        if P[0] < V[i][0] + vt * (V[i+1][0] - V[i][0]): # P[0] < intersect
            cn += 1  # a valid crossing of y=P[1] right of P[0]

    return cn % 2   # 0 if even (out), and 1 if odd (in)


def point_to_line(P0, P1, P2):
  (x0,y0) = P0
  (x1,y1) = P1
  (x2,y2) = P2

  return abs((y2-y1)*x0 - (x2-x1)*y0 + x2*y1 - y2*x1) / math.sqrt(math.pow(y2-y1, 2) + math.pow(x2-x1, 2))


def point_to_point(P1, P2):
  (x1,y1) = P1
  (x2,y2) = P2

  return math.sqrt(math.pow(y2-y1, 2) + math.pow(x2-x1, 2))
