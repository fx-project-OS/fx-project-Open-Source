////////////////////////////////////////////////////////////////////////////////
// File name   : 217.js                                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2021-04-08                                                   //
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
 * PF 217: Form Designer - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

function fxf_fn_init217()
{
//alert('Init Start: '+oFXP.tr);
	tSet.oldconfig='';
	var tvl=$('Text_varLang');
	if(tvl)
		tSet.oldconfig=tvl.value;

	var colorNode=$('conf_color_input');
	if(colorNode)
	{
        var parent = colorNode.parentNode;
        var prevNode = colorNode.previousSibling;
        var selector = fxf_fn_makeColorSelector(colorNode);
        parent.insertBefore(selector, (prevNode ? prevNode.nextSibling : null));
	}
}

function fxf_fn_saveElement217(element)
{
//alert('JS: saveElement'+oFXP.tr);
	fxf_fn_ajaxSaveElement(element);

	if((element.id == 'mordner') || (element.id == 'ktyp'))
		fxf_fn_configUpdateSelectFile();
	else if(element.id == 'conf_unit_mm')
	{
		var mm = fxf_fn_string2float(element.value);
		var pt = mm * 2.83501683501;
		element.value=fxf_fn_float2string(mm);
		$('conf_unit_pdf').value = pt;
	}
	else if(element.id == 'conf_unit_pdf')
	{
		var pt = fxf_fn_string2float(element.value);
		var mm = pt / 2.83501683501;
		element.value=pt;
		$('conf_unit_mm').value = fxf_fn_float2string(mm);
	}

	return 0;
}

function fxf_fn_configClose(div)
{
	$('conf_'+div+'_div').style.display='none';
}

function fxf_fn_configUpdateSelectFile()
{
	id_ctype=fxf_fn_getField($('ktyp'),true);
	id_cfile=fxf_fn_getField($('kdatei'),true);

	// Change select
	if(id_ctype && id_cfile)
	{
		fxf_fn_waitScreen('#');
		var i=0;
		while(true)
		{
			id_cfolder=document.getElementsByName('mordner')[i];
			if(id_cfolder)
			{
				if(id_cfolder.checked)
					break;
				i++;
			}
			else
				break;
		}

		var url=fxf_fn_gProgram('config_select', '');
		if(id_cfolder)
			url += '&mid='+parseInt(id_cfolder.value);
		url += '&ktyp='+parseInt(fxf_fn_getSelectedValue(id_ctype).value)+fxf_fn_gParam();
//alert('url='+url);

		new Ajax.Request(url,
		{
			method:'get', asynchronous:true,
			onSuccess: function(transport)
			{
				var ds=transport.responseText;
				fxf_fn_changeSelectedValue(id_cfile,ds,true);
				if(achg)
					fxf_fn_waitScreen('');
			}
		});

		fxf_fn_configChangeSelect.delay(oFXP.mdelay, 0);
	}
}

function fxf_fn_configChangeSelect(val)
{
	id_config=$('Text_varLang');
	if(id_config && !val && (id_config.value != tSet.oldconfig))
	{
		fxf_fn_waitScreen('');
		fxf_fn_question('', tSet.savechanges, [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_configChangeSelect(1);', 'fxf_fn_configChangeSelect(-1);'], 100);
	}
	if(val)
	{
		$('force_save').value=val;
		if(val > 0)
			fxf_fn_loadTR.delay(oFXP.sdelay, oFXP.tr,'Button_Speichern');
		else
			fxf_fn_loadTR.delay(oFXP.sdelay, oFXP.tr,'Reload');
	}
}

function fxf_fn_configDeletePic(delpic)
{
	fxf_fn_waitScreen('');
	fxf_fn_question('', tSet.deletepicture, [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_waitScreen(\'deleting\');fxf_fn_configIntegrate(\''+delpic+'\')', ''], 100);
}

function fxf_fn_configDeleteConf()
{
	fxf_fn_waitScreen('');
	fxf_fn_question('', tSet.deleteconfig, [fxf_fn_getText(10), fxf_fn_getText(11)], ['fxf_fn_waitScreen(\'deleting\');fxf_fn_loadTR('+oFXP.tr+',\'delete\')', ''], 100);
}

function fxf_fn_configIntegrate(delpic)
{
	var i=0;
	while(true)
	{
		id_cfolder=document.getElementsByName('mordner')[i];
		if(id_cfolder)
		{
			if(id_cfolder.checked)
				break;
			i++;
		}
		else
			break;
	}

	id_ctype=fxf_fn_getField($('ktyp'),true);

	var url=fxf_fn_gProgram('config_integrate', 'ajax=1&ktyp='+parseInt(fxf_fn_getSelectedValue(id_ctype).value)+fxf_fn_gParam());
	if(id_cfolder)
		url += '&mordner='+parseInt(id_cfolder.value);
	if(delpic.length)
		url += '&delpic='+delpic;
//alert('url='+url);

	new Ajax.Request(url,
	{
		method:'get', asynchronous:true,
		onSuccess: function(transport)
		{
			var ds=transport.responseText;
			$('conf_integrate_pics').innerHTML=ds;
			$('conf_integrate_div').style.display='';
			if(delpic.length)
				$('conf_status_div').innerHTML=tSet.deleteok;
			fxf_fn_waitScreen('');
		},
		onFailure: function()
		{
			fxf_fn_waitScreen('');
		}
	});
}