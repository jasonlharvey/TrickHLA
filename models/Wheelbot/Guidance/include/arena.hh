#ifndef ARENA_H
#define ARENA_H
#include "gridSquare.hh"
#include "point.hh"
#include <iostream>
#include <vector>

class Arena
{
  public:
   Arena( unsigned int width, unsigned int height );
   Arena( unsigned int width, unsigned int height, unsigned char bits[] );
   ~Arena();
   void                        block( unsigned int x, unsigned int y );
   void                        unblock( unsigned int x, unsigned int y );
   void                        mark( unsigned int x, unsigned int y, char c );
   std::vector< GridSquare * > get_neighbors( GridSquare *grid_square_pointer );
   GridSquare                 *get_grid_square( unsigned int x, unsigned int y );
   int                         get_grid_square_coordinates( GridSquare *grid_square_pointer, Point &coords );
   int                         movement_cost_estimate( GridSquare *orig, GridSquare *dest, int &cost );
   int                         distance_between( GridSquare *orig, GridSquare *dest, int &distance );
   int                         get_height() { return height; }
   int                         get_width() { return width; }

   friend std::ostream &operator<<( std::ostream &s, const Arena &arena );

  private:
   int         height;
   int         width;
   GridSquare *grid;
   int         calc_offset( unsigned int x, unsigned int y, size_t &offset );
   int         calc_offset( GridSquare *grid_square, size_t &offset );
};
#endif
