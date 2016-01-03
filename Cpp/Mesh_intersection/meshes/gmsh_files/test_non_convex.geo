cl__1 = 1e+22;
Point(1) = {0, 0, 0, 1e+22};
Point(2) = {1, 0, 0, 1e+22};
Point(3) = {1, 1, 0, 1e+22};
Point(4) = {0.5, 1, 0, 1e+22};
Point(5) = {0.5, 0.5, 0, 1e+22};
Point(6) = {0, 0.5, 0, 1e+22};
Point(7) = {0, 0, 0.5, 1e+22};
Point(8) = {1, 0, 0.5, 1e+22};
Point(12) = {1, 1, 0.5, 1e+22};
Point(16) = {0.5, 1, 0.5, 1e+22};
Point(20) = {0.5, 0.5, 0.5, 1e+22};
Point(24) = {0, 0.5, 0.5, 1e+22};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 1};
Transfinite Line {1,2} = 3Using Progression 1;
Transfinite Line {3,4,5,6} = 2Using Progression 1;

Line(8) = {7, 8};
Line(9) = {8, 12};
Line(10) = {12, 16};
Line(11) = {16, 20};
Line(12) = {20, 24};
Line(13) = {24, 7};
Transfinite Line {8,9} = 3Using Progression 1;
Transfinite Line {10,11,12,13} = 2Using Progression 1;

Line(15) = {1, 7};
Line(16) = {2, 8};
Line(20) = {3, 12};
Line(24) = {4, 16};
Line(28) = {5, 20};
Line(32) = {6, 24};
Transfinite Line {15,16,20,24,28,32} = 2Using Progression 1;

Line Loop(1) = {1, 2, 3, 4, 5, 6};
Plane Surface(1) = {1};
Line Loop(17) = {1, 16, -8, -15};
Ruled Surface(17) = {17};
Line Loop(21) = {2, 20, -9, -16};
Ruled Surface(21) = {21};
Line Loop(25) = {3, 24, -10, -20};
Ruled Surface(25) = {25};
Line Loop(29) = {4, 28, -11, -24};
Ruled Surface(29) = {29};
Line Loop(33) = {5, 32, -12, -28};
Ruled Surface(33) = {33};
Line Loop(37) = {6, 15, -13, -32};
Ruled Surface(37) = {37};
Line Loop(38) = {8, 9, 10, 11, 12, 13};
Plane Surface(38) = {38};
Surface Loop(1) = {1, 38, 17, 21, 25, 29, 33, 37};
Volume(1) = {1};
Physical Surface(1) = {1};
