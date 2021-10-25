////////////////////////////////////////////////////////////////////////////////
// File name   : 023.js                                                       //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init23()
{
//alert('Init Start: '+oFXP.tr);
	tSet.cpid=0;
	var pie=$('Projekt_IDopen');
	if(pie)
		tSet.cpid=parseInt(fxf_fn_getSelectedValue(pie).value);
	tSet.pval=0;
	tSet.minstart='00000000';
	tSet.maxstart='99999999';
	tSet.minend='00000000';
	tSet.maxend='99999999';
	var dmm=$('dmmdates');
	if(dmm)
	{
		var dmma=dmm.innerHTML.split(',');
		tSet.minstart=dmma[0];
		tSet.maxstart=dmma[1];
		tSet.minend=dmma[2];
		tSet.maxend=dmma[3];
//alert('dmma='+dmma);
	}
	tSet.prefix='';
	var pfi=$('pprefix');
	if(pfi)
		tSet.prefix=pfi.value;
	fxf_fn_changePType();
}

function fxf_fn_saveElement23(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if((oFXP.action == 3) || (oFXP.action == 4))
	{
		if(element.id == 'Projekt_IDopen')
			fxf_fn_checkPID(element);
	}
	if((oFXP.action == 2) || (oFXP.action == 3))
	{
		if(element.id == 'fpp_fproj')
			fxf_fn_changeFP(element);

		if(element.id == 'fpp_fmodell')
			fxf_fn_changeFPScheme(element);
	}
//alert('element.id='+element.id);
	// Contact person has changed - adjust prefix
	if(element.id.substr(0,15) == 'Ansprechpartner')
	{
		fxf_fn_changePrefix(element);
		fxf_fn_changePD.delay(oFXP.mdelay, element);
	}
	else if(element.id == 'Aufwand_Soll')
	{
		var npval=fxf_fn_float2string(Math.max(0.0, fxf_fn_string2float(element.fxv)));
		var npunit=fxf_fn_getSelectedValue($('Zeiteinheit_Aufw_S')).value;
//alert('npval='+npval+', npunit='+npunit);
		var npsec=fxf_fn_unit2sec(npval,npunit);
		sunit[element.id]=npsec;
		element.value=fxf_fn_sec2unit(npsec,npunit);
		element.fxv=element.value;
//alert('npval='+npval+', npunit='+npunit+', npsec='+npsec);

		if(oSet.adjust_meffort == 'j')
		{
			var mtf=fxf_fn_getField($('Max_Zeit_Aufw'),true);
			var nmval=Math.max(0.0, fxf_fn_string2float(mtf.value));
			if(nmval > 0.0)
			{
				var nmunit=fxf_fn_getSelectedValue($('Zeiteinheit_max_Zeitaufw')).value;
				var nmsec=fxf_fn_unit2sec(fxf_fn_float2string(nmval),nmunit);
				sunit['Max_Zeit_Aufw']=Math.max(npsec,nmsec);
				mtf.value=fxf_fn_sec2unit(sunit['Max_Zeit_Aufw']);
				mtf.fxv=mtf.value;
				fxf_fn_saveFields.delay(oFXP.sdelay, 'Max_Zeit_Aufw=tx'+mtf.value, 'frm='+mtf.attributes['fxform'].value);
//alert('nmval='+nmval+', nmunit='+nmunit+', nmsec='+nmsec);
			}
		}
		if((oSet.cost_see > 0) && (oSet.cost_btype != 'N'))
		{
			var href=fxf_fn_gProgram('popup_ppsp', 'md=0&sf=1&'+fxf_fn_ppsp_param()+fxf_fn_gParam());
//alert('href='+href);
			var pop=fxf_fn_PPSPPopup(href,-1,0);
		}
	}
	else if(element.id == 'Max_Zeit_Aufw')
	{
		var nmval=Math.max(0.0, fxf_fn_string2float(element.fxv));
		if(nmval > 0.0)
		{
			var npval=fxf_fn_float2string(Math.max(0.0, fxf_fn_string2float(element.fxv)));
			var npunit=fxf_fn_getSelectedValue($('Zeiteinheit_Aufw_S')).value;
			var npsec=fxf_fn_unit2sec(npval,npunit);

			var nmunit=fxf_fn_getSelectedValue($('Zeiteinheit_max_Zeitaufw')).value;
			var nmsec=fxf_fn_unit2sec(fxf_fn_float2string(nmval),nmunit);
			sunit['Max_Zeit_Aufw']=Math.max(npsec,nmsec);
			element.value=fxf_fn_sec2unit(sunit['Max_Zeit_Aufw']);
			element.fxv=element.value;
			fxf_fn_saveFields.delay(oFXP.sdelay, 'Max_Zeit_Aufw=tx'+element.value, 'frm='+element.attributes['fxform'].value);
//alert('nmval='+nmval+', nmunit='+nmunit+', nmsec='+nmsec);
		}
	}
	else if((element.id == 'budget_pers_int_gepl') || (element.id == 'budget_pers_ext_gepl'))
	{
		element.value=fxf_fn_float2string(fxf_fn_string2float(element.fxv));
		element.fxv=element.value;
		var sf=2;
		if(element.id == 'budget_pers_ext_gepl')
			sf=3;
		var href=fxf_fn_gProgram('popup_ppsp', 'md=0&sf='+sf+'&'+fxf_fn_ppsp_param()+fxf_fn_gParam());
//alert('href='+href);
		var pop=fxf_fn_PPSPPopup(href,-1,0);
	}
	else if(element.id == 'Personen_ID')
	{
		var fs='';
		var form='';
		var hr=fxf_fn_selectedValue(element.id,element.fxv,false);

		var el=fxf_fn_getField($('EK'),true);
		if(el)
		{
			if(!form.length)
				form=el.attributes['fxform'].value;
			if(hr.length > 2)
				el.value=fxf_fn_float2string(hr[2]);
			else
				el.value=fxf_fn_float2string(0.0);
			el.fxv=el.value;
			if(fs.length)
				fs += '&';
			fs += 'EK=tx'+el.fxv;
		}

		el=fxf_fn_getField($('VK'),true);
		if(el)
		{
			if(!form.length)
				form=el.attributes['fxform'].value;
			if(hr.length > 3)
				el.value=fxf_fn_float2string(hr[3]);
			else
				el.value=fxf_fn_float2string(0.0);
			el.fxv=el.value;
			if(fs.length)
				fs += '&';
			fs += 'VK=tx'+el.fxv;
		}

		if(fs.length)
			fxf_fn_saveFields(fs, 'frm='+form);
	}
	else if(element.id == 'Projektart')
		fxf_fn_changePType();
	else if((element.id == 'Projektmanager') || (element.id == 'Projektleiter') || (element.id == 'Vertreter') || (element.id == 'Kategorie_ID'))
		fxf_fn_changePD.delay(oFXP.mdelay, element);
	else if((element.id == 'Soll_Beg_Dtm') || (element.id == 'Soll_End_Dtm'))
		fxf_fn_checkDMMDates(element);

	return 0;
}
