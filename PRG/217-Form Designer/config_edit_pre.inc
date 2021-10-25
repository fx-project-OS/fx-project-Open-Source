<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_edit_pre.inc                                          //
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
 * PF 217: Form Designer - Edit Configuration File
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

setForm(true);

// Konfigurationsdatei (kdatei)
$kdatei=$_POST['kdatei'];
if(substr($kdatei, 0, 1) == '_')
	$kdatei=substr($kdatei,1);
$ddatei=$kdatei;
$inact=false;
$kcol='black';
if(substr($kdatei,-5,1) == '!')
{
	$inact=true;
	$kcol='red';
	$ddatei=substr($kdatei,0,-5).substr($kdatei,-4);
}
$maskenfeldwerte['kdatei']='~<font class="'.$kcol.'">'.$ddatei.'</font>';
// Icons, falls Konfigurationsdatei existiert
if(($_POST['mordner'] != '0') && file_exists($kdatei))
{
	$ita=explode('|',meldung(2046,false));	// Konfigurationsdatei deaktivieren|Konfigurationsdatei reaktivieren|Konfigurationsdatei löschen
	// Reaktivierungs-Icon
	if($inact)
		$maskenfeldwerte['kdatei'] .= '<img id="reaconf" src="'.$GLOBALS['gfxpath'].'icrs.png" align="top" tooltip="'.$ita[1].'" style="cursor:pointer;margin-left:12px;" '.fxf_jsFunction('loadTR', $GLOBALS['fxpglobals']['tr'], 'react').'>';	// Konfigurationsdatei reaktivieren
	// Deaktivierungs-Icon
	else
		$maskenfeldwerte['kdatei'] .= '<img id="inaconf" src="'.$GLOBALS['gfxpath'].'icri.png" align="top" tooltip="'.$ita[0].'" style="cursor:pointer;margin-left:12px;" '.fxf_jsFunction('loadTR', $GLOBALS['fxpglobals']['tr'], 'inact').'>';	// Konfigurationsdatei deaktivieren
	// Lösch-Icon
	$maskenfeldwerte['kdatei'] .= '<img id="delconf" src="'.$GLOBALS['gfxpath'].'icde.png" align="top" tooltip="'.$ita[2].'" style="cursor:pointer;margin-left:4px;" '.fxf_jsFunction('configDeleteConf').'>';	// Konfigurationsdatei löschen
}
$maskenfeldwerte['kdatei'] .= '<br /><br />';

// Hidden
$maskenfeldwerte['kdatei'] .= fieldHidden('cpath',$kdatei).fieldHidden('force_save',0);

// Werkzeugleiste (kbar)
$istyle = ' style="background:#cccccc;border:1px solid #bbbbbb;border-radius:4px;box-shadow:1px 1px 2px #666666;padding:2px;margin:2px;cursor:pointer;"';
$iwidth = 120;
$kbar  = '<table border="0" cellpadding="0" cellspacing="0">';
$kbar .= '<tr>';
// ...Bild hochladen
$kbar .= '<td><img id="ic_upload" src="'.$GLOBALS['gfxpath'].'cfg_upl.png"'.$istyle.'></td>';
$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[0].'</font></td>';
// ...Bild einbinden
$cs=6;
$kbar .= '<td><img id="ic_integrate" src="'.$GLOBALS['gfxpath'].'cfg_dnl.png"'.$istyle.'></td>';
$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[1].'</font></td>';
if($ktyp != 2018)
{
	// ...Kalkulator
	$cs -= 2;
	$kbar .= '<td><img id="ic_calculator" src="'.$GLOBALS['gfxpath'].'cfg_cal.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[2].'</font></td>';
	if($ktyp == 2021)
	{
		// ...Farbe
		$cs -= 2;
		$kbar .= '<td><img id="ic_color" src="'.$GLOBALS['gfxpath'].'cfg_col.png"'.$istyle.'></td>';
		$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[3].'</font></td>';
	}
}
$kbar .= '<td colspan="'.$cs.'">&nbsp;</td>';
$kbar .= '</tr>';
if($ktyp != 2018)
{
	$kbar .= '<tr>';
	// ... Bild
	$kbar .= '<td><img id="icg_pic" src="'.$GLOBALS['gfxpath'].'cfg_pic.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[4].'</font></td>';
	// ... Text
	$kbar .= '<td><img id="icg_txt" src="'.$GLOBALS['gfxpath'].'cfg_txt.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[5].'</font></td>';
	// ... Box
	$kbar .= '<td><img id="icg_box" src="'.$GLOBALS['gfxpath'].'cfg_box.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[6].'</font></td>';
	// ... Rechteck
	$kbar .= '<td><img id="icg_rec" src="'.$GLOBALS['gfxpath'].'cfg_rec.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[7].'</font></td>';
	// ... Linie
	$kbar .= '<td><img id="icg_lin" src="'.$GLOBALS['gfxpath'].'cfg_lin.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[8].'</font></td>';
	$kbar .= '</tr>';
}
$kbar .= '</table>';
$maskenfeldwerte['kbar'] = '~'.$kbar;

// Include integration file
$_GET['ktyp'] = $ktyp;
$_GET['mordner'] = $mordner;
$req_fn=fxf_fn_reqFilename('config_integrate');
if(strlen($req_fn))
	require($req_fn);

// Befehlszeile (Textzeile)
$litwidth=134;
$close_literal = 'Schliesen';
// ...Statuszeile
$befehl  = '<div id="conf_status_div" style="margin:2px 0;padding:2px 4px;text-align:left;box-shadow:1px 1px 4px #666666;padding:4px;display:block;">';
$befehl .= '	<font color="grey">'.get_text(1843, '10').'</font>';
$befehl .= '</div>';
// ...Bereich: Bild hochladen
$befehl .= '<div id="conf_upload_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
$befehl .= '		<tr>';
$befehl .= '			<td class="maskh" colspan="2" align="top" width="100%">';
$befehl .= '				<b>'.$text_icons[0].'</b>'.closeIcon(0,0, 36, 'cfg_upl_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'upload'));
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '		<tr>';
$befehl .= '			<td width="'.$litwidth.'" valign="top" nowrap><br />&nbsp;&nbsp;<span class="lit">'.get_text(1572, '15').':</span>&nbsp;</td>';	// Quelle (Datei)
$befehl .= '			<td valign="top" nowrap>';
$befehl .= '				<br />'.fieldFile('file',792).'<br /><br /><br />';
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '	</table>';
$befehl .= '</div>';
// ...Bereich: Bild einbinden
$befehl .= '<div id="conf_integrate_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
$befehl .= '		<tr>';
$befehl .= '			<td class="maskh" align="top" width="100%">';
$befehl .= '				<b>'.$text_icons[1].'</b>'.closeIcon(0,0, 36, 'cfg_dnl_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'integrate'));
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '		<tr>';
$befehl .= '			<td valign="top">';
$befehl .= '				<br /><div id="conf_integrate_pics" style="margin:0 6px;">';
$befehl .= $integrate;
$befehl .= '				</div><br />';
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '	</table>';
$befehl .= '</div>';

if($ktyp != 2018)
{
	// ...Bereich: Kalkulator
	$befehl .= '<div id="conf_calculator_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
	$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
	$befehl .= '		<tr>';
	$befehl .= '			<td class="maskh" colspan="2" align="top" width="100%">';
	$befehl .= '				<b>'.$text_icons[2].'</b>'.closeIcon(0,0, 36, 'cfg_cal_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'calculator'));
	$befehl .= '			</td>';
	$befehl .= '		</tr>';
	$befehl .= '		<tr>';
	$befehl .= '			<td valign="top" align="left" nowrap>';
	$befehl .= '				<br />&nbsp;&nbsp;<span class="lit">'.$unit_mm.':</span>&nbsp;&nbsp;'.fieldText('conf_unit_mm',get_entry(0, 'dezimal'),125);
	$befehl .= '				&nbsp;x&nbsp;2.83501683501&nbsp;&nbsp;&nbsp;';
	$befehl .= '			</td>';
	$befehl .= '			<td valign="top" align="right" nowrap>';
	$befehl .= '				<br /><span class="lit">'.$unit_pdf.':</span>&nbsp;&nbsp;'.fieldText('conf_unit_pdf', '0.00', 125).'&nbsp;&nbsp;<br /><br />';
	$befehl .= '			</td>';
	$befehl .= '		</tr>';
	$befehl .= '	</table>';
	$befehl .= '</div>';
	if($ktyp == 2021)
	{
		// ...Bereich: Farbe
		$befehl .= '<div id="conf_color_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
		$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
		$befehl .= '		<tr>';
		$befehl .= '			<td class="maskh" align="top" width="100%">';
		$befehl .= '				<b>'.$text_icons[3].'</b>'.closeIcon(0,0, 36, 'cfg_col_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'color'));
		$befehl .= '			</td>';
		$befehl .= '		</tr>';
		$befehl .= '		<tr>';
		$befehl .= '			<td valign="top" align="center" nowrap>';
		$befehl .= '				<br /><input id="conf_color_input" type="text" class="color" value="#006B9F"><br />';
		$befehl .= '			</td>';
		$befehl .= '		</tr>';
		$befehl .= '	</table>';
		$befehl .= '</div>';
	}
}
$maskenfeldwerte['textzeile'] = '~'.$befehl;

// Textarea (Text_varLang)
$konfig = '';
$ndatei = $kdatei;
if($err && $_POST['Text_varLang'])
	$konfig = $_POST['Text_varLang'];
if(!strlen($konfig) && file_exists($ndatei))
	$konfig = fxLoad($ndatei);
if(!strlen($konfig))
{
	$spos = strpos($kdatei, '/');
	if(($_POST['ktyp'] == 2019) || ($_POST['ktyp'] == 2021))	// 2019=Leistungsnachweis oder 2021=Rechnung
		$ndatei = $_POST['mordner'].substr($kdatei, $spos, 9).substr($kdatei, -4);
	else
		$ndatei = $_POST['mordner'].substr($kdatei, $spos);
	if(file_exists($ndatei))
		$konfig = fxLoad($ndatei);
	else
	{
		if(($_POST['ktyp'] == 2019) || ($_POST['ktyp'] == 2021))	// 2019=Leistungsnachweis oder 2021=Rechnung
			$ndatei = '0'.substr($kdatei, $spos, 9).substr($kdatei, -4);
		else
			$ndatei = '0'.substr($kdatei, $spos);
		if(file_exists($ndatei))
			$konfig = fxLoad($ndatei);
	}
}
// ...Neue Rechnung
if((($_POST['ktyp'] == 2019) || ($_POST['ktyp'] == 2021)) && (substr($_POST['kdatei'], 0, 1) == '_'))
{
	$formpos = strpos($konfig, '#form');
	if(!$formpos)
	{
		$add  = "// Formularname".$nl;
		$add .= "// ------------".$nl;
		$add .= "#form\t\tFormularname ".substr($kdatei, -7, 3).$nl.$nl;

		$konfig = $add.$konfig;
	}

	if($_POST['ktyp'] == 2021)
	{
		$ktpos = strpos($konfig, '#kto_');
		if(!$ktpos)
			$ktpos = strpos($konfig, '#ktu_');
		if(!$ktpos)
		{
			$add  = "// Kommunikationsvariablen".$nl;
			$add .= "// -----------------------".$nl;
			$add .= "#kto_".str_replace(' ', '_', strtolower(trim(meldung(2113)))).$nl;	// rechnungsanschrift

			$sql = "SELECT tabwert FROM wertetab WHERE mandanten_id=".$_POST['mordner']." AND id_feld=94 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ".db_orderby('tabwert');
			$ktv = db_values($sql, '*');
			if(is_array($ktv))
			{
				foreach($ktv as $val)
					$add .= "// #kto_".str_replace(' ', '_', strtolower(trim($val['tabwert']))).$nl;
				$add .= $nl;
				foreach($ktv as $val)
					$add .= "// #ktu_".str_replace(' ', '_', strtolower(trim($val['tabwert']))).$nl;
			}
			else
			{
				$add .= "// #kto_kommunikationsvariablenname_oben".$nl.$nl;
				$add .= "// #ktu_kommunikationsvariablenname_unten".$nl;
			}

			$kopfpos = strpos($konfig, '// Kopfabschnitt');
			if($kopfpos !== false)
				$konfig = substr($konfig, 0, $kopfpos).$add.$nl.substr($konfig, $kopfpos);
			else
				$konfig .= $nl.$add;
		}

		$ippos = strpos($konfig, '#_intprocess');
		if(!$ippos)
		{
			$add = '//#_intprocess		// falls aktiviert, dann wird die Spalte "Vorgang" in die Spalte "Bezeichnung" integriert';
			$varpos = strpos($konfig, $nl.'// Variablen');
			if($varpos !== false)
				$konfig = substr($konfig, 0, $varpos).$add.$nl.substr($konfig, $varpos);
			else
				$konfig .= $nl.$add;
		}

		$nppos = strpos($konfig, '#_npprocnr');
		if(!$nppos)
		{
			$add = '//#_noprocnr		// falls gesetzt, dann wird in die Spalte "Vorgang" keine Projektnummer geschrieben, sondern nur der Name';
			$varpos = strpos($konfig, $nl.'// Variablen');
			if($varpos !== false)
				$konfig = substr($konfig, 0, $varpos).$add.$nl.substr($konfig, $varpos);
			else
				$konfig .= $nl.$add;
		}

		$nvpos = strpos($konfig, '#_novat');
		if(!$nvpos)
		{
			$add = '//#_novat		// falls aktiviert, dann werden die Spalten "MwSt.", "GP MwSt." und "GP Brutto" nicht angezeigt';
			$varpos = strpos($konfig, $nl.'// Variablen');
			if($varpos !== false)
				$konfig = substr($konfig, 0, $varpos).$add.$nl.substr($konfig, $varpos);
			else
				$konfig .= $nl.$add;
		}
	}
}
$Text_varLang_style = 'font-family:Courier,sans-serif;font-style:normal;font-weight:normal;color:#006b9f;';
$maskenfeldwerte['text_varlang'] = $konfig;
?>