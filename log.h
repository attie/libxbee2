#ifndef __XBEE_LOG_H
#define __XBEE_LOG_H

/*
  libxbee - a C library to aid the use of Digi's XBee wireless modules
            running in API mode (AP=2).

  Copyright (C) 2009  Attie Grande (attie@attie.co.uk)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XBEE_DISABLE_LOGGING

int xbee_shouldLog(int minLevel);

void _xbee_log(const char *file, int line, const char *function, struct xbee *xbee, int minLevel, char *format, ...);
#define xbee_log(...) \
	_xbee_log(__FILE__, __LINE__, __FUNCTION__, xbee, __VA_ARGS__)

void _xbee_perror(const char *file, int line, const char *function, struct xbee *xbee, int minLevel, char *format, ...);
#define xbee_perror(...) \
	_xbee_perror(__FILE__, __LINE__, __FUNCTION__, xbee, __VA_ARGS__)

void _xbee_logstderr(const char *file, int line, const char *function, struct xbee *xbee, int minLevel, char *format, ...);
#define xbee_logstderr(...) \
	_xbee_logstderr(__FILE__, __LINE__, __FUNCTION__, xbee, __VA_ARGS__)

#else /* XBEE_DISABLE_LOGGING */

#define xbee_log(...)
#define xbee_perror(...)
#define xbee_logstderr(...)

#endif /* XBEE_DISABLE_LOGGING */

#endif /* __XBEE_LOG_H */

