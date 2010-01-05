/* $Id$ */

/*
	Skyscraper 1.6 Alpha - Object Information Dialog
	Copyright (C)2003-2010 Ryan Thoryk
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

//(*InternalHeaders(ObjectInfo)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "debugpanel.h"
#include "objectinfo.h"
#include "globals.h"
#include "sbs.h"

extern SBS *Simcore; //external pointer to the SBS engine

//(*IdInit(ObjectInfo)
const long ObjectInfo::ID_STATICTEXT1 = wxNewId();
const long ObjectInfo::ID_tNumber = wxNewId();
const long ObjectInfo::ID_STATICTEXT5 = wxNewId();
const long ObjectInfo::ID_tName = wxNewId();
const long ObjectInfo::ID_STATICTEXT2 = wxNewId();
const long ObjectInfo::ID_tType = wxNewId();
const long ObjectInfo::ID_STATICTEXT3 = wxNewId();
const long ObjectInfo::ID_tParent = wxNewId();
const long ObjectInfo::ID_STATICTEXT6 = wxNewId();
const long ObjectInfo::ID_tParentName = wxNewId();
const long ObjectInfo::ID_STATICTEXT4 = wxNewId();
const long ObjectInfo::ID_tParentType = wxNewId();
const long ObjectInfo::ID_STATICLINE1 = wxNewId();
const long ObjectInfo::ID_STATICTEXT7 = wxNewId();
const long ObjectInfo::ID_tLineNum = wxNewId();
const long ObjectInfo::ID_STATICTEXT8 = wxNewId();
const long ObjectInfo::ID_tScriptCommand = wxNewId();
const long ObjectInfo::ID_STATICTEXT9 = wxNewId();
const long ObjectInfo::ID_tScriptCommand2 = wxNewId();
const long ObjectInfo::ID_bOK = wxNewId();
//*)

BEGIN_EVENT_TABLE(ObjectInfo,wxDialog)
	//(*EventTable(ObjectInfo)
	//*)
END_EVENT_TABLE()

ObjectInfo::ObjectInfo(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(ObjectInfo)
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	
	Create(parent, wxID_ANY, _("Object Info"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	FlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Number:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tNumber = new wxTextCtrl(this, ID_tNumber, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tNumber"));
	FlexGridSizer1->Add(tNumber, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tName = new wxTextCtrl(this, ID_tName, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tName"));
	FlexGridSizer1->Add(tName, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tType = new wxTextCtrl(this, ID_tType, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tType"));
	FlexGridSizer1->Add(tType, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Parent:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tParent = new wxTextCtrl(this, ID_tParent, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tParent"));
	FlexGridSizer1->Add(tParent, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Parent Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tParentName = new wxTextCtrl(this, ID_tParentName, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tParentName"));
	FlexGridSizer1->Add(tParentName, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Parent Type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tParentType = new wxTextCtrl(this, ID_tParentType, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tParentType"));
	FlexGridSizer1->Add(tParentType, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	FlexGridSizer2->Add(StaticLine1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Object created on line:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer3->Add(StaticText7, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tLineNum = new wxTextCtrl(this, ID_tLineNum, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tLineNum"));
	FlexGridSizer3->Add(tLineNum, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Script Command:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer2->Add(StaticText8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tScriptCommand = new wxTextCtrl(this, ID_tScriptCommand, wxEmptyString, wxDefaultPosition, wxSize(300,40), wxTE_MULTILINE|wxTE_READONLY|wxTE_CENTRE|wxTE_WORDWRAP, wxDefaultValidator, _T("ID_tScriptCommand"));
	FlexGridSizer2->Add(tScriptCommand, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Processed Script Command:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer2->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	tScriptCommand2 = new wxTextCtrl(this, ID_tScriptCommand2, wxEmptyString, wxDefaultPosition, wxSize(-1,40), wxTE_MULTILINE|wxTE_READONLY|wxTE_CENTRE, wxDefaultValidator, _T("ID_tScriptCommand2"));
	FlexGridSizer2->Add(tScriptCommand2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bOK = new wxButton(this, ID_bOK, _("OK"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_bOK"));
	FlexGridSizer2->Add(bOK, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	
	Connect(ID_bOK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectInfo::On_bOK_Click);
	//*)
	//OnInit();
	oldobject = -1;
}

ObjectInfo::~ObjectInfo()
{
	//(*Destroy(ObjectInfo)
	//*)
}


void ObjectInfo::On_bOK_Click(wxCommandEvent& event)
{
	this->Hide();
}

void ObjectInfo::Loop()
{
	int number = Simcore->camera->GetClickedObjectNumber();
	if (number == oldobject)
		return;
	
	oldobject = number;
	tNumber->SetValue(wxVariant(number).GetString());
	tLineNum->SetValue(wxVariant(Simcore->camera->GetClickedObjectLine()).GetString());
	tScriptCommand->SetValue(wxString::FromAscii(Simcore->camera->GetClickedObjectCommand()));
	tScriptCommand2->SetValue(wxString::FromAscii(Simcore->camera->GetClickedObjectCommandP()));
	tName->Clear();
	tType->Clear();
	tParent->Clear();
	tParentName->Clear();
	tParentType->Clear();

	Object *object = Simcore->GetObject(number);

	if (object)
	{
		tType->SetValue(wxString::FromAscii(object->GetType()));

		Object *parent = object->GetParent();
		if (parent)
		{
			tParent->SetValue(wxVariant(parent->GetNumber()).GetString());
			tParentType->SetValue(wxString::FromAscii(parent->GetType()));
		}
	}
}
