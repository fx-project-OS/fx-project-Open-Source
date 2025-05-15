<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_hlp.inc                                                //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-07-25                                                   //
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
 * Toolbar: Help
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

$GLOBALS['fxpglobals']['lang']=(int)$GLOBALS['fxpglobals']['lang'];
$lng=$GLOBALS['fxpglobals']['lang'];
$mtr=0;
$ftr='';
$ltr=$GLOBALS['fxpglobals']['tr'];
if(fxIsArray($_GET))
{
	if(isset($_GET['lang']))
		$GLOBALS['fxpglobals']['lang']=(int)$_GET['lang'];
	if(isset($_GET['mtr']))
	{
		$mtr=(int)$_GET['mtr'];
		if((string)$mtr !== (string)$_GET['mtr'])
		{
			$mtr=-1;
			$ftr=$_GET['mtr'];
		}
		$ltr=$mtr;
	}
	else if(isset($_GET['htr']))
		$ltr=(int)$_GET['htr'];
}

// Head, Keys and Description
$head='Login';
$db=checkDB();
if($db)
{
	$tex=db_check('texte','t');
	if($tex)
	{
		$sql="SELECT text_kurz FROM texte WHERE id_referenz=".$ltr." AND id_ref_art=6 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$txs=db_value($sql);
		if(strlen((string)$txs))
			$head=$txs;
		else if($ltr)
			$head='TR: '.$ltr;
	}
}

// ...Path
$filepath=$GLOBALS['docpath'].substr('00'.$GLOBALS['fxpglobals']['lang'],-2).'/';

// ...Description
$withborder=true;
$width='width:664px;';
$html='';
if(strlen((string)$ftr))
{
	$filename=$filepath.$ftr;
	if($ftr == '_workflow.htm')
		$width='';
}
else
	$filename=$filepath.substr('000000'.$ltr,-6).'.htm';
if(file_exists($filename))
{
	$html=getHelpText($filename,$withborder,$width);
}
else
{
	if(fxIsArray($_GET) && isset($_GET['tfa']))
	{
		$head='2FA';
		$filename=$filepath.'_2fa.htm';
	}
	else
		$filename=$filepath.'_intro.htm';
	if(file_exists($filename))
		$html=getHelpText($filename,$withborder,$width);
}
if(strlen((string)$html))
	$html=strtr($html, array('"../../'=>'"./'));
// ...available?
$print_icon=true;
if(!strlen((string)$html))
{
	$print_icon=false;
	$html='<table width=100% height=100% cellspacing=32><tr><td width=35% align=right><b style="font-size:128px;color:#c50016;text-shadow:2px 2px rgba(0,0,0, 0.5);">!</b></td><td width=65% align=left><b class="s5" style="color:#c50016;">'.meldung(966,false).'</b></td></tr></table>'; // Help text not available!
}
//echo('$ftr='.$ftr.', $ltr='.$ltr.', $filename='.$filename.'<hr />');

// Display
if($mtr)
	echo($html);
else
{
	// Set literals
	if($GLOBALS['fxpglobals']['lang'] == 1)
		$headline='Hilfe und Support';
	else
		$headline='Help and Support';
	if($ltr)
		$headline .= ' #'.$ltr;

	// Link to the manual
	$manual_icon=false;
	if($ltr && ($ltr != 231))
		$manual_icon=true;

	$c=popupMask($headline, $head, str_replace('class="rtable" ', '', $html), 'dhlppu', 725, 640, $print_icon, $manual_icon);
	echo($c);
}
$GLOBALS['fxpglobals']['lang']=$lng;
?>