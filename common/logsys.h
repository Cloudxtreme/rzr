/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2006 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef LOGSYS_H_
#define LOGSYS_H_

/*
*
* Usage:
*
*	These are the main functions provided by logsys:
*		- _log(TYPE, fmt, ...) - Log a message in any context
*		- mlog(TYPE, fmt, ...) - Zone only. Log a message from a Mob:: context, prefixing it with the mob's name.
*		- clog(TYPE, fmt, ...) - World only. Log a message from a Client:: context, prefixing it with the client's account name.
*		- zlog(TYPE, fmt, ...) - World only. Log a message from a ZoneServer:: context, prefixing it with the zones id/name or ip/port.
*		- _hex(TYPE, data, length) - Log hex dump in any context.
*		- mhex(TYPE, data, length) - Zone only. Log a hex dump from a Mob:: context, prefixing it with the mob's name
*		- _pkt(TYPE, BasePacket *) - Log a packet hex dump with header in any context.
*		- mhex(TYPE, data, length) - Zone only. Log a packet hex dump from a Mob:: context, prefixing it with the mob's name
* Types are defined in logtypes.h
*
* this is very C-ish, not C++ish, but thats how I felt like writting it
*/

#include <stdarg.h>
#include "types.h"

#define LOG_CATEGORY(category) LOG_ ##category ,
typedef enum {
	#include "logtypes.h"
	NUMBER_OF_LOG_CATEGORIES
} LogCategory;

#define LOG_TYPE(category, type, enabled) category##__##type ,
typedef enum {
	#include "logtypes.h"
	NUMBER_OF_LOG_TYPES
} LogType;

extern const char *log_category_names[NUMBER_OF_LOG_CATEGORIES];

typedef struct {
	bool enabled;
	LogCategory category;
	const char *name;
} LogTypeStatus;

//expose a read-only pointer
extern const LogTypeStatus *log_type_info;

// For log_packet, et all.
class BasePacket;

extern void log_message(LogType type, const char *fmt, ...);
extern void log_messageVA(LogType type, const char *fmt, va_list args);
extern void log_hex(LogType type, const void *data, unsigned long length, unsigned char padding=4);
extern void log_packet(LogType type, const BasePacket *p);
extern void log_raw_packet(LogType type, uint16 seq, const BasePacket *p);

#ifndef DISABLE_LOGSYS
/* these are macros which do not use ..., and work for anybody */
	#define _hex( type, data, len) \
		do { \
			if(log_type_info[ type ].enabled) { \
				log_hex(type, (const char *)data, len); \
			} \
		} while(false)
	#define _pkt( type, packet) \
		do { \
			if(log_type_info[ type ].enabled) { \
				log_packet(type, packet); \
			} \
		} while(false)
	#define _raw( type, seq, packet) \
		do { \
			if(log_type_info[ type ].enabled) { \
				log_raw_packet(type, seq, packet); \
			} \
		} while(false)

#else
		#define _hex( type, data, len) {}
		#define _pkt( type, packet) {}
		#define _raw( type, seq, packet) {}
#endif //!DISABLE_LOGSYS
#ifdef ZONE
	class Mob;
	extern void log_hex_mob(LogType type, Mob *who, const char *data, uint32 length);
	#define mhex( type, data, len) \
		do { \
		if(IsLoggingEnabled()) \
			if(log_type_info[ type ].enabled) { \
				log_hex_mob(type, this, data, len); \
			} \
		} while(false)
#endif

extern void log_enable(LogType t);
extern void log_disable(LogType t);
extern void log_toggle(LogType t);

#define is_log_enabled( type ) \
	log_type_info[ type ].enabled

extern bool load_log_settings(const char *filename);

#endif /*LOGSYS_H_*/

