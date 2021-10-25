<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pers_info.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2021-01-15                                                   //
// Last Change : 2021-01-15                                                   //
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
 * PF 248: Communication Report
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

$GLOBALS['_maskcounter']=1;
$tr=$GLOBALS['fxpglobals']['tr'];

$_pid=$_GET['pid'];
if(!$_pid)
	$_pid=0;

// Prüfen, ob Recht auf Person
$pid_ok=false;
// Verwaltung oder Geschäfstführer oder eigene Person
if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['person'] == $_pid))
	$pid_ok=true;
else
{
	$pers=get_persons();
	$eps=explode(',',$pers);
	$pid_ok=in_array($_pid,$eps);
}

$title=meldung(228, true);	// Übersicht

if($pid_ok)
{
	$inhalt=popupMessage(meldung(712,true));	// Es wurden keine passenden Einträge gefunden.

	// Existiert die Person
	$_cid=(int)db_value("SELECT mandanten_id FROM personen WHERE personen_id=".$_pid." AND mandanten_id IS NOT NULL");
	if($_pid && $_cid)
	{
		// Werte ermitteln
		$Personen_ID=$pid;
		$_POST=array('Person_waehlen'=>true, 'Person_alias'=>$_cid.'_'.$_pid);

		$req_fn=fxf_fn_reqFilename('personen_kompakt_info_pre', 166);
		if(strlen($req_fn))
		{
			require($req_fn);
			$inhalt=maske(387,'',true);
		}
	}
}
else
	$inhalt=popupMessage(meldung(1563,true), 'red');	// Sie haben nicht die erforderlichen Rechte um Informationen über diese Person zu erhalten!

echo($inhalt);
?>