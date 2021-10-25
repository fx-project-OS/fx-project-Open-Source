<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_planung_ini.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * PF 28: Ressourceplanner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$cpo28=true;

// 23 [Vorgänge->Vorgänge (Einzelansicht)]
if($GLOBALS['fxpglobals']['tr'] == 23)
{
	$Name_Sachmittel_nv=true;
	$Sachmitteltyp_nv=true;
	$Soll_Beg_Zeit_nv=true;
	$Soll_End_Zeit_nv=true;

	$Plan_Beg_Dtm_sf=2;
	$Plan_End_Dtm_sf=2;
}

// 28 [Controlling->Ressourcen->Mitarbeiterzuordnung]
else if($GLOBALS['fxpglobals']['tr'] == 28)
{
	if($_POST['Zeitspanne_von'])
	{
		$zp_v=validate_entry($_POST['Zeitspanne_von'], $err_zp_v, 'datum');
		if($err_zp_v)
		{
			$err=true;
			error_msg($err_zp_v, false, 'Zeitspanne_von');
		}
	}
	else
		$zp_v='00000000000000';
	if($_POST['Zeitspanne_bis'])
	{
		$zp_b=validate_entry($_POST['Zeitspanne_bis'], $err_zp_b, 'datum');
		if($err_zp_b)
		{
			$err=true;
			error_msg($err_zp_b, false, 'Zeitspanne_bis');
		}
	}
	else
		$zp_b='99999999999999';

	// Startdatum nach Enddatum => Error
	if($zp_v > $zp_b)
	{
		$err=true;
		error_msg(651, false, 'Zeitspanne_von', 'Zeitspanne_bis');
	}
}

// 101 [Controlling->Ressourcen->Sachmittelzuordnung]
else if($GLOBALS['fxpglobals']['tr'] == 101)
{
	$MaArt_ID_nv=true;
	$MaFaehigkeit_ID_nv=true;
	$Pname_nv=true;
	$Vorname_nv=true;
}

if($Button_Weiter_x || $Button_Zurueck_x)
	$Button_sort=true;
// Nach dem Klick auf "sortiere Daten" soll nicht gleich eine DB-Zugriff ausgeführt werden
if($Button_sort)
	$db_update=false;

// Sortieranhängsel für ListenSQL
if($Sachmitteltyp)
	$ORDER=" AND sachmitteltyp=".$Sachmitteltyp;
else
	$ORDER='';

if($Projekt_ID)
	$Projektauswahl=" AND mp.projekt_id IN (".inferior_vn($Projekt_ID).")";
else
	$Projektauswahl='';
?>