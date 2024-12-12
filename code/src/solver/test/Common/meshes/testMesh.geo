// Gmsh project created on Fri Sep 27 10:24:00 2024
SetFactory("OpenCASCADE");
//+
Point(1) = {1, 0, 0, 1.0};
//+
Point(2) = {2, 0, 0, 1.0};
//+
Point(3) = {0, 1, 0, 1.0};
//+
Point(4) = {1, 1, 0, 1.0};
//+
Point(5) = {2, 1, 0, 1.0};
//+
Point(6) = {3, 1, 0, 1.0};
//+
Point(7) = {4, 1, 0, 1.0};
//+
Recursive Delete {
  Point{7}; 
}
//+
Point(7) = {0, 2, 0, 1.0};
//+
Point(8) = {1, 2, 0, 1.0};
//+
Point(9) = {2, 2, 0, 1.0};
//+
Point(10) = {3, 2, 0, 1.0};
//+
Point(11) = {1, 3, 0, 1.0};
//+
Point(12) = {2, 3, 0, 1.0};
//+
Line(1) = {3, 4};
//+
Line(2) = {4, 1};
//+
Line(3) = {1, 2};
//+
Line(4) = {2, 5};
//+
Line(5) = {5, 6};
//+
Line(6) = {6, 10};
//+
Line(7) = {10, 9};
//+
Line(8) = {9, 12};
//+
Line(9) = {12, 11};
//+
Line(10) = {11, 8};
//+
Line(11) = {8, 7};
//+
Line(12) = {7, 3};
//+
Physical Curve("lower", 13) = {3, 2, 1, 4, 5, 6, 7, 8, 12, 11, 10};
//+
Physical Curve("upper", 14) = {9};
//+
Curve Loop(1) = {11, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//+
Plane Surface(1) = {1};
//+
Physical Surface("fluid", 15) = {1};
//+
Physical Surface("fluid", 15) += {1};
//+
Physical Surface("fluid", 15) += {1};
