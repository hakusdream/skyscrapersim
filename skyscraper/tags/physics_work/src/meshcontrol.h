/* $Id$ */

/*
	Skyscraper 1.1 Alpha - Mesh Control Form
	Copyright (C)2005-2007 Ryan Thoryk
	http://www.skyscrapersim.com
	http://sourceforge.net/projects/skyscraper
	Contact - ryan@tliquest.net

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef MESHCONTROL_H
#define MESHCONTROL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(MeshControl)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class MeshControl: public wxDialog
{
	friend class Timer;
	public:

		MeshControl(wxWindow* parent,wxWindowID id = -1);
		virtual ~MeshControl();

		//(*Identifiers(MeshControl)
		static const long ID_STATICTEXT1;
		static const long ID_chkExternal;
		static const long ID_chkBuildings;
		static const long ID_chkLandscape;
		static const long ID_chkSky;
		static const long ID_chkColumnFrame;
		static const long ID_STATICTEXT2;
		static const long ID_chkFloor;
		static const long ID_bOk;
		//*)

	protected:

		//(*Handlers(MeshControl)
		void On_bOk_Click(wxCommandEvent& event);
		void On_chkColumnFrame_Click(wxCommandEvent& event);
		void On_chkSky_Click(wxCommandEvent& event);
		void On_chkLandscape_Click(wxCommandEvent& event);
		void On_chkBuildings_Click(wxCommandEvent& event);
		void On_chkExternal_Click(wxCommandEvent& event);
		void On_chkFloor_Click(wxCommandEvent& event);
		//*)
		void OnInit();

		//(*Declarations(MeshControl)
		wxBoxSizer* BoxSizer1;
		wxBoxSizer* BoxSizer2;
		wxBoxSizer* BoxSizer3;
		wxStaticText* StaticText1;
		wxCheckBox* chkExternal;
		wxCheckBox* chkBuildings;
		wxCheckBox* chkLandscape;
		wxCheckBox* chkSky;
		wxCheckBox* chkColumnFrame;
		wxBoxSizer* BoxSizer4;
		wxStaticText* StaticText2;
		wxCheckBox* chkFloor;
		wxButton* bOk;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
