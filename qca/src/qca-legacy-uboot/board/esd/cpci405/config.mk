#
# (C) Copyright 2000
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

#
# esd CPCI405 boards
#

ifeq ($(BOARD_REVISION),CPCI4052)
TEXT_BASE = 0xFFFC0000
else
ifeq ($(BOARD_REVISION),CPCI405DT)
TEXT_BASE = 0xFFFC0000
else
ifeq ($(BOARD_REVISION),CPCI405AB)
TEXT_BASE = 0xFFFC0000
else
TEXT_BASE = 0xFFFD0000
endif
endif
endif

# Compile the legacy NAND code (CFG_NAND_LEGACY must be defined)
BOARDLIBS = drivers/nand_legacy/libnand_legacy.a
