/*
 * Copyright (C) 2016 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2011 Paul Davis
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

#include <gtkmm/frame.h>
#include <gtkmm/label.h>

#include "plugin_setup_dialog.h"
#include "i18n.h"

using namespace ARDOUR;
using namespace Gtk;

inline PluginSetupDialog::SetupOptions operator|= (PluginSetupDialog::SetupOptions& a, const PluginSetupDialog::SetupOptions& b) {
	return a = static_cast<PluginSetupDialog::SetupOptions> (static_cast <int>(a) | static_cast<int> (b));
}

PluginSetupDialog::PluginSetupDialog (boost::shared_ptr<ARDOUR::Route> r, boost::shared_ptr<ARDOUR::PluginInsert> pi)
	: ArdourDialog (_("Plugin Setup"), true, false)
	, _route (r)
	, _pi (pi)
	, _setup_options (None)
{
	check_setup_needed ();

	if (_setup_options & CanReplace) {
		Gtk::Label *l = manage (new Label (string_compose (
						_("An Instrument plugin is already present.\nReplace '%1' with '%2'?"),
						r->the_instrument ()->name (),
						pi->name ()
						)));
		l->set_line_wrap ();
		get_vbox()->pack_start (*l);
		add_button ("Replace", 2);
		// TODO option keep mapping if [selected] port-count matches
	}

	if (_setup_options & MultiOut) {
		setup_output_presets ();
		Frame* f = manage (new Frame ());
		f->set_label (_("Output Configuration"));
		f->add (_out_presets);
		get_vbox()->pack_start (*f, false, false, 6);
	}

	add_button (Stock::ADD, 1);
	add_button (Stock::CANCEL, RESPONSE_CANCEL);
	set_default_response (RESPONSE_ACCEPT);
	show_all ();
}


bool
PluginSetupDialog::need_setup () const
{
	// TODO mask "don't ask again" flags
	return _setup_options != None;
}


void
PluginSetupDialog::check_setup_needed ()
{
	bool need_dropdown = true;

	if (!_pi->plugin ()->get_info ()->is_instrument ()) {
		return;
	}

	boost::shared_ptr<Processor> the_instrument = _route->the_instrument ();

	if (the_instrument) {
		_setup_options |= CanReplace;
	}

	if (_pi->plugin (0)->get_info()->reconfigurable_io ()) {
		ChanCount in (DataType::MIDI, 1);
		ChanCount out (DataType::AUDIO, 2); // XXX route's out?!
		if (the_instrument) {
			in = the_instrument->input_streams ();
			out = the_instrument->output_streams ();
		}
		// collect possible outs
		_pi->plugin (0)->can_support_io_configuration (in, out);
	}

	// compare to PluginPinDialog::refill_output_presets ()
	PluginOutputConfiguration ppc (_pi->plugin (0)->possible_output ());
	if (ppc.size () == 0) {
		need_dropdown = false;
	}

	if (!_route->strict_io () && ppc.size () == 1) {
		need_dropdown = false;
	}
	if (_route->strict_io () && ppc.size () == 1) {
		// "stereo" is currently preferred default for instruments, see PluginInsert
		if (ppc.find (2) != ppc.end ()) {
			need_dropdown = false;
		}
	}

	if (need_dropdown) {
		_setup_options |= MultiOut;
	}
}

void
PluginSetupDialog::setup_output_presets ()
{
	// compare to PluginPinDialog::refill_output_presets ()
	using namespace Menu_Helpers;
	PluginOutputConfiguration ppc (_pi->plugin (0)->possible_output ());

	_out_presets.AddMenuElem (MenuElem (_("Automatic"), sigc::bind (sigc::mem_fun (*this, &PluginSetupDialog::select_output_preset), 0)));

	if (ppc.find (0) != ppc.end ()) {
		// anyting goes
		ppc.clear ();
		ppc.insert (1);
		ppc.insert (2);
		ppc.insert (8);
		ppc.insert (16);
		ppc.insert (24);
		ppc.insert (32);
	}

	for (PluginOutputConfiguration::const_iterator i = ppc.begin () ; i != ppc.end (); ++i) {
		assert (*i > 0);
		std::string tmp;
		switch (*i) {
			case 1:
				tmp = _("Mono");
				break;
			case 2:
				tmp = _("Stereo");
				break;
			default:
				tmp = string_compose (P_("%1 Channel", "%1 Channels", *i), *i);
				break;
		}
		_out_presets.AddMenuElem (MenuElem (tmp, sigc::bind (sigc::mem_fun (*this, &PluginSetupDialog::select_output_preset), *i)));
	}

	_out_presets.set_text (_("Automatic"));
}

void
PluginSetupDialog::select_output_preset (uint32_t n_audio)
{
	_pi->set_preset_out (ChanCount (DataType::AUDIO, n_audio));
	// TODO check if it matches the_instrument(), if so allow to keep I/O map.
}
