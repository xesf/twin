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

#include "common/stream.h"
#include "common/textconsole.h"

#include "engines/twin/entity_information.h"
#include "engines/twin/twin.h"
#include "engines/twin/hqr.h"

namespace Twin {

EntityInformation::EntityInformation(Common::SeekableReadStream *stream) {
	uint32 offset = stream->readUint32LE();

	stream->seek(offset);

	_numEntities = (offset/4) - 1;
	_entities = new EntityEntry[_numEntities];
	for (uint32 i = 0; i < _numEntities; ++i) {
		EntityEntry *e = &_entities[i];
		loadEntity(stream, e);
	}
	delete stream;
}

EntityInformation::EntityInformation(Hqr *file) {
	_numEntities = file->getNumIndices() - 1;
	_entities = new EntityEntry[_numEntities];
	for (uint32 i = 0; i < _numEntities; ++i) {
		Common::SeekableReadStream *stream = file->createReadStreamForIndex(i);
		EntityEntry *e = &_entities[i];
		loadEntity(stream, e);
		delete stream;
	}
}

EntityInformation::~EntityInformation() {

}

void EntityInformation::loadEntity(Common::SeekableReadStream *stream, EntityEntry *e) {
	for (;;) {
		byte opcode = stream->readByte();
		if (opcode == 1) {
			EntityBody b;
			b._index = stream->readByte();

			// This is just the size of the body
			stream->readByte();

			b._bodyIndex = stream->readUint16LE();
				
			b._hasBox = stream->readByte();
			if (b._hasBox) {
				// this is always 0x0E
				stream->readByte();
				b._box.load(stream);
			}
			e->_bodies.push_back(b);
		} else if (opcode == 3) {
			EntityAnim a;
			//TODO: figure out the anim data
			a._index = stream->readByte();
			if (g_twin->getGameType() == GType_LBA2) {
				stream->readByte();
			}
			byte size = stream->readByte();
			a._animIndex = stream->readUint16LE();
			for (byte j = 3; j < size; ++j) {
				stream->readByte();
			}
			e->_anims.push_back(a);
		} else if (opcode == 0xff) {
			break;
		} else {
			error("Unknown Entity Opcode: %d", opcode);
		}
	}
}


} // end of namespace Twin
