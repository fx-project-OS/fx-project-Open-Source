////////////////////////////////////////////////////////////////////////////////
// File name   : 011.js                                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2020-10-15                                                   //
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
 * PF 11: Project Categories - JavaScript function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

function fxf_fn_init11()
{
//alert('Init Start: '+oFXP.tr);
}

function fxf_fn_prjcatGroupAdd()
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
		return;

	var tg = $('tg');
	if(tg)
	{
		var gc = 1;
		while($('t'+gc))
			gc++;
fxf_fn_writeDebug('info', 'fxf_fn_prjcatGroupAdd(): tg='+tg+', gc='+gc);

		var gheight = Math.ceil(tSet.cheight/2) + 2*tSet.cheight;

		var dt = new Element('div', {id:'t'+gc, style:'position:relative;left:0;top:12px;width:'+(tSet.cwidth*2 + 4*tSet.cpadd + tSet.cspace)+'px;height:'+gheight+'px;margin-bottom:8px;box-shadow:2px 2px 6px rgba(0,0,0,0.10);'});
		tg.insert(dt);

		var dg = new Element('div', {id:'g'+gc, style:'position:absolute;left:2px;top:8px;width:'+(tSet.cwidth + 2*tSet.cpadd)+'px;height:'+tSet.cheight+'px;z-index:128;'});
		var h = '	<input class="fxftx" type="text" id="gp'+gc+'" name="gp'+gc+'" tabindex="0" fxform="'+tSet.form+'" value="9999" style="width:45px;">';
		h += '	<input class="fxftx" type="text" id="gg'+gc+'" name="gg'+gc+'" tabindex="0" fxform="'+tSet.form+'" value="" tooltip="'+tSet.gcol+'" style="background:url(GFX/pcb.png) no-repeat top left;width:10px;text-align:center;cursor:pointer;" onclick="fxf_fn_prjcatChangeBG('+gc+');">';
		h += '	<input class="fxftx" type="text" id="gv'+gc+'" name="gv'+gc+'" tabindex="0" fxform="'+tSet.form+'" value="('+tSet.empty+')" maxlength="'+tSet.maxlength+'" style="width:'+(tSet.cwidth-64)+'px;">';
		h += '	'+fxf_fn_fieldHidden('gi'+gc, '-1', tSet.form);
		h += '	'+fxf_fn_fieldHidden('gm'+gc, '-1', tSet.form);
		h += '	<div id="gs'+gc+'" style="position:absolute;right:30px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('+tSet.imgpath+'icso.png);" onclick="fxf_fn_prjcatRedraw('+gc+',0);" tooltip="'+tSet.gsrt+'"></div>';
		h += '	<div id="gd'+gc+'" style="position:absolute;right:10px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('+tSet.imgpath+'icde.png);" onclick="fxf_fn_prjcatGroupDel('+gc+');" tooltip="'+tSet.gdel+'"></div>';
		dg.innerHTML  = h;

		dt.insert(dg);

		fxf_fn_saveFields('gp'+gc+'=tx9999&gg'+gc+'=tx&gv'+gc+'=tx('+tSet.empty+')&gi'+gc+'=tx-1&gm'+gc+'=tx-1', 'frm='+tSet.form);

		fxf_fn_prjcatRedraw(gc, 0);
	}
}

function fxf_fn_prjcatGroupDel(gc)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
		return;
fxf_fn_writeDebug('info', 'fxf_fn_prjcatGroupDel('+gc+')');

	var tg = $('tg');
	if(tg)
	{
		var sf = '';
		var td = $('t'+gc);
		var gua = parseInt($('gua').value);

		var ca = []; ca.length = 0;
		var cn = 0;
		var cc = 1;
		while($('c'+gc+'_'+cc))
		{
			if(sf.length)
				sf += '&';
			sf += 'cp'+gc+'_'+cc+'=un&cg'+gc+'_'+cc+'=un&cv'+gc+'_'+cc+'=un&ci'+gc+'_'+cc+'=un&cm'+gc+'_'+cc+'=un&cu'+gc+'_'+cc+'=un';

			var mid = parseInt($('cm'+gc+'_'+cc).value);
			var prj = parseInt($('cu'+gc+'_'+cc).value);
			if((mid == 0) || (prj > 0))
			{
				ca[cn]			= {};

				ca[cn]['pos']	= parseInt($('cp'+gc+'_'+cc).value);
				ca[cn]['bg']	= parseInt($('cg'+gc+'_'+cc).value);
				ca[cn]['value']	= $('cv'+gc+'_'+cc).value;
				ca[cn]['id']	= parseInt($('ci'+gc+'_'+cc).value);
				ca[cn]['mid']	= mid;
				ca[cn]['prj']	= prj;

				cn++;
			}

			cc++;
		}
//alert('Del group ['+gc+']\n\ngua='+gua+', ca.length='+ca.length);

		tg.removeChild(td);

		if(sf.length)
			sf += '&';
		sf += 'gp'+gc+'=un&gg'+gc+'=un&gv'+gc+'=un&gi'+gc+'=un&gm'+gc+'=un&gua'+gc+'=un';
		fxf_fn_saveFields(sf, 'frm='+tSet.form);

		if(ca.length)
		{
			for(var c=0; c<ca.length; c++)
				fxf_fn_prjcatRedraw(gua, ca[c].pos+'|'+ca[c].bg+'|'+ca[c].value+'|'+ca[c].id+'|'+ca[c].mid+'|'+ca[c].prj);
		}
	}
}

function fxf_fn_prjcatAdd(gc)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
		return;
fxf_fn_writeDebug('info', 'fxf_fn_prjcatAdd('+gc+')');

	fxf_fn_prjcatRedraw(gc, 1);
}

function fxf_fn_prjcatDel(gc, cc)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
		return;
fxf_fn_writeDebug('info', 'fxf_fn_prjcatDel('+gc+', '+cc+')');

	fxf_fn_prjcatRedraw(gc, -cc);
}

function fxf_fn_prjcatRedraw(gc, mode)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
		return;
fxf_fn_writeDebug('info', 'fxf_fn_prjcatRedraw('+gc+', mode='+mode+')');

	var ca		= []; ca.length = 0;
	var cs		= []; cs.length = 0;
	var cnt_b	= 0;
	var cnt_a	= 0;
	var dt		= $('t'+gc);
	var dc		= $('c'+gc);

	var sf		= '';

	if(dc)
	{
		var gcc = 1;
		while($('c'+gc+'_'+gcc))
		{
			cnt_b++;
			if(mode != -gcc)
			{
				ca[cnt_a]			= {};

				ca[cnt_a]['pos']	= parseInt($('cp'+gc+'_'+gcc).value);
				ca[cnt_a]['bg']		= $('cg'+gc+'_'+gcc).value;
				ca[cnt_a]['value']	= $('cv'+gc+'_'+gcc).value;
				ca[cnt_a]['id']		= parseInt($('ci'+gc+'_'+gcc).value);
				ca[cnt_a]['mid']	= parseInt($('cm'+gc+'_'+gcc).value);
				ca[cnt_a]['prj']	= parseInt($('cu'+gc+'_'+gcc).value);

				if(!ca[cnt_a]['pos'])
					ca[cnt_a]['pos'] = 0;

				cnt_a++;
			}
			else
			{
				if(sf.length)
					sf += '&';
				sf += 'cp'+gc+'_'+gcc+'=un&cg'+gc+'_'+gcc+'=un&cv'+gc+'_'+gcc+'=un&ci'+gc+'_'+gcc+'=un&cm'+gc+'_'+gcc+'=un&cu'+gc+'_'+gcc+'=un';
			}

			gcc++;
		}

		var cmode = parseInt(mode);
		if(cmode && (cmode > 0))
		{
			for(var i=0; i<mode; i++)
			{
				ca[cnt_a]			= {};

				ca[cnt_a]['pos']	= 9999;
				ca[cnt_a]['bg']		= '';
				ca[cnt_a]['value']	= '('+tSet.empty+')';
				ca[cnt_a]['id']		= -1;
				ca[cnt_a]['mid']	= -1;
				ca[cnt_a]['prj']	= 0;

				cnt_a++;
			}
		}
		cmode = (mode+'').indexOf('|');
		if(cmode > -1)
		{
			cmode = (mode+'').split('|');

			ca[cnt_a]			= {};

			ca[cnt_a]['pos']	= parseInt(cmode[0]);
			ca[cnt_a]['bg']		= cmode[1];
			ca[cnt_a]['value']	= cmode[2];
			ca[cnt_a]['id']		= parseInt(cmode[3]);
			ca[cnt_a]['mid']	= parseInt(cmode[4]);
			ca[cnt_a]['prj']	= parseInt(cmode[5]);

			cnt_a++;
		}
//alert('cnt_b='+cnt_b+', cnt_a='+cnt_a+', ca.length='+ca.length);

		if(cnt_a > 0)
		{
			for(var i=0; i<cnt_a; i++)
			{
				var fi = -1;
				var sp = -1;
				var sv = '';

				for(var s=0; s<cnt_a; s++)
				{
					var st = false;

					if(cs.length)
					{
						for(var f=0; f<cs.length; f++)
						{
							if(cs[f] == s)
								st = true;
						}
					}

					if(!st)
					{
						if((sp == -1) || (ca[s].pos < sp) || ((ca[s].pos == sp) && (ca[s].value.toLowerCase() < sv)))
						{
							fi = s;
							sp = ca[s].pos;
							sv = ca[s].value.toLowerCase();
						}

						cs[i] = fi;
					}
				}
//alert(i+' -> '+cs[i]+':\n\npos='+ca[cs[i]].pos+', value='+ca[cs[i]].value);
			}
		}

		dt.removeChild(dc);
	}

	if(cnt_b != cnt_a)
		dt.style.height = (parseInt(dt.style.height) + (cnt_a-cnt_b)*tSet.cheight)+'px';

	var ctop = tSet.ctop;
	var cc = 1;

	ndc = new Element('div', {id:'c'+gc, style:'position:relative;left:0;top:0;'});
	dt.insert(ndc);

	if(cs.length)
	{
		for(var s=0; s<cs.length; s++)
		{
			var nvalue		= ca[cs[s]].value.replace(/\&/g, '&amp;').replace(/\"/g, '&quot;');

			var ct			= ' onclick="fxf_fn_prjcatDel('+gc+','+cc+');" tooltip="'+tSet.cdel+'"';
			var cursor		= 'pointer';
			var style		= '';
			var disabled	= false;
			if((ca[cs[s]].mid == 0) || (ca[cs[s]].prj > 0))
			{
				ct			= ' tooltip="'+tSet.fuse+'"';
				cursor		= 'help';
				style		= 'opacity:0.50;';
				if(ca[cs[s]].mid == 0)
				{
					ct			= ' tooltip="'+tSet.fdel+'"';
					disabled	= true;
				}
			}

			var dcn = new Element('div', {id:'c'+gc+'_'+cc, style:'position:relative;left:0;top:0;'});
			dcn.innerHTML  = '	<div id="cb'+gc+'_'+cc+'" style="position:absolute;left:0;top:'+ctop+'px;width:'+tSet.cwidth+'px;height:'+tSet.cheight+'px;"></div><div id="cl'+gc+'_'+cc+'" style="position:absolute;left:'+((tSet.cwidth + 2*tSet.cpadd)/2)+'px;top:'+(ctop-12)+'px;width:'+((tSet.cwidth + 2*tSet.cpadd)/2 - 22)+'px;height:'+tSet.cheight+'px;border-left:1px dotted #bbbbbb;border-bottom:1px dotted #bbbbbb;z-index:64;"></div>';
			ndc.insert(dcn);

			var dcc = new Element('div', {id:'cc'+gc+'_'+cc, style:'position:absolute;left:'+(tSet.cwidth + 2*tSet.cpadd + tSet.cspace)+'px;top:'+ctop+'px;'});
			dcc.innerHTML  = '	<div id="cr'+gc+'_'+cc+'" style="position:absolute;left:-22px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('+tSet.imgpath+'icmo.png);" onclick="fxf_fn_prjcatMove('+gc+','+cc+',event);" tooltip="'+tSet.cmov+'"></div>';
			dcn.insert(dcc);

			if(sf.length)
				sf += '&';

			var dca = new Element('div', {id:'ca'+gc+'_'+cc, style:'position:absolute;left:0;top:0;width:'+(tSet.cwidth + 2*tSet.cpadd)+'px;'});
			var h = '';
			if(disabled)
			{
				h = '	<input class="fxftxg" type="text" id="cp'+gc+'_'+cc+'" name="cp'+gc+'_'+cc+'" fxform="'+tSet.form+'" value="'+ca[cs[s]].pos+'" readonly="readonly" disabled style="width:45px;"> <input class="fxftx" type="text" id="cg'+gc+'_'+cc+'" name="cg'+gc+'_'+cc+'" fxform="'+tSet.form+'" value="'+ca[cs[s]].bg+'" style="background:'+fxf_fn_prjcatGetBG(ca[cs[s]].bg,disabled)+';width:10px;text-align:center;"> <input class="fxftxg" type="text" id="cv'+gc+'_'+cc+'" name="cv'+gc+'_'+cc+'" fxform="'+tSet.form+'" value="'+nvalue+'" maxlength="'+tSet.maxlength+'" readonly="readonly" disabled style="width:'+(tSet.cwidth-64)+'px;">';

				sf += 'cp'+gc+'_'+cc+'=un&cg'+gc+'_'+cc+'=un&cv'+gc+'_'+cc+'=un';
			}
			else
			{
				h = '	<input class="fxftx" type="text" id="cp'+gc+'_'+cc+'" name="cp'+gc+'_'+cc+'" tabindex="0" fxform="'+tSet.form+'" value="'+ca[cs[s]].pos+'" style="width:45px;"> <input class="fxftx" type="text" id="cg'+gc+'_'+cc+'" name="cg'+gc+'_'+cc+'" tabindex="0" fxform="'+tSet.form+'" value="'+ca[cs[s]].bg+'" tooltip="'+tSet.ccol+'" style="background:'+fxf_fn_prjcatGetBG(ca[cs[s]].bg,disabled)+';width:10px;text-align:center;cursor:pointer;" onclick="fxf_fn_prjcatChangeBG('+gc+','+cc+');"> <input class="fxftx" type="text" id="cv'+gc+'_'+cc+'" name="cv'+gc+'_'+cc+'" fxform="'+tSet.form+'" value="'+nvalue+'" maxlength="'+tSet.maxlength+'" style="width:'+(tSet.cwidth-64)+'px;">';

				sf += 'cp'+gc+'_'+cc+'=tx'+ca[cs[s]].pos+'&cg'+gc+'_'+cc+'=tx'+ca[cs[s]].bg+'&cv'+gc+'_'+cc+'=tx'+fxf_fn_escapeText(nvalue,false);
			}
			h += '	'+fxf_fn_fieldHidden('ci'+gc+'_'+cc, ca[cs[s]].id, tSet.form);
			h += '	'+fxf_fn_fieldHidden('cm'+gc+'_'+cc, ca[cs[s]].mid, tSet.form);
			h += '	'+fxf_fn_fieldHidden('cu'+gc+'_'+cc, ca[cs[s]].prj, tSet.form);
			h += '	<div id="cd'+gc+'_'+cc+'" style="position:absolute;right:10px;top:4px;width:17px;height:16px;cursor:'+cursor+';background-image:url('+tSet.imgpath+'icde.png);'+style+'"'+ct+'></div>';
			dca.innerHTML = h;
			dcc.insert(dca);

			sf += '&ci'+gc+'_'+cc+'=tx'+ca[cs[s]].id+'&cm'+gc+'_'+cc+'=tx'+ca[cs[s]].mid+'&cu'+gc+'_'+cc+'=tx'+ca[cs[s]].prj;

			ctop += tSet.cheight;
			cc++;
		}
	}

	var da = new Element('div', {id:'c'+gc+'_a', style:'position:relative;left:0;top:0;'});
	da.innerHTML  = '	<div id="cb'+gc+'_a" style="position:absolute;left:0;top:'+ctop+'px;width:'+tSet.cwidth+'px;height:'+tSet.cheight+'px;"></div><div id="cl'+gc+'_a" style="position:absolute;left:'+((tSet.cwidth + 2*tSet.cpadd)/2)+'px;top:'+(ctop-12)+'px;width:'+((tSet.cwidth + 2*tSet.cpadd)/2 - 22)+'px;height:'+tSet.cheight+'px;border-left:1px dotted #bbbbbb;border-bottom:1px dotted #bbbbbb;z-index:64;"></div>';
	ndc.insert(da);

	var cca = new Element('div', {id:'cc'+gc+'_a', style:'position:absolute;left:'+(tSet.cwidth + 2*tSet.cpadd + tSet.cspace)+'px;top:'+ctop+'px;'});
	cca.innerHTML = '	<div id="ca'+gc+'_a" style="position:absolute;left:-22px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('+tSet.imgpath+'icad.png);" onclick="fxf_fn_prjcatAdd('+gc+');" tooltip="'+tSet.cadd+'"></div>';
	da.insert(cca);

	if(sf.length)
		fxf_fn_saveFields(sf, 'frm='+tSet.form);
}

function fxf_fn_prjcatMove(gc, cc, ev)
{
	var prjcatpc = $('prjcatpc');
	if(prjcatpc)
	{
		fxf_fn_prjcatClose();
		return;
	}
fxf_fn_writeDebug('info', 'fxf_fn_prjcatMove('+gc+','+cc+')');

	var tg = $('tg');
	var ga = []; ga.length = 0;
	var gac = 0;
	var cnl = tg.childNodes.length;
	for(var c=0; c<cnl; c++)
	{
		var cid = tg.childNodes[c]['id'];
		if(cid && cid.length && (cid.substr(0, 1) == 't'))
		{
			var dgc = parseInt(cid.substr(1));
			if(dgc != gc)
			{
				ga[gac] = dgc;
				gac++;
			}
		}
	}

	if(ga.length)
	{
		if(ga.length == 1)
			fxf_fn_prjcatMoveIt(gc, cc, ga[0]);
		else
		{
			var dleft 	= mcx-12;
			var dtop  	= mcy;
			var dwidth	= 400;
			var dheight	= 70 + ga.length*18;

			if(dtop+dheight+50 > dim.sd.pheight)
				dtop -= (dheight-20);
			else
				dtop -= 20;

			var spos = parseInt($('cp'+gc+'_'+cc).value);
			var sval = $('cv'+gc+'_'+cc).value;

			oID.iact.style.display='';

			var cont = new Element('div', {id:'prjcatpc', style:'position:absolute;left:'+dleft+'px;top:'+dtop+'px;width:auto;height:auto;border:0;background-color:transparent;'});
			$(document.body).insert(cont);

			var url=fxf_fn_gProgram('prj_kategorie', 'language='+oFXP.lang+'&gc='+gc+'&cc='+cc+fxf_fn_gParam());
			var mg='';
			for(var g=0; g<ga.length; g++)
			{
				if(mg.length)
					mg += '~';
				mg += ga[g]+'|'+parseInt($('gp'+ga[g]).value)+'|'+($('gv'+ga[g]).value);
			}
//alert(url+'\n\npost:\nspos='+spos+'\nsval='+sval+'\nmg='+mg);
			new Ajax.Request(url,
			{
				method:'post', postBody:'spos='+spos+'&sval='+escape(sval)+'&mg='+escape(mg), asynchronous:true,

				onSuccess: function(transport)
				{
					cont.innerHTML = transport.responseText;
				},

				onFailure: function(failure)
				{
					fxf_fn_prjcatClose();
				}
			});

		}
	}
}

function fxf_fn_prjcatMoveIt(sgc, scc, dgc)
{
	fxf_fn_prjcatClose();

	var pos = parseInt($('cp'+sgc+'_'+scc).value);
	var bg	= $('cg'+sgc+'_'+scc).value;
	var val = $('cv'+sgc+'_'+scc).value;
	var id  = parseInt($('ci'+sgc+'_'+scc).value);
	var mid = parseInt($('cm'+sgc+'_'+scc).value);
	var prj = parseInt($('cu'+sgc+'_'+scc).value);

	fxf_fn_prjcatRedraw(sgc, -scc);

	fxf_fn_prjcatRedraw(dgc, pos+'|'+bg+'|'+val+'|'+id+'|'+mid+'|'+prj);
}