<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_b_auswahl_val.inc                                        //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(fxIsArray($_POST) && isset($_POST['Projektart']) && isset($_POST['Button_Submit']) && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && !$err_open && !$Button_ProAnzeigen)
{
	// Bestimmen der Projektart des übergeordneten Projektes
	$vEPART=0;
	if($vEPID)
		$vEPART=$pinfo[$vEPID]['data']['projektart'];
	// 789: Im Feld $var1 muss ein Eintrag ausgewählt werden!
	if(!$err && !$vPART)
	{
		error_msg(789, false, 'Projektart');
		$err=true;
	}
	// 1091: Sie dürfen keine Teilprojekte unter Aufgaben anlegen.
	if(!$err && ($vPART == FXP_PRJ_SUB) && ($vEPART > FXP_PRJ_SUB))
	{
		error_msg(1091, false);
		$err=true;
	}
	// 1096: Sie dürfen unter Aufgaben keine weiteren Vorgänge mehr anlegen.
	if(!$err && ($vPART == FXP_PRJ_TASK) && ($vEPART >= FXP_PRJ_TASK))
	{
		error_msg(1096, false);
		$err=true;
	}
	// 1090: Sie müssen die Projektart Projekt für ihren ersten Vorgang wählen. Mutterprojekte bilden die Grundlage für den Projektbaum.
	if(!$err && ($vPART != FXP_PRJ_MAIN) && !$Projekt_da)
	{
		error_msg(1090, false);
		$err=true;
	}
	// 1097: Vorgänge vom Typ Projekt können nicht unter andere Vorgänge verschoben werden.
	if(!$err && ($vPART == FXP_PRJ_MAIN) && $vEPID)
	{
		error_msg(1097, false);
		$err=true;
	}
	// 1098: Vorgänge die nicht vom Typ Projekt sind brauchen ein übergeordnetes Objekt.
	if(!$err && ($vPART != FXP_PRJ_MAIN) && !$vEPID)
	{
		error_msg(1098, false, 'Projektart');
		$err=true;
	}
	if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $_POST['Button_Submit'] && $Projekt_IDopen)
	{
		// 1206: Aufgaben dürfen nicht unterteilt werden.
		if(($vPART == FXP_PRJ_TASK) && db_value("SELECT projekt_id FROM projekte WHERE elter=".$Projekt_IDopen." AND mandanten_id=".$GLOBALS['fxpglobals']['client']))
		{
			error_msg(1206, false);
			$err=true;
		}
		// 2080: Zu diesem Vorgang besteht bereits eine Zeiterfassung es muss als Aufgabe bestehen bleiben.
		if(($vPART != FXP_PRJ_TASK) && db_value("SELECT zeit_id FROM zeitdaten WHERE projekt_id=".$Projekt_IDopen." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']))
		{
			error_msg(2080, false);
			$err=true;
		}
	}

	if(isset($db_update) && $db_update)
		$db_update=false;
}

if($err)
	$err_VALI=true;
?>