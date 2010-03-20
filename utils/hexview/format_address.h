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

#ifndef FORMAT_ADDRESS_20090910_H_
#define FORMAT_ADDRESS_20090910_H_

namespace util {
	namespace detail {
		template <class T, size_t N>
		struct address_format {};

		template <class T>
		struct address_format<T, 4> {
			template <size_t N>
			static void format_string(char (&buffer)[N], bool show_separator) {
				if(show_separator) {
					strncpy(buffer, "%04x:%04x", N);
				} else {
					strncpy(buffer, "%04x%04x", N);
				}
				buffer[N - 1] = '\0';
			}

			static QString format(const char *fmt, T address) {
				return QString().sprintf(fmt, (address >> 16) & 0xffff, address & 0xffff);
			}
		};

		template <class T>
		struct address_format<T, 8> {
			template <size_t N>
			static void format_string(char (&buffer)[N], bool show_separator) {
				if(show_separator) {
					strncpy(buffer, "%08x:%08x", N);
				} else {
					strncpy(buffer, "%08x%08x", N);
				}
				buffer[N - 1] = '\0';
			}

			static QString format(const char *fmt, T address) {
				return QString().sprintf(fmt, (address >> 32) & 0xffffffff, address & 0xffffffff);
			}
		};
	}


	template <class T>
	struct format_address : public detail::address_format<T, sizeof(T)> {
	};
}

#endif
