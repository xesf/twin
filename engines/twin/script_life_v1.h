/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TWIN_SCRIPT_LIFE_V1_H
#define TWIN_SCRIPT_LIFE_V1_H

#include "common/stack.h"

#include "engines/twin/script.h"
#include "engines/twin/script_life.h"

namespace Common {
class SeekableReadStream;
}

namespace Twin {

#define LIFE_OPCODES_V1					\
	OPCODEI(0x00, END);					\
										\
	OPCODEI(0x02, SNIF);				\
										\
	OPCODEI(0x04, NEVERIF);				\
										\
	OPCODEI(0x0B, RETURN);				\
	OPCODEI(0x0C, IF);					\
	OPCODEI(0x0D, SWIF);				\
	OPCODEI(0x0E, ONEIF);				\
	OPCODEI(0x0F, ELSE);				\
										\
	OPCODEI(0x11, BODY);				\
	OPCODEI(0x12, BODY_OBJ);			\
	OPCODEI(0x13, ANIM);				\
	OPCODEI(0x14, ANIM_OBJ);			\
										\
	OPCODEI(0x17, SET_TRACK);			\
	OPCODEI(0x18, SET_TRACK_OBJ);		\
	OPCODEI(0x19, MESSAGE);				\
	OPCODEI(0x1A, CAN_FALL);			\
	OPCODEI(0x1B, SET_DIRMODE);			\
	OPCODEI(0x1C, SET_DIRMODE_OBJ);		\
	OPCODEI(0x1D, CAM_FOLLOW);			\
	OPCODEI(0x1E, SET_BEHAVIOUR);		\
	OPCODEI(0x1F, SET_VAR_CUBE);		\
										\
	OPCODEI(0x21, SET_COMPORTEMENT);	\
	OPCODEI(0x22, SET_COMPORTEMENT_OBJ);\
	OPCODEI(0x23, END_COMPORTEMENT);	\
	OPCODEI(0x24, SET_VAR_GAME);		\
	OPCODEI(0x25, KILL_OBJ);			\
	OPCODEI(0x26, SUICIDE);				\
	OPCODEI(0x27, USE_ONE_LITTLE_KEY);	\
	OPCODEI(0x28, GIVE_GOLD_PIECES);	\
	OPCODEI(0x29, END_LIFE);			\
	OPCODEI(0x2A, STOP_CURRENT_TRACK );	\
	OPCODEI(0x2B, RESTORE_LAST_TRACK );	\
	OPCODEI(0x2C, MESSAGE_OBJ);			\
	OPCODEI(0x2D, INC_CHAPTER);			\
	OPCODEI(0x2E, FOUND_OBJECT);		\
	OPCODEI(0x2F, SET_DOOR_LEFT);		\
	OPCODEI(0x30, SET_DOOR_RIGHT);		\
	OPCODEI(0x31, SET_DOOR_UP);			\
	OPCODEI(0x32, SET_DOOR_DOWN);		\
	OPCODEI(0x33, GIVE_BONUS);			\
										\
	OPCODEI(0x37, OR_IF);				\
										\
	OPCODEI(0x3D, SET_LIFE_POINT_OBJ);	\
										\
	OPCODE(0x47, INIT_PINGOUIN);		\



#define LIFE_COND_OPCODES_V1				\
	COND_OPCODE(0x00, COL, 0);				\
	COND_OPCODE(0x01, COL_OBJ, 1);			\
	COND_OPCODE(0x02, DISTANCE, 1);			\
	COND_OPCODE(0x03, ZONE, 0);				\
	COND_OPCODE(0x04, ZONE_OBJ, 1);			\
	COND_OPCODE(0x05, BODY_COND, 0);		\
	COND_OPCODE(0x06, BODY_OBJ_COND, 1);	\
	COND_OPCODE(0x07, ANIM_COND, 0);		\
	COND_OPCODE(0x08, ANIM_OBJ_COND, 1);	\
	COND_OPCODE(0x09, CURRENT_TRACK, 0);	\
	COND_OPCODE(0x0A, CURRENT_TRACK_OBJ, 1);\
	COND_OPCODE(0x0B, VAR_CUBE, 1);			\
	COND_OPCODE(0x0C, CONE_VIEW, 1);		\
	COND_OPCODE(0x0D, HIT_BY, 0);			\
	COND_OPCODE(0x0E, ACTION, 0);			\
	COND_OPCODE(0x0F, VAR_GAME, 1);			\
	COND_OPCODE(0x10, LIFE_POINT, 0);		\
	COND_OPCODE(0x11, LIFE_POINT_OBJ, 1);	\
	COND_OPCODE(0x12, NUM_LITTLE_KEYS, 0);	\
	COND_OPCODE(0x13, NUM_GOLD_PIECES, 0);	\
	COND_OPCODE(0x14, BEHAVIOUR, 0);		\
	COND_OPCODE(0x15, CHAPTER, 0);			\
	COND_OPCODE(0x16, DISTANCE_3D, 1);		\
											\
	COND_OPCODE(0x19, USE_INVERNTORY, 1);	\
											\
	COND_OPCODE(0x1C, CARRIED_BY, 0);		\


class ScriptLifeV1 : public ScriptLife {
public:
	ScriptLifeV1(Common::SeekableReadStream *stream, ScriptTrack *track);

private:
	bool testCond(uint16 a, uint16 b, byte oper);

	void execute(byte opcode) override;

	bool checkFuncCondition(byte cond) override;
	void loadFuncConditionParam(byte cond) override;


	//Opcodes
	#define OPCODE(op, func) void func()
	#define OPCODEI(op, func)
		LIFE_OPCODES_V1
	#undef OPCODE
	#undef OPCODEI
};

} // end of namespace Twin

#endif
