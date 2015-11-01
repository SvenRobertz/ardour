/*
    Copyright (C) 2015 Paul Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __ardour_trigger_track_h__
#define __ardour_trigger_track_h__

#include "ardour/track.h"
#include "ardour/data_type.h"

namespace ARDOUR
{

class InterThreadInfo;
class RouteGroup;
class Session;

class LIBARDOUR_API TriggerTrack : public Track
{
public:
	TriggerTrack (Session&, std::string name, Route::Flag f = Route::Flag (0), TrackMode m = Normal);
	~TriggerTrack ();

	int init ();

	int roll (pframes_t nframes, framepos_t start_frame, framepos_t end_frame, int declick, bool& need_butler);

	void realtime_handle_transport_stopped ();
	void realtime_locate ();
	void non_realtime_locate (framepos_t);

	boost::shared_ptr<Diskstream> create_diskstream ();
	void set_diskstream (boost::shared_ptr<Diskstream>);

	DataType data_type () const {
		return DataType::AUDIO;
	}

	bool bounceable (boost::shared_ptr<Processor>, bool) const { return false; }

	int set_state (const XMLNode&, int version);

protected:
	XMLNode& state (bool full);

private:
	int no_roll (pframes_t nframes, framepos_t start_frame, framepos_t end_frame, bool state_changing);
};

} /* namespace ARDOUR*/

#endif /* __ardour_trigger_track_h__ */
