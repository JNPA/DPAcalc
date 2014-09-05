/*
Copyright (C) 2012	Massimo Maggi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#pragma once
#include "input/bin1.hpp"
#include "keygen/aes128keygen.hpp"
#include "gen_intermediate/aes128round1.hpp"
#include "gen_powermodel/hammingweight.hpp"
#include "statisticaltest/pearsonNormWithGPU2.hpp"
#include "exec/cxx11threads.hpp"
#include "output/multiple.hpp"
