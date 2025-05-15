<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prs_info.inc                                                 //
// Version     : 24.1                                                         //
// Begin       : 2021-01-15                                                   //
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
 * AJAX: Person popup info
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

$GLOBALS['_maskcounter']=1;
$tr=$GLOBALS['fxpglobals']['tr'];

$pid=$_GET['pid'];
if(!$pid)
	$pid=0;

// Prüfen, ob Recht auf Person
$pgp=false;
// Verwaltung oder Geschäfstführer
if($GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])
{
	$all_pers=db_values("SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'], '*');
	$pgp=true;
}
// Projektleiter
else if($GLOBALS['fxpglobals']['persdat']['roles'][1])
{
	$all_pers=db_values("SELECT personen_id FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (elter=".$GLOBALS['fxpglobals']['person']." OR personen_id=".$GLOBALS['fxpglobals']['person'].")", '*');
}
// Mitarbeiter
else
{
	$all_pers[0]['personen_id']=$GLOBALS['fxpglobals']['person'];
}
if($GLOBALS['fxpglobals']['person'] == $pid)
	$pgp=true;

if(fxIsArray($all_pers))
{
	$pos_pers ='';
	foreach($all_pers as $p)
		$pos_pers .= ','.$p['personen_id'];
	$pos_pers=substr((string)$pos_pers, 1);
}
else
	$pos_pers='-1';
$eps=explode(',', $pos_pers);
$pid_ok=in_array($pid, $eps);

$ititle=meldung(2615,true);								// Employee - Personnel Main Data Sheet
$ctitle=meldung(2568,true);								// Contractor - Personnel Main Data Sheet
$title=trim(substr((string)$ititle, 1+strpos($ititle,'-')));	// Personnel Main Data Sheet
$pmwidth=640;
$pmheight=128;

if($pid_ok)
{
	// Es wurden keine passenden Einträge gefunden.
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h2 class="darkgrey">'.meldung(712,true).'</h2></td></tr></table>';

	// Existiert die Person
	$pexists=db_value("SELECT pname FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(!is_null($pexists) && strlen((string)trim((string)$pexists)))
	{
		// Werte ermitteln
		$Personen_ID=$pid;
		$Button_Anzeig=true;

		// Vertragsdaten
		$Anz_Vd=true;
		if($pgp)
		{
			// Bankdaten
			$Anz_Ba=true;
			// Adressen: Hauptperson
			$Anz_Adr=true;
			// Kommunikation: Hauptperson
			$Anz_Kom=true;
			// Fähigkeiten
			$Anz_Fae=true;
		}

		// Personenhierarchie
		$Anz_PersHier=true;

		$req_fn=fxf_fn_reqFilename('personalstammblatt_rep_pre', 205);
		if(!is_null($req_fn) && strlen((string)$req_fn))
		{
			$pmwidth=0;
			$pmheight=$GLOBALS['fxpglobals']['sheight']-128;

			ob_start();
			require($req_fn);
			ob_get_contents();
			ob_end_clean();

			$inhalt=maske(426);
		}
	}
}
else
{
	// Sie haben nicht die erforderlichen Rechte um Informationen über diese Person zu erhalten!
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h1 class="red">'.meldung(1563,true).'</h1></td></tr></table>';
}

echo(popupMask($title, '', $inhalt, '', $pmwidth, $pmheight));
?>