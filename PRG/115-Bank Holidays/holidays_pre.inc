<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : holidays.inc                                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-09-24                                                   //
// Last Change : 2024-04-29                                                   //
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
 * PF 115: Bank Holidays - File for presetting the mask fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Mask prepearation variables
$index=true;
$mc=$GLOBALS['_maskcounter'];
$l=0;
$GLOBALS['fxptdata']['lvalues'][$mc]=array();

// Mask data field padding
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$maske343_dpd='6px 4px';
else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
	$maske343_dpd='4px';

// Hide button?
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;

// Holiday types and display colors
$hta=fxf_getReferenceValues(591);
$htf=get_text(1273,'text');
//fxDebug($hta,'$hta: $htf='.$htf, 0);
$fca=array(FXP_HL_FULL=>"black", FXP_HL_HALF=>"grey", FXP_HL_DISPLAY=>"lightergrey");

// Weekdays
$wda=fxf_getWeekdayNames(2);
//fxDebug($wda,'$wda', 0);

// Checkbox display headline texts in popup according to action mode
$GLOBALS['fxptdata']['litf_343']['Checkbox']=get_text(380,'5').' &rarr; '.meldung(54+$GLOBALS['fxpglobals']['action']);
$Checkbox_sf=true;
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	$Checkbox_nv=true;
	// Compensate holiday input width with hidden checkbox width
	$Name_Feiertag_width=48+abs(db_value("SELECT anzeigegroesse1 FROM maskenfelder WHERE nummer=343 AND eingabefeld=1274 AND zeile>1"));
}

// Day padding
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Tage_dpd='6px';
else
	$Tage_dpd='4px';

// Mask headline
$mha=explode('/', get_text(343,'kurz',0,2));
// ...Holidays for the Year + Input field: year
$maske343_head='<div class="maskht" style="display:inline-block;">'.trim((string)$mha[0]).'&nbsp;';
$y=(int)$year;
if($y <= 0)
	$y=(int)substr((string)$GLOBALS['date'],0,4);
if($y <= $minyear)
{
	$oca='';
	$ysa='opacity:0.33;';
}
else
{
	$oca=' onclick="'.fxf_jsFunction('waitScreen', '!name').'(\'reloading\'); '.fxf_jsFunction('loadTR', '!name').'('.$GLOBALS['fxpglobals']['tr'].', \'pyear\');"';
	$ysa='cursor:pointer;';
}
$maske343_head .= '<div class="maskht bold bshadowhd"'.$oca.' style="display:inline-block;width:16px;text-align:center;background:#cecece;border:1px solid #d2d2d2;'.$ysa.'">-</div>';
$maske343_head .= fieldInteger('year', $y, 50, true, false, false, '', '', '', false, '', 'maskht');
if($y >= $maxyear)
{
	$oca='';
	$ysa='opacity:0.33;';
}
else
{
	$oca=' onclick="'.fxf_jsFunction('waitScreen', '!name').'(\'reloading\'); '.fxf_jsFunction('loadTR', '!name').'('.$GLOBALS['fxpglobals']['tr'].', \'nyear\');"';
	$ysa='cursor:pointer;';
}
$maske343_head .= '<div class="maskht bold bshadowhd"'.$oca.' style="display:inline-block;width:16px;text-align:center;background:#cecece;border:1px solid #d2d2d2;'.$ysa.'">+</div>';
if(sizeof($hda))
{
	// ...and for the Country + Select field: country
	$maske343_head .= '<div class="maskht" style="display:inline-block;">&nbsp;'.trim((string)$mha[1]).'&nbsp;</div>';
	$GLOBALS['countryselect']=array();
	foreach($hda as $cn => $ca)
	{
		$fi='';
		if(strlen((string)$ca['flag']))
			$fi='<img src='.$ca['flag'].' align=top width=20 height=13 style=\'margin-top:3px;\' />&nbsp;';
		$GLOBALS['countryselect'][$cn]=$fi.$cn;
	}
	$GLOBALS['country_classadd']='maskht';
	$GLOBALS['country_styleadd']='display:inline-flex;';
	$maske343_head .= fieldSelect('country', $country, 256, 0, false, false, false, '', '', false, '');
}
$maske343_head .= '</div>';

// No right border in the following columns
fxf_maskColumnBorder(array('Checkbox', 'Tage'), 0, -1, $mc);

// Do not add an empty default select value into the select field "Half Day"
$GLOBALS['halbtaegig_ds']=0;

// Display list
$da=array();
// Insert all holidays for the selected country
if(sizeof($hda) && strlen((string)$country) && fxIsArray($hda[$country]) && fxIsArray($hda[$country]['days']))
{
	foreach($hda[$country]['days'] as $hn => $hd)
	{
		$hdt=fxf_calcDate($hd, $y);
		if(substr((string)$hn,0,1) == '-')
		{
			$name=substr((string)$hn,1);
			$type=FXP_HT_PUBLIC;
		}
		else if(substr((string)$hn,0,1) == '+')
		{
			$name=substr((string)$hn,1);
			$type=FXP_HT_RELIGIOUS;
		}
		else
		{
			$name=$hn;
			$type=FXP_HT_OTHER;
		}
		$da[$hdt]=array('hid'=>0, 'nid'=>0, 'calc'=>$hd, 'type'=>$type, 'check'=>0, 'name'=>$name, 'length'=>FXP_HL_DISPLAY);
	}
}

// Load and insert/update already stored holidays for the current and the past year
$sql  = "SELECT m.feiertag_id, m.datum, m.feiertag_name, m.halbtaegig, n.id_sprache, n.f_tag, n.name_feiertag, n.feiertag_art";
$sql .= " FROM feiertage_mand m, feiertage_name n";
$sql .= " WHERE (m.datum LIKE '".$y."%' OR m.datum LIKE '".($y-1)."%') AND m.mandanten_id=n.mandanten_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$sql .= " AND m.feiertag_name=n.feiertag_name AND n.id_sprache=".$GLOBALS['fxpglobals']['lang'];
$sql .= " ORDER BY m.datum";
$hca=db_values($sql, '*');
fxDebug($hca,'$hca: $sql='.$sql, 3);
if(fxIsArray($hca))
{
	foreach($hca as $ha)
	{
		$hdt=$y.substr((string)$ha['datum'],4,4);
		if(strlen((string)$ha['f_tag']))
			$hdt=fxf_calcDate($ha['f_tag'], $y);
		$hid=0;
		if((int)substr((string)$ha['datum'],0,4) == $y)
			$hid=(int)$ha['feiertag_id'];

		if(!isset($da[$hdt]) || $hid)
			$da[$hdt]=array('hid'=>$hid, 'nid'=>(int)$ha['feiertag_name']);
		$da[$hdt]['calc']=$ha['f_tag'];
		$da[$hdt]['type']=(int)$ha['feiertag_art'];
		$da[$hdt]['check']=1;
		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			$da[$hdt]['check']=0;
		$da[$hdt]['name']=$ha['name_feiertag'];
		$da[$hdt]['length']=(int)$ha['halbtaegig'];
	}
}

if(sizeof($da))
{
	ksort($da);
fxDebug($da,'$da', 2);

	$dm=0;
	foreach($da as $dt => $dta)
	{
		// Skip existing holidays in mode: creation
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && $dta['hid'])
			continue;

		// Skip non-existing holidays in mode: display, change or delete
		if(($GLOBALS['fxpglobals']['action'] != FXP_CREATE) &&!$dta['hid'])
			continue;

		// Month header
		$cm=(int)substr((string)$dt,4,2);
		if($cm != $dm)
		{
			$dm=$cm;
			// Empty line with top border?
			if($l)
			{
				fxf_maskColumnBorder(array('Checkbox|0', 'Tage', 'Datum', 'Name_Feiertag', 'halbtaegig'), 1, -1, $mc, $l, true);
				$l++;
			}
			$headline=meldung(17+$dm).' '.$y;	// January, February, ...
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_feiertag']=$headline;
			fxf_listSectHeader($mc, $l, '&nbsp;<font class="s4b">'.$headline.'</font>', 'tage', 'halbtaegig', 4, -2, 1);
			$l++;
		}

		// Class
		$cls='^class_';
		if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			$cls .= 'rv'.(($l+1)%2).' ';
		$cls .= $fca[$dta['length']].'|';

		// ...(Checkbox)
		if($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY)
		{
			$val=$dta['check'];
			if($err && sizeof($hva) && fxIsArray($hva[$l]))
				$val=$hva[$l]['check'];
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']=$val;
		}
		// ...(Day)
		$cld='^class_rv'.(($l+1)%2).' ';
		if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			$cld .= $fca[$dta['length']];
		$cld .= '|';
		$val=$dt;
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['date'];
		$wd=getWeekday($val);
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['tage']=$cld.'~&nbsp;'.$wda[$wd];
		// ...(Date)
		if((($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && ($dta['type'] != FXP_HT_COMPANY))
		{
			$tt='<b>'.$hta[$dta['type']].'</b><br /><br />'.$htf.'<br />&rarr; <span class=blue>'.$dta['calc'].'</span>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['datum']='~<div style="position:relative;width:90px;margin-right:22px;padding:4px 2px;text-align:center;">'.get_entry($val,'date').'<img class="op33" src="'.$gfxpath.'hlp_b_21x21.png" tooltip="'.$tt.'" style="position:absolute;top:2px;right:-22px;cursor:help;"></div>';
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['datum']=$cls.$val;
		// ...(Holiday = Name)
		$val=$dta['name'];
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['name'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_feiertag']=$cls.$val;
		// ...(Half Day = Length)
		$val=$dta['length'];
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['length'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['halbtaegig']=$cls.$val;
		// ...Hidden field
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']=$dta['hid'].$GLOBALS['_divstr'][0].$dta['nid'].$GLOBALS['_divstr'][0].$dta['calc'].$GLOBALS['_divstr'][0].$dta['type'].$GLOBALS['_divstr'][0].$dta['check'].$GLOBALS['_divstr'][0].$dt.$GLOBALS['_divstr'][0].$dta['name'].$GLOBALS['_divstr'][0].$dta['length'];

		$l++;
	}
}

// Empty line with top border?
if($l)
{
	fxf_maskColumnBorder(array('Checkbox|0', 'Tage', 'Datum', 'Name_Feiertag', 'halbtaegig'), 1, -1, $mc, $l, true);
	$l++;
}

// Add new empty input lines
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	$headline='+';
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~|';
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_feiertag']=$headline;
	fxf_listSectHeader($mc, $l, '&nbsp;<font class="s4b">+</font>', 'tage', 'halbtaegig', 4, -2, 1);
	$l++;

	for($j=0; $j<$GLOBALS['fxpglobals']['settings']['listenneu']; $j++)
	{
		// ...(Checkbox)
		$val=0;
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['check'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']=$val;
		// ...(Day)
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['tage']='~&nbsp;+';
		// ...(Date)
		$val='';
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['date'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['datum']=$val;
		// ...(Holiday = Name)
		$val='';
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['name'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_feiertag']=$val;
		// ...(Half Day = Length)
		$val=FXP_HL_DISPLAY;
		if($err && sizeof($hva) && fxIsArray($hva[$l]))
			$val=$hva[$l]['length'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['halbtaegig']=$val;
		// ...Hidden field
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']='0'.$GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0].''.$GLOBALS['_divstr'][0].FXP_HT_COMPANY.$GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0].''.$GLOBALS['_divstr'][0].''.$GLOBALS['_divstr'][0].FXP_HL_DISPLAY;

		$l++;
	}

	// Empty line with top border
	fxf_maskColumnBorder(array('Checkbox|0', 'Tage', 'Datum', 'Name_Feiertag', 'halbtaegig'), 1, -1, $mc, $l, true);
	$l++;
}
?>