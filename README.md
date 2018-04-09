# RobotPathFinding
This application to simulate the way how Robot find a path to reach its destination in Windows.

## For inputting test case to this project:
* Open input.txt file (located in AI folder)
* Format of input file:
  * n: determine the number of shape
  
  (n line following)
  * NumberOfEdges x1 y1 x2 y2...xn yn
  
  .
  
  .
  
  .
  
  * StartPosition (x, y)
  
  * EndPosition   (x, y)
  
* For example:

2                 // Two shapes

3 7 10 8 4 2 5    // Shape 1 with 3 edges (7, 10), (8, 4), (2, 5)

4 1 1 2 2 1 2 2 1 // Shape 1 with 4 edges (1, 1), (2, 2), (1, 2), (2, 1)

0 0               // Start at (0, 0)

3 3               // End at (3, 3)
