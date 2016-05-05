/*
 * Copyright (C) 2016 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __gtkardour_plugin_setup_dialog_h__
#define __gtkardour_plugin_setup_dialog_h__

#include "ardour/plugin_insert.h"
#include "ardour/route.h"

#include "ardour_dropdown.h"
#include "ardour_dialog.h"

class PluginSetupDialog : public ArdourDialog
{
public:
	PluginSetupDialog (boost::shared_ptr<ARDOUR::Route>, boost::shared_ptr<ARDOUR::PluginInsert>);
	bool need_setup () const;

	enum SetupOptions {
		None = 0x0,
		CanReplace = 0x1,
		MultiOut = 0x2,
	};

private:
	void check_setup_needed ();

	void setup_output_presets ();
	void select_output_preset (uint32_t n_audio);

	boost::shared_ptr<ARDOUR::Route> _route;
	boost::shared_ptr<ARDOUR::PluginInsert> _pi;
	enum SetupOptions _setup_options;

	ArdourDropdown _out_presets;
};

#endif
