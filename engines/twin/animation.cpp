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

#include "engines/twin/animation.h"
#include "engines/twin/twin.h"
#include "engines/twin/model.h"

namespace Twin {

Animation::Animation(Common::SeekableReadStream *stream, Model *m) {
	_model = m;
	if (g_twin->getGameType() == GType_LBA2) {
		loadLBA2(stream);
	}
}

Animation::~Animation() {
	for (int i = 0; i < _numKeys; ++i) {
		delete[] _keyframes[i]._bones;
	}
	delete[] _keyframes;
}

void Animation::loadLBA2(Common::SeekableReadStream *stream) {
	_numKeys = stream->readUint16LE();
	_numBones = stream->readUint16LE();
	_startFrame = stream->readUint16LE();
	uint16 unknown1 = stream->readUint16LE();

	_keyframes = new Keyframe[_numKeys];
	for (int i = 0; i < _numKeys; ++i) {
		Keyframe *k = &_keyframes[i];
		k->_length = stream->readUint16LE();
		k->_x = stream->readSint16LE();
		k->_y = stream->readSint16LE();
		k->_z = stream->readSint16LE();
		k->_bones = new Boneframe[_numBones];
		for (int j = 0; j < _numBones; ++j) {
			Boneframe *b = &k->_bones[j];
			b->_type = stream->readUint16LE();
			float x = stream->readSint16LE();
			float y = stream->readSint16LE();
			float z = stream->readSint16LE();
			if (b->_type == 0) {
				b->_pitch = (x * 360) / 4096;
				b->_yaw = (z * 360) / 4096;
				b->_roll = (y * 360) / 4096;
			} else {
				b->_pos.set(x / 16384.0f, y / 16384.0f, z / 16384.0f);
			}
		}
	}
	_currentFrame = 0;
	_time = 0;
}

void Animation::update(uint32 time) {
	_time += time;
	Keyframe *k = &_keyframes[_currentFrame];
	if (_time > k->_length) {
		_time = 0;
		++_currentFrame;
		if (_currentFrame >= _numKeys) {
			_currentFrame = _startFrame;
		}
		k = &_keyframes[_currentFrame];
	}
	Keyframe *next;
	if (_currentFrame != _numKeys - 1) {
		next = &_keyframes[_currentFrame + 1];
	} else {
		next = &_keyframes[_startFrame];
	}
	float interp = (float)_time / k->_length;
	for (int i = 0; i < _model->_numBones; ++i) {
		Boneframe *b = &k->_bones[i];
		Boneframe *bn = &next->_bones[i];
		Hierarchy *h = &_model->_heirs[i];
		if (b->_type == 0) {
			h->_isTransltion = false;
			h->_pitch = b->_pitch + (((((int)(bn->_pitch - b->_pitch).getDegrees() % 360) + 540) % 360) - 180) * interp;
			h->_yaw = b->_yaw + (((((int)(bn->_yaw - b->_yaw).getDegrees() % 360) + 540) % 360) - 180) * interp;
			h->_roll = b->_roll + (((((int)(bn->_roll - b->_roll).getDegrees() % 360) + 540) % 360) - 180) * interp;
		} else {
			h->_isTransltion = true;
			h->_translation = b->_pos + (bn->_pos - b->_pos) * interp;
		}
	}
	_model->recalculateHierarchy();
}


} // end of namespace Twin