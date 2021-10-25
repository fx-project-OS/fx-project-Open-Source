<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : mta_auswahl_ini.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2021-01-13                                                   //
// Last Change : 2021-10-06                                                   //
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
 * PF 162: Milestone-Trend-Analysis (MTA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

fxDeleteTMP();

// Auch Liste einbinden wegen Grafik
$include_all_masks=true;

// Es soll trotz Anzeigemodus validiert werden
$force_valid=true;

// Speichen der Eingabewerte
if(!$_POST['dateigroesse'])
	$_POST['dateigroesse']=670;
else
	$_POST['dateigroesse']=max(400, min(1200, (int)$_POST['dateigroesse']));

if($_POST['Button_ProAnzeigen'])
{
	$GLOBALS['fxptdata']['backup']['Projekt_ID']		= $_POST['Projekt_ID'];

	$GLOBALS['fxptdata']['backup']['Zeitspanne_von']	= $_POST['Zeitspanne_von'];
	$GLOBALS['fxptdata']['backup']['Zeitspanne_bis']	= $_POST['Zeitspanne_bis'];

	$GLOBALS['fxptdata']['backup']['Unterprojekte']		= $_POST['Unterprojekte'];
	$GLOBALS['fxptdata']['backup']['Tabelle']			= $_POST['Tabelle'];

	$GLOBALS['fxptdata']['backup']['Checkpunkte']		= $_POST['Checkpunkte'];
	$GLOBALS['fxptdata']['backup']['dateigroesse']		= $_POST['dateigroesse'];
}

if(!is_array($GLOBALS['fxptdata']['backup']))
	$GLOBALS['fxptdata']['backup']['Unterprojekte']	= 1;
if(!$GLOBALS['fxptdata']['backup']['dateigroesse'])
	$GLOBALS['fxptdata']['backup']['dateigroesse']	= $_POST['dateigroesse'];
if(!$GLOBALS['fxptdata']['backup']['Projekt_ID'])
	$GLOBALS['fxptdata']['backup']['Projekt_ID']	= $GLOBALS['fxpglobals']['project'];


$pids='';
$mst=array();
if($_POST['Button_ProAnzeigen'] && $_POST['Projekt_ID'])
{
	$pid=(int)$_POST['Projekt_ID'];

	if($tr == 251)	// TTA
	{
		$ipids="SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ";
		if($_POST['Unterprojekte'])
			$ipids .= "elter=".$pid;
		else
			$ipids .= "projekt_id=".$pid;

		// Alle Vorgänge als Dummy-Meilensteine holen
		$sqlz  = " FROM projekte p, zeitdaten z";
		$sqlz .= " WHERE p.projekt_id IN (".$ipids.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];

		$sqlp="p.projekt_id";

		$sql="SELECT p.name_projekt AS name_meilenstein, p.projekt_id, p.zeitstempel, z.soll_ende".$sqlz." ORDER BY p.projekt_id";
	}
	else			// MTA
	{
		if($_POST['Unterprojekte'])
			$ipids=inferior_vn($pid);
		else
			$ipids=$pid;

		// Alle Meilensteine der untergeordneten Vorgänge holen
		$sqlz  = " FROM meilensteine m";
		$sqlz .= " WHERE (m.projekt_id=".$pid." OR m.projekt_id IN (".$ipids.")) AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];

		$sqlp="m.projekt_id";

		$sql="SELECT m.name_meilenstein, m.projekt_id, m.soll_ende, m.zeitstempel".$sqlz." ORDER BY m.projekt_id";
	}
//fxDebug($ipids, $pid);

	$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(fxIsArray($tmp))
	{
		$pids="SELECT ".$sqlp.$sqlz;
//fxDebug($pids, '$pids');

		foreach($tmp as $t)
		{
			$mpid=(int)$t['projekt_id'];
			$mst[$mpid]=array('date'=>substr($t['soll_ende'], 0, 8), 'name'=>$t['name_meilenstein'], 'timestamp'=>$t['zeitstempel'], 'epos'=>true);
		}
	}

	// Wurden Meilensteine gefunden?
	if(($tr == 162) && !strlen($pids))
	{
		if($_POST['Unterprojekte'])
			error_msg(1762, false);	// Achtung: Es wurden zu dem ausgewählten Vorgang oder einem Untervorgang keine Meilensteine definiert!
		else
			error_msg(1761, false); // Achtung: Es wurde zu dem ausgewählten Vorgang kein Meilenstein definiert!
		$Button_ProAnzeigen=false;
	}
}

if($Button_ProAnzeigen)
	$Tabelle=true;
?>