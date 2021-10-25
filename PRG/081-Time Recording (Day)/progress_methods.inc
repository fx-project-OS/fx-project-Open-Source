<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : progress_methods.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2020-11-09                                                   //
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
 * PF 81: Time Recording (Day) - AJAX
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
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

$m=0;
$s=0;
if(fxIsArray($_GET))
{
	if(isset($_GET['m']))
		$m=(int)$_GET['m'];
	if(isset($_GET['s']))
		$s=(int)$_GET['s'];
}
if(!$s)
	$s=$GLOBALS['fxpglobals']['lang'];

// Methoden
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1659 AND id_sprache=".$s." AND mandanten_id=0";
$methods=get_array($sql, 'wertetabid', 'tabwert');

// Methodenhilfe
$hdl='';
$mh0='';
$msg='';
$sql="SELECT suchtext, meldungstext FROM meldungen WHERE suchtext LIKE 'Methode%' AND id_sprache=".$s." AND mandanten_id=0";
$methodshelp=get_array($sql, 'suchtext', 'meldungstext');
if(fxIsArray($methodshelp))
{
	foreach($methodshelp as $mn => $mh)
	{
		$mt=(int)substr($mn, 7);
		if(!$m || ($m == $mt))
		{
			if($methods[$mt])
			{
				// Headline
				$hd='<div class="s3b" style="padding:4px;background:#e8e8e8;border-bottom:1px solid black;">'.$methods[$mt].'</div>';
				if(!strlen($mh0))
					$mh0=$hd;
				else
					$msg .= $hd;

				if(!strlen($hdl))
					$hdl=$methods[$mt];
				else
					$hdl .= ' +';
			}
			$msg .= $mh.'<br /><br />';
		}
	}
}
if(substr($hdl,-1) == '+')
	$msg=$mh0.$msg;

$print_icon=true;
if(!strlen($msg))
{
	$print_icon=false;
	$msg='<table width=100% height=100% cellspacing=32><tr><td width=38% align=right><b style="font-size:128px;color:#c50016;text-shadow:2px 2px rgba(0,0,0, 0.5);">!</b></td><td width=62% align=left><b class="s5" style="color:#c50016;">'.meldung(966,false).'</b></td></tr></table>'; // Help text not available!
}

// Popup mask: Setting help text
$html=popupMask($hdl, '', $msg, 'dtxpm', 880, 640, true, $print_icon);
echo($html);
?>