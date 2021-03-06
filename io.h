#ifndef __XBEE_IO_H
#define __XBEE_IO_H

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

#define XBEE_IO_RETRIES 10
#define XBEE_IO_RETRIES_WARN 6

int xbee_io_open(struct xbee *xbee);
void xbee_io_close(struct xbee *xbee);
int xbee_io_reopen(struct xbee *xbee);

int xbee_io_getRawByte(struct xbee *xbee, unsigned char *cOut);
int xbee_io_getEscapedByte(struct xbee *xbee, unsigned char *cOut);

int xbee_io_writeRawByte(struct xbee *xbee, unsigned char c);
int xbee_io_writeEscapedByte(struct xbee *xbee, unsigned char c);

#endif /* __XBEE_IO_H */

