////////////////////////////////////////////////////////////////////////////////
// File name   : 081.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-11-03                                                   //
// Last Change : 2020-11-03                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 81: Time Recording (Day)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init81()
{
//alert('Init Start: '+oFXP.tr);
	fxf_fn_warnTimeRecordingDates();
}

function fxf_fn_saveElement81(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if(element.id == 'Projekt_ID')
	{
		var pid=parseInt(fxf_fn_getSelectedValue(element).value);
		fxf_fn_updateEstMethod.delay(oFXP.mdelay, pid);
	}
	if((element.id == 'Datum') || (element.id == 'startdatum') || (element.id == 'enddatum'))
		fxf_fn_warnTimeRecordingDates();

	return 0;
}

function fxf_fn_warnTimeRecordingDates()
{
	// Necessary elements
	var div=$('warn_pers_div');
	var dates=$('warn_pers_dates');
	var date=$('warn_pers_date');
	if(!div || !dates || !date)
		return;

	// Get actual date
	if(oFXP.tr == 81)	// Day
	{
		var sinput=fxf_fn_getElement('Datum');
		var einput=sinput;
	}
	else
	{
		var sinput=fxf_fn_getElement('startdatum');
		var einput=fxf_fn_getElement('enddatum');
	}
	if(!sinput || !einput)
	{
		div.style.display='none';
		return;
	}

	var svalue=fxf_fn_convertDate2Timestamp(sinput.value);
	if((oFXP.tr == 81) || !einput.value.length)
		var evalue=svalue;
	else
	{
		var evalue=fxf_fn_convertDate2Timestamp(einput.value);
		if(evalue.timestamp < svalue.timestamp)
		{
			var swap=svalue;
			svalue=evalue;
			evalue=swap;
		}
	}

	// Get person id, name, entry and exit date
	var data=dates.innerHTML.split('|');
	var pid=parseInt(data[0]);
	var name=data[1];
	var edate=data[2];
	var xdate=data[3];
//alert('fxf_fn_warnTimeRecordingDates()\n\npid='+pid+'\nname='+name+'\nedate='+edate+'\nxdate='+xdate+'\n\nsvalue.timestamp='+svalue.timestamp+'\nsvalue.date='+svalue.date+'\n\nevalue.timestamp='+evalue.timestamp+'\nevalue.date='+evalue.date);

	// Save button
	var sbutton=$('Button_OK');

	// Set date and display or hide message
	if(evalue.date.length && (evalue.date != svalue.date))
		date.innerHTML=svalue.date+'-'+evalue.date;
	else
		date.innerHTML=svalue.date;

	var disp=false;
	if((svalue.timestamp == evalue.timestamp) && ((svalue.timestamp < edate) || (svalue.timestamp > xdate)))
		disp=true;
	else if((svalue.timestamp != evalue.timestamp) && ((xdate < evalue.timestamp) || (edate > svalue.timestamp)))
		disp=true;
	if(disp)
	{
		div.style.display='block';
		oID.fxwork.scrollTop=0;
		if(sbutton)
			sbutton.style.display='none';
	}
	else
	{
		div.style.display='none';
		if(sbutton)
			sbutton.style.display='';
	}
}
