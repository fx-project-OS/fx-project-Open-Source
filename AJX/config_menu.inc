<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_menu.inc                                              //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-07-22                                                   //
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
 * Change menu structure to alternative version or vice versa
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
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

// Menu structure
$ma=loadMenu();
//fxDebug($ma, '$ma');
if(sizeof($ma))
{
	fxSession('g','p','menudate',$GLOBALS['datetime']);

	echo('<div id="mstructure" style="display:none;">');
	$mfs=false;
	$ms=0;
	foreach($ma as $mid => $marr)
	{
		if(!$marr['lv'] && ($marr['tx'] != 'Sysadmin'))
			$ms=max($ms, (int)substr($mid,1,2));

		$mid=substr($mid,1);
		if($marr['lv'] < 3)	// max. 3 Level
		{
			if($mfs)
				echo('~');
			echo($mid.'|'.$marr['lv'].'|'.$marr['tr'].'|'.$marr['tx'].'|'.$marr['ar'].'|'.$marr['tt']);
			$mfs=true;
		}
	}
	echo('</div>'.$nl);

	// Menu main entries
	echo('<div id="mentries" style="display:none;">'.$ms.'</div>'.$nl);

	// Menu colors
	$mct=getMenuColorsCSS($ms);
//echo('<b>$mct</b><pre>'.fxHtmlEncode($mct).'</pre>');
	echo('<div id="mcolors" style="display:none;">'.$mct.'</div>'.$nl);

	// Breadcrumb path
	$cam_tpath='';
	$cam_tkf='';
	$cam_arg='';
	if(isset($_GET['repid']))
		$cam_arg='repid='.$_GET['repid'];
	foreach($ma as $tk => $tc)
	{
		if(($tc['tr'] == $GLOBALS['fxpglobals']['tr']) && ($tc['ar'] == $cam_arg))
		{
			$cam_tkf=$tk;
			break;
		}
	}
	if(strlen($cam_tkf))
	{
		$cam_fxheader=$ma[$cam_tkf]['tx'];
		if(strlen($cam_arg))
			$cam_arg=' arg="'.$arg.'"';
		$cam_tpath='<div id="fxm_'.substr($cam_tkf,1).'_p" class="ttitle" tr="'.$GLOBALS['fxpglobals']['tr'].'"'.$cam_arg.' style="float:left;font-weight:800;cursor:pointer;">'.$GLOBALS['fxpglobals']['tr'].': '.$cam_fxheader.'</div>';
		while(true)
		{
			$cam_tkf=substr($cam_tkf,0,-3);
			if((strlen($cam_tkf) < 3) || !isset($ma[$cam_tkf]))
				break;
			else
				$cam_tpath='<div id="fxm_'.substr($cam_tkf,1).'_p"  class="ttitle" style="float:left;text-decoration:underline;cursor:pointer;">'.$ma[$cam_tkf]['tx'].'</div><div class="ttitle" style="float:left;margin:0 4px;">&gt;</div>'.$cam_tpath;
		}
	}
	echo('<div id="tpath" style="display:none;">'.$cam_tpath.'</div>'.$nl);
}
?>