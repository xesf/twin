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

#ifndef TWIN_BLOCK_LIBRARY_H
#define TWIN_BLOCK_LIBRARY_H


namespace Common {
class SeekableReadStream;
}

namespace Twin {

enum ShapeType {
	kNone = 0,
	kSolid = 1,
	kStairsTopLeft = 2,
	kStairsTopRight = 3,
	kStairsBottomLeft = 4,
	kStairsBottomRight = 5,
	kDoubleSideStairsTop1 = 6,
	kDoubleSideStairsBottom1 = 7,
	kDoubleSideStairsLeft1 = 8,
	kDoubleSideStairsRight1 = 9,
	kDoubleSideStairsTop2 = 10,
	kDoubleSideStairsBottom2 = 11,
	kDoubleSideStairsLeft2 = 12,
	kDoubleSideStairsRight2 = 13,
	kFlatBottom1 = 14,
	kFlatBottom2 = 15
};

class SubBlock {
public:
	uint16 _index;
	ShapeType _shape;
	byte _type;
};

class BlockInfo {
public:
	SubBlock *_blocks;
	uint32 _numBlocks;
	byte _x;
	byte _y;
	byte _z;
};

class BlockLibrary {
public:
	BlockLibrary(Common::SeekableReadStream *stream);
	~BlockLibrary();
	BlockInfo *getBlock(uint16 i) { return &_blocks[i]; }
private:
	void loadLBA2(Common::SeekableReadStream *stream);


	uint32 _numOffsets;
	uint32 *_offsets;
	BlockInfo *_blocks;
};

} // end of namespace Twin

#endif
