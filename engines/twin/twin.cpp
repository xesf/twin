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

#include "common/error.h"
#include "common/file.h"
#include "common/events.h"

#include "engines/engine.h"
#include "graphics/pixelbuffer.h"

#include "engines/twin/twin.h"
#include "engines/twin/fla_decoder.h"
#include "engines/twin/hqr.h"
#include "engines/twin/image.h"
#include "engines/twin/scene.h"
#include "engines/twin/resource.h"
#include "engines/twin/colour_palette.h"
#include "engines/twin/grid.h"
#include "engines/twin/animation.h"
#include "engines/twin/model.h"
#include "engines/twin/entity_information.h"
#include "engines/twin/island.h"
#include "engines/twin/video_player.h"
#include "engines/twin/video_player_smk.h"
#include "engines/twin/animation.h"


namespace Twin {

TwinEngine *g_twin = NULL;
GfxBase *g_renderer = NULL;

TwinEngine::TwinEngine(OSystem *syst, TwinGameType type) :
		Engine(syst), _type(type) {
	g_twin = this;
}

TwinEngine::~TwinEngine() {
	g_twin = NULL;
	delete g_resource;
	g_resource = NULL;
}

void TwinEngine::playMovie(const Common::String &name) {
	// TODO: This is a simple hack for testing FLA-decoding,
	// Smacker-support is still not done, nor pretty file-opening.

	// Don't open files this way, HACK for now:
	Common::File *file = new Common::File();
	file->open("FLA/" + name);
	if (!file->isOpen()) {
		warning("Couldn't open %d", file->size());
	}
	
	// HACK: We just want to show the movie for now, we will probably want a cleaner
	// way to do so than this later.
	FlaDecoder movie;
	movie.loadStream(file);
	movie.start();
	
	while (!movie.endOfVideo()) {
		if (!movie.needsUpdate()) {
			continue;
		}
		const Graphics::Surface *movieSurface = movie.decodeNextFrame();
		
		uint32 texId = _renderer->createBitmap(movieSurface);
		_renderer->clearScreen();
		_renderer->drawBitmap(texId, 0, 0, 640 , 480);
		_renderer->flipBuffer();
		_renderer->destroyBitmap(texId);
	}
	delete file;

}

void TwinEngine::intro() {
	// TODO: Should fade Black to White.
	// TODO: Additional palette-work (for fading).
	Image *adelineLogo = nullptr;
	if (getGameType() == GType_LBA) {
		adelineLogo = new Image("RESS.HQR", 27, _renderer);
	} else if (getGameType() == GType_LBA2) {
		adelineLogo = new Image("SCREEN.HQR", 0, _renderer);
	}
	_renderer->clearScreen();
	adelineLogo->display();
	_renderer->flipBuffer();

	// Just delay a bit for now, so we can see the results.
	g_system->delayMillis(6000);
	delete adelineLogo;

	if (getGameType() == GType_LBA) {
		Image logo("RESS.HQR", 49, _renderer); // 12 in US-version
		_renderer->clearScreen();
		logo.display();
		_renderer->flipBuffer();

		g_system->delayMillis(3000);

		// TODO: Crossfade
		Image logo2("RESS.HQR", 52, _renderer);
		_renderer->clearScreen();
		logo2.display();
		_renderer->flipBuffer();
		g_system->delayMillis(1000);

		playMovie("DRAGON3.FLA");
	}
	
}

void TwinEngine::createRenderer() {
	_renderer = new GfxOpenGL();
	g_renderer = _renderer;
}

Common::Error TwinEngine::run() {
	g_resource = new Resource();
	createRenderer();
	_renderer->setupScreen(1024, 768, false);
	//intro();

	//Hqr body;
	//body.open("BODY.HQR");
	//Model *m;
	//m = new Model(body.createReadStreamForIndex(0));

	Hqr ress;
	ress.open("RESS.HQR");
	ColourPalette cp(ress.createReadStreamForIndex(0));
	_renderer->setColourPalette(&cp);
	_renderer->loadModelTexture(ress.createReadStreamForIndex(6));



	//VideoPlayerSmacker *player = new VideoPlayerSmacker(_renderer, nullptr);
	//player->play(16);

	Hqr CITABAU;
	CITABAU.open("DESERT.ILE");
	Hqr CITABAU2;
	CITABAU2.open("DESERT.OBL");
	Island idland(&CITABAU, &CITABAU2);

	uint32 body = 0;
	uint32 entity = 0;
	uint32 anim = 0;
	Entity *e = g_resource->getEntity(entity, body, anim);
	bool entityInfoChanged = false;
	//Animation a(anim.createReadStreamForIndex(67), m);

	Hqr scene;
	scene.open("SCENE.HQR");

	int grid = 15;
	Grid *g = g_resource->getGrid(grid);
	Scene *s = new Scene(scene.createReadStreamForIndex(grid + 1));
	bool mouseDown = false, wheelDown = false, rDown = false;
	uint32 lastTime = g_system->getMillis();
	for (;;) {
		Common::Event event;
		while (g_system->getEventManager()->pollEvent(event)) {
			Common::EventType type = event.type;
			if (type == Common::EVENT_KEYUP) {
				Common::KeyCode k = event.kbd.keycode;
				if (k == Common::KEYCODE_q) {
					++entity;
					entityInfoChanged = true;
				} else if (k == Common::KEYCODE_a && entity != 0) {
					--entity;
					entityInfoChanged = true;
				} else if (k == Common::KEYCODE_w) {
					++body;
					entityInfoChanged = true;
				} else if (k == Common::KEYCODE_s && body != 0) {
					--body;
					entityInfoChanged = true;
				} else if (k == Common::KEYCODE_e) {
					++anim;
					entityInfoChanged = true;
				} else if (k == Common::KEYCODE_d && anim != 0) {
					--anim;
					entityInfoChanged = true;
				}
			} else if (type == Common::EVENT_WHEELUP) {
				++grid;
				delete g;
				delete s;
				g = g_resource->getGrid(grid);
				s = new Scene(scene.createReadStreamForIndex(grid + 1));
			} else if (type == Common::EVENT_WHEELDOWN) {
				--grid;
				if (grid < 0) {
					grid = 0;
				} else {
					delete g;
					delete s;
					g = g_resource->getGrid(grid);
					s = new Scene(scene.createReadStreamForIndex(grid + 1));
				}
			} else if (type == Common::EVENT_LBUTTONDOWN) {
				mouseDown = true;
			} else if (type == Common::EVENT_LBUTTONUP) {
				mouseDown = false;
			} else if (type == Common::EVENT_MOUSEMOVE && mouseDown) {
				_renderer->moveCamera(event.relMouse.x, event.relMouse.y, 0);
			} else if (type == Common::EVENT_MOUSEMOVE && wheelDown) {
				_renderer->moveCamera(0, 0, event.relMouse.y);
			} else if (type == Common::EVENT_MOUSEMOVE && rDown) {
				_renderer->rotateObject(event.relMouse.x, event.relMouse.y, 0);
			} else if (type == Common::EVENT_MBUTTONDOWN) {
				wheelDown = true;
			} else if (type == Common::EVENT_MBUTTONUP) {
				wheelDown = false;
			} else if (type == Common::EVENT_RBUTTONDOWN) {
				rDown = true;
			} else if (type == Common::EVENT_RBUTTONUP) {
				rDown = false;
			} else if (type == Common::EVENT_QUIT) {
				return Common::kNoError;
			}
		}

		if (entityInfoChanged) {
			entityInfoChanged = false;
			delete e;
			e = g_resource->getEntity(entity, body, anim);
		}

		uint32 currentTime = g_system->getMillis();
		uint32 deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		_renderer->clearScreen();
		e->update(deltaTime);
		_renderer->drawGrid(g);
		_renderer->drawModel(e->_model);
		s->draw();
		//_renderer->drawIsland(&idland);
		_renderer->flipBuffer();
	}

	delete e;
	delete s;
	delete g;

	return Common::kNoError;
}

} // end of namespace Twin
