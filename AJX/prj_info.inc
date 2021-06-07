<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_info.inc                                                 //
// Version     : 21.1                                                         //
// Begin       : 2021-03-30                                                   //
// Last Change : 2021-03-30                                                   //
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
 * AJAX: Project popup info
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

$tr=$GLOBALS['fxpglobals']['tr'];

$pid=$_GET['pid'];
if(!$pid)
	$pid=0;

// Prüfen, ob Recht auf Projekt
$pid_ok=prights_check($pid, 2);

$title=meldung(2585);	// Projektübersicht
$pmwidth=640;
$pmheight=128;

if($pid_ok)
{
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h2 class="darkgrey">'.meldung(712,true).'</h2></td></tr></table>';	// Es wurden keine passenden Einträge gefunden.

	// Existiert das Projekt
	$pexists=db_value("SELECT name_projekt FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(strlen(trim($pexists)))
	{
		$_GET['pid']=$pid;

		$req_fn=fxf_fn_reqFilename('projektuebersicht_ini', 209);
		if(strlen($req_fn))
		{
			$pmwidth=0;
			$pmheight=0;

			ob_start();
			require($req_fn);
			$inhalt=ob_get_contents();
			ob_end_clean();
		}
	}
}
else
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h1 class="red">'.meldung(1556,true).'</h1></td></tr></table>';	// Sie haben nicht die erforderlichen Rechte um Informationen über dieses Projekt zu erhalten!

echo(popupMask($title, '', $inhalt, '', $pmwidth, $pmheight));
?>