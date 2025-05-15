<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design_liste_pre.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2021-02-01                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 40: Design
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$tta_act=explode('|',meldung(3038));	// Set design to {dsg}|Create new client design based on {dsg}|Edit client design {dsg}|Delete client design {dsg}

$fa=fxf_getMaskFields(10);
//fxDebug($fa,'$fa', 0);

$mc=$GLOBALS['_maskcounter'];
$l=0;

// No right border in the following column
fxf_maskColumnBorder(array('bild'), 0, -1, $mc);

foreach($fxdsa as $fxdsf => $fxdst)
{
	$dt=(int)substr((string)$fxdsf,0,1);
	if(!$dt)	// Our designs
	{
		$cf='CSS';
		$df=substr((string)$fxdsf,1);
		$dname='<font class="grey">'.$fxdst.'</font>';
		$ic=$GLOBALS['csspath'].$df.'.png';
	}
	else		// Client designs
	{
		$cp=strpos($fxdsf,'C',1);
		$cf=substr((string)$fxdsf,0,$cp);
		$df=substr((string)$fxdsf,$cp+1);
		$dname=$fxdst;
		$ic=$GLOBALS['datpath'].$cf.'/'.$df.'.png';
	}
	if(file_exists($ic))
		$img='<img src="'.$ic.'?'.$GLOBALS['datetime'].'" style="border:1px solid #000000;box-shadow:2px 2px 2px #888;" />';
	else
		$img='';

	// Bild (bild)
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['bild']='~'.$img;

	// Design (design)
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['design']='~'.$dname;

	// Aktion (aktion_nr)
	// ...Set design
	$sdp=$fxdsf;
	if(substr((string)$sdp,0,3) == '000')
		$sdp='';
	$a  = '<img src="'.$GLOBALS['gfxpath'].'icsh.png" tooltip="'.str_replace('{dsg}', '&quot;'.$fxdst.'&quot;', $tta_act[0]).'" style="cursor:pointer;" '.fxf_jsFunction('setDesign', $sdp).'>';
	$a .= fieldSubmitImage('dcreate', 'icne', '', str_replace('{dsg}', '&quot;'.$fxdst.'&quot;', $tta_act[1]), false, false, '', 'd='.$fxdsf.'&keep_get=1', ' style="margin-left:3px;"');
	if(!$dt)
	{
		$a .= '<img class="ainactive" src="'.$GLOBALS['gfxpath'].'icch.png" style="margin-left:3px;">';
		$a .= '<img class="ainactive" src="'.$GLOBALS['gfxpath'].'icde.png" style="margin-left:3px;">';
	}
	else
	{
		$a .= fieldSubmitImage('dedit', 'icch', '', str_replace('{dsg}', '&quot;'.$fxdst.'&quot;', $tta_act[2]), false, false, '', 'd='.$fxdsf.'&keep_get=1', ' style="margin-left:3px;"');
		$a .= fieldSubmitImage('ddelete', 'icde', '', str_replace('{dsg}', '&quot;'.$fxdst.'&quot;', $tta_act[3]), false, false, '', 'd='.$fxdsf.'&keep_get=1', ' style="margin-left:3px;"');
	}
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['aktion_nr']='~'.$a;

	$l++;
}

// Company logo
if($l)
{
	fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
	$l++;
}

$cla=explode('|',meldung(3052));	// 3052: <b>HINWEIS:</b> Das Firmenlogo muss als PNG-Grafikdatei hochgeladen werden!|Firmenlogo einbinden|Firmenlogo entfernen
// Bild (bild)
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['bild']='~<br /><b class=darkgrey>Logo</b>';
// Design (design)
$clogo_field=fieldFile('clogo_upload', 802, false, false, false, false, $cla[0], '', '');
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['design']='~<br />'.$clogo_field.'<br /><br /><br />';
// Aktion (aktion_nr)
$clogo_path=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/logo.png';
$clogo_img='0';
if(file_exists($clogo_path))
	$clogo_img='1';
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['aktion_nr']='~<br /><img src="'.$GLOBALS['gfxpath'].$clogo_img.'.png" style="padding-top:10px;">';
$l++;

fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
$l++;

// Button
fieldSubmit('int_clogo', $cla[1], 'sub', 150, false, false, true);
if(file_exists($clogo_path))
	fieldSubmit('rem_clogo', $cla[2], 'sub', 150, false, false, true);
?>