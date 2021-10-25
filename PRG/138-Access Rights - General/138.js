////////////////////////////////////////////////////////////////////////////////
// File name   : 138.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init138()
{
//alert('Init Start: '+oFXP.tr);
	fxf_fn_updateReferences(null);
}

function fxf_fn_saveElement138(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if((element.id == 'Rechte_Ref') || (element.id == 'Rechte_ReferenzID') || (element.id == 'ID_Referenz'))
		fxf_fn_updateReferences(element.id);

	return 0;
}

function fxf_fn_updateReferences(eid)
{
	var frcat=fxf_fn_getElement('Rechte_Ref');
	if(frcat)
	{
		var tro=3;
		var rcat=parseInt(fxf_fn_getSelectedValue(frcat).value);
	}
	else
	{
		var tro=1;
		var rcat=parseInt($('temp_rcat').value);
	}

	var trfld=$('tro'+tro);
	var frfld=fxf_fn_getElement('Rechte_ReferenzID');
	var rfld=-1;
	if(eid == 'Rechte_Ref')
		rfld=0;
	else if(frfld)
		rfld=parseInt(fxf_fn_getSelectedValue(frfld).value);

	var trid=$('tro'+(tro+2));
	var frid=fxf_fn_getElement('ID_Referenz');
	var rid=-1;
	if((eid == 'Rechte_Ref') || (eid == 'Rechte_ReferenzID'))
		rid=0;
	else if(frid)
		rid=parseInt(fxf_fn_getSelectedValue(frid).value);

	var fbtn=$('Button_Anzeigen');
//alert('fxf_fn_updateReferences('+eid+')\n---------------------------------------\n\nfrcat='+frcat+'\n->rcat='+rcat+'\n\ntrfld='+trfld+'\nfrfld='+frfld+'\n->rfld='+rfld+'\n\ntrid='+trid+'\nfrid='+frid+'\n->rid='+rid+'\n\nfbtn='+fbtn);

	// Hide or display "Referenz Folder"?
	if(trfld)
	{
		if(rcat == 0)
		{
			trfld.style.display='none';
			rfld=0;
			rid=0;
			fxf_fn_changeSelectedValue(frfld,0,true);
		}
		else
		{
			trfld.style.display='';
			if(frcat)
				$('lit_Rechte_ReferenzID').innerHTML='<b>'+fxf_fn_getSelectedValue(frcat).text+'</b>:<br />';
//			fxf_fn_changeSelectedValue(frfld,rfld,true);
		}
	}

	// Hide or display "Referenz ID"?
	if(rcat == 35)	// Reports
	{
		trid.style.display='none';
		rid=rfld;
	}
	else if(trid)
	{
		if(rfld == 0)
		{
			trid.style.display='none';
			rid=0;
			fxf_fn_changeSelectedValue(frid,0,true);
		}
		else
		{
			trid.style.display='';
//			fxf_fn_changeSelectedValue(frid,rid,true);
		}
	}

	// Hide or display "Button"
	if(fbtn)
	{
		if(rcat && rfld && rid)
			fbtn.style.display='';
		else
			fbtn.style.display='none';
	}

	// Hide "Rights Mask" and "Save"
	if(!rcat || !rfld || !rid || ((eid !== undefined) && eid && eid.length))
	{
		oID.fxbuttons.style.display='none';
		var trm=$('mt351_rahmen');
		if(trm)
			trm.style.display='none';
	}

	if(rcat && ((eid == 'Rechte_Ref') || (rfld && (eid == 'Rechte_ReferenzID'))))
	{
		var url=fxf_fn_gProgram('rrefs', 'eid='+eid+'&rcat='+rcat+'&rfld='+rfld+'&rid='+rid+fxf_fn_gParam());
//alert('url='+url);
		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ret=transport.responseText;
//alert('ret='+ret);
				if(eid == 'Rechte_Ref')
				{
					fxf_fn_setNewSelects(frfld,ret,true);
					rfld=parseInt(fxf_fn_getSelectedValue(frfld).value);
					rid=0;
				}
				else
				{
					fxf_fn_setNewSelects(frid,ret,true);
					rid=parseInt(fxf_fn_getSelectedValue(frid).value);
				}

				fxf_fn_waitScreen('');
			}
		});
	}
}
