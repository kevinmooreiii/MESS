/*
        Chemical Kinetics and Dynamics Library
        Copyright (C) 2008-2013, Yuri Georgievski <ygeorgi@anl.gov>

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Library General Public
        License as published by the Free Software Foundation; either
        version 2 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Library General Public License for more details.
*/

#ifndef FOURIER_HH
#define FOURIER_HH

// multidimensional fourier expansion
class FourierExpansion {
  int _dim;
  std::map<std::vector<int>, double > _data;
  void _check (const std::vector<int>&) ;

public:

  void add (const std::vector<int>&, double) ;
};

#endif
