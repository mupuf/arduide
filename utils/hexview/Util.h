/*
Copyright (C) 2006 - 2009 Evan Teran
                   eteran@alum.rit.edu

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef UTIL_20061126_H_
#define UTIL_20061126_H_

namespace util {

//------------------------------------------------------------------------------
// Name: safe_ctype(unsigned char c)
// Desc: <cctype> functions accept only unsigned char values or EOF, but 'char'
//       is often signed. When passing 'char', use safe_ctype<std::tolower>(c) 
//       instead of std::tolower(c).
//------------------------------------------------------------------------------
template <int (&F) (int)>
int safe_ctype(unsigned char c) {
	return F(c);
}

//------------------------------------------------------------------------------
// Name: percentage(int regionsFinished, int regionsTotal, int bytesDone, int bytesTotal)
// Desc: calculates how much of a multi-region byte search we have completed
//------------------------------------------------------------------------------
inline int percentage(int regionsFinished, int regionsTotal, int bytesDone, int bytesTotal) {

	// how much percent does each region account for?
	const double regionStep         = 1.0 / static_cast<double>(regionsTotal) * 100;
	
	// how many regions are done?
	const double regionsComplete    = regionStep * regionsFinished;

	// how much of the current region is done?
	const double regionPercent      = regionStep * static_cast<float>(bytesDone) / static_cast<float>(bytesTotal);
	
	return static_cast<int>(regionsComplete + regionPercent);
}

//------------------------------------------------------------------------------
// Name: percentage(int regionsFinished, int regionsTotal, int bytesDone, int bytesTotal)
// Desc: calculates how much of a single-region byte search we have completed
//------------------------------------------------------------------------------
inline int percentage(int bytesDone, int bytesTotal) {
	return percentage(0, 1, bytesDone, bytesTotal);
}

}

#endif
