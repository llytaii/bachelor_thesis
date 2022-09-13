/* Copyright (C) 2005-2022, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */

#include "Unigine.h"
#include "AppSystemLogic.h"
#include "UnigineApp.h"
#include "UnigineComponentSystem.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace Unigine;

// System logic, it exists during the application life cycle.
// These methods are called right after corresponding system script's (UnigineScript) methods.

AppSystemLogic::AppSystemLogic()
{
}

AppSystemLogic::~AppSystemLogic()
{
}

int AppSystemLogic::init()
{
	// read shutdown config file
	std::fstream config{"../shutdown_config.txt", std::ios::in};
	if (config.is_open())
	{
		std::string tmp;
		float number;
		std::getline(config, tmp);
		std::stringstream ss{ tmp };

		m_shutdown_timer = 1;
	
		while (ss >> number)
		{
			m_shutdown_timer *= number;
		}
		++m_shutdown_timer;
	}


	// Write here code to be called on engine initialization.
	Unigine::App::setBackgroundUpdate(true);
	ComponentSystem::get()->initialize();
	World::setUnpackNodeReferences(true);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppSystemLogic::update()
{
	// Write here code to be called before updating each render frame.
	m_shutdown_timer -= Game::getIFps();
	if (m_shutdown_timer < 0.0f)
		Unigine::App::exit();
	return 1;
}

int AppSystemLogic::postUpdate()
{
	// Write here code to be called after updating each render frame.
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppSystemLogic::shutdown()
{
	// Write here code to be called on engine shutdown.
	return 1;
}
