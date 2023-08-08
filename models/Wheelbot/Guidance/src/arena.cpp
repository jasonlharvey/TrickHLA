#include "arena.hh"
#include <iostream>
#include <math.h>   // sqrt()
#include <stdlib.h> // abs()

Arena::Arena( unsigned int width, unsigned int height )
   : height( height ), width( width )
{

   unsigned int area = height * width;
   grid              = new GridSquare[area];
   for ( int i = 0; i < area; i++ ) {
      grid[i].is_blocked = false;
      grid[i].mark       = ' ';
   }
}

Arena::Arena( unsigned int width, unsigned int height, unsigned char bits[] )
   : height( height ), width( width )
{

   unsigned int area = height * width;
   grid              = new GridSquare[area];

   unsigned int cx = 0;
   for ( int row = 0; row < height; row++ ) {
      unsigned char octet = bits[cx];
      unsigned int  bx    = 0;
      while ( bx < width ) {
         unsigned int ii = row * width + bx;
         if ( ( bx != 0 ) && ( ( bx % 8 ) == 0 ) ) {
            cx++;
            octet = bits[cx];
         }
         grid[ii].is_blocked = ( 0x01 & octet );
         octet               = octet >> 1;
         grid[ii].mark       = ' ';
         bx++;
      }
      cx++;
      octet = bits[cx];
   }
}

Arena::~Arena()
{
   delete grid;
}

// straightDistance
int Arena::distance_between( GridSquare *orig, GridSquare *dest, int &distance )
{
   Point orig_pt;
   Point dest_pt;
   if ( ( ( get_grid_square_coordinates( orig, orig_pt ) ) == 0 ) && ( ( get_grid_square_coordinates( dest, dest_pt ) ) == 0 ) ) {
      distance = 10 * sqrt( ( dest_pt.x - orig_pt.x ) * ( dest_pt.x - orig_pt.x ) + ( dest_pt.y - orig_pt.y ) * ( dest_pt.y - orig_pt.y ) );
      return 0;
   }
   std::cerr << "Arena::distanceBetween: bad pointer parameter(s)." << std::endl;
   return 1;
}

// manhattenDistance
int Arena::movement_cost_estimate( GridSquare *orig, GridSquare *dest, int &cost_estimate )
{
   Point orig_pt;
   Point dest_pt;
   if ( ( ( get_grid_square_coordinates( orig, origPt ) ) == 0 ) && ( ( get_grid_square_coordinates( dest, destPt ) ) == 0 ) ) {
      cost_estimate = 10 * ( abs( dest_pt.x - orig_pt.x ) + abs( dest_pt.y - orig_pt.y ) );
      return 0;
   }
   std::cerr << "Arena::movementCostEstimate: bad pointer parameter(s)." << std::endl;
   return 1;
}

void Arena::block( unsigned int x, unsigned int y )
{
   GridSquare *grid_square;
   if ( ( grid_square = get_grid_square( x, y ) ) != (GridSquare *)0 ) {
      grid_square->is_blocked = true;
   }
}

void Arena::unblock( unsigned int x, unsigned int y )
{
   GridSquare *grid_square;
   if ( ( grid_square = get_grid_square( x, y ) ) != (GridSquare *)0 ) {
      grid_square->is_blocked = false;
   }
}

void Arena::mark( unsigned int x, unsigned int y, char c )
{
   GridSquare *grid_square;
   if ( ( grid_square = get_grid_square( x, y ) ) != (GridSquare *)0 ) {
      grid_square->mark = c;
   }
}

int Arena::calc_offset( unsigned int x, unsigned int y, size_t &offset )
{
   if ( ( x < width ) && ( y < height ) ) {
      offset = x + width * y;
      return 0;
   }
   return 1;
}

int Arena::calc_offset( GridSquare *gridSquare, size_t &offset )
{

   if ( grid_square >= grid ) {
      size_t toffset = ( grid_square - grid );
      if ( toffset < ( width * height ) ) {
         offset = toffset;
         return 0;
      }
   }
   return 1;
}

GridSquare *Arena::get_grid_square( unsigned int x, unsigned int y )
{
   size_t offset;
   if ( calc_offset( x, y, offset ) == 0 ) {
      return ( grid + offset );
   }
   return ( (GridSquare *)0 );
}

int Arena::get_grid_square_coordinates( GridSquare *grid_square_pointer, Point &coords )
{
   size_t offset;
   if ( calcOffset( grid_square_pointer, offset ) == 0 ) {
      coords.x = offset % width;
      coords.y = offset / width;
      return 0;
   } else {
      std::cerr << "Arena::getGridSquareCoordinates: problem." << std::endl;
      return 1;
   }
}

std::vector< GridSquare * > Arena::get_neighbors( GridSquare *grid_square_pointer )
{

   std::vector< GridSquare * > neighbors;
   GridSquare                 *neighbor;
   Point                       loc;

   if ( get_grid_square_coordinates( grid_square_pointer, loc ) == 0 ) {

#ifdef DIAGONAL_NEIGHBORS
      if ( ( neighbor = get_grid_square( loc.x + 1, loc.y + 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x + 1, loc.y - 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x - 1, loc.y - 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x - 1, loc.y + 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
#endif
      if ( ( neighbor = get_grid_square( loc.x, loc.y + 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x, loc.y - 1 ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x - 1, loc.y ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }
      if ( ( neighbor = get_grid_square( loc.x + 1, loc.y ) ) != (GridSquare *)0 ) {
         if ( !neighbor->is_blocked )
            neighbors.push_back( neighbor );
      }

   } else {
      std::cerr << "Arena::getNeighbors: invalid gridSquarePointer.";
   }
   return neighbors;
}

std::ostream &operator<<( std::ostream &s, const Arena &arena )
{
   s << "Arena height=" << arena.height << " width=" << arena.width << std::endl;

   for ( int y = 0; y < arena.height; y++ ) {
      s << "|";
      for ( int x = 0; x < arena.width; x++ ) {
         if ( arena.grid[x + arena.width * y].is_blocked ) {
            s << "\x1b[41m" << arena.grid[x + arena.width * y].mark << "\x1b[47m";
         } else {
            s << arena.grid[x + arena.width * y].mark;
         }
         s << "|";
      }
      s << std::endl;
   }

   return s;
}
