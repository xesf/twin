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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/scummsys.h"

#ifdef USE_GLES2

namespace Graphics {
namespace BuiltinShaders {

const char *compatVertex =
	"#ifdef GL_ES\n"
	"mediump float round(in mediump float x) {\n"
	"	return sign(x) * floor(abs(x) + .5);\n"
	"}\n"
	"#define in attribute\n"
	"#define out varying\n"
	"#endif\n";

const char *compatFragment =
	"#ifdef GL_ES\n"
		"#define in varying\n"
		"#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
			"precision highp float;\n"
		"#else\n"
			"precision mediump float;\n"
		"#endif\n"
		"#define OUTPUT\n"
		"#define outColor gl_FragColor\n"
		"#define texture texture2D\n"
	"#else\n"
		"#define OUTPUT out vec4 outColor;\n"
	"#endif\n";

}
}

#endif
