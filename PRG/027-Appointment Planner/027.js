////////////////////////////////////////////////////////////////////////////////
// File name   : 027.js                                                       //
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
 * PF 27: Appointment Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init27()
{
//alert('JS: init'+oFXP.tr);
	fxf_fn_adjustInvitations();
}

function fxf_fn_saveElement27(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if(element.id == 'Projekt_ID')
		fxf_fn_adjustInvitations.delay(oFXP.ldelay);

	return 0;
}

function fxf_fn_adjustInvitations()
{
	var element=$('Projekt_ID');
	if(!element)
		return;

	var trids=null;
	var peids=null;

	var tmp=$('trids');
	if(tmp)
		trids=tmp.innerHTML.split('|');
//alert('trids='+trids+' (length='+trids.length+')');

	tmp=$('peids');
	if(tmp)
		peids=tmp.innerHTML.split('|');
//alert('peids='+peids+' (length='+peids.length+')');

	var pid=parseInt(fxf_fn_getSelectedValue(element).value);
	var url='loader.php?url=termine_einladung.inc&pid='+pid+fxf_fn_gParam();

	var prpeids=[];
	new Ajax.Request(url,
	{
		method: 'get',

		onSuccess: function(transport)
		{
			if(transport.responseText.length)
				prpeids=transport.responseText.split(',');
//alert('url='+url+'\nprpeids='+prpeids+'\nprpeids.length='+prpeids.length);

			var grpids=[];
			var grpimg=[];
			if(trids.length > 0)
			{
				var id_tr=null;
				var ogrp=-1;
				var grpc=-1;
				for(var i=0; i<trids.length; i++)
				{
					id_tr=$('tr'+trids[i]);
					if(id_tr)
					{
						var gl=trids[i].split('_');
						var grp=parseInt(gl[0]);
						if(grp > 34)
						{
							var cp=peids[i].split('_');
							if(grp != ogrp)
							{
								ogrp=grp;
								grpc++;
								grpids[grpc]='ipg'+grp;
								grpimg[grpc]='op';
							}
							var dlarge='none';
							if(prpeids.length > 0)
							{
								for(var p=0; p<prpeids.length; p++)
								{
									if(prpeids[p] == cp[1])
										dlarge='';
								}
							}
							if(dlarge.length)
							{
								var celem=$('einladen[3]['+cp[0]+']');
								if(celem.checked)
									dlarge='';
							}
							if(!dlarge.length)
								grpimg[grpc]='cl';
							id_tr.style.display=dlarge;
						}
					}
				}
			}

			if(grpids.length > 0)
			{
				for(var i=0; i<grpids.length; i++)
				{
					var elem=$(grpids[i]);
					if(elem)
						elem.src=elem.src.substr(0, elem.src.length-6)+grpimg[i]+'.png';
				}
			}
		}
	});
}
