<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prteam_auswahl_ini.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-11-16                                                   //
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
 * PF 230: Team Builder
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$show_liste=false;
if($_POST['Button_Multi'])
	$show_liste=true;
else if($_POST['Button_Submit'])
{
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		$show_liste=true;
	else if($_POST['projektteam_id'])
		$show_liste=true;
	else
	{
		error_msg(789, -1, 'projektteam_id');	// 789: Im Feld $var1 muss ein Eintrag ausgewählt werden!
		$err=true;
	}
}

// Validierung
if($_POST['Button_Multi'] && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
{
	if(isset($_POST['Personen_ID']) && is_array($_POST['Personen_ID']))
	{
		reset($_POST['Personen_ID']);
		$mc=key($_POST['Personen_ID']);
	}
	else
		$mc=2;
	// Leiter
	if(!$err && is_array($_POST['leiter'][$mc]))
	{
		if(sizeof($_POST['leiter'][$mc]) > 1)
		{
			error_msg(278, false);	// 278: Pro Projekt-Team darf es nur einen Teamleiter geben!
			$err=true;
		}
		else
		{
			foreach($_POST['leiter'][$mc] as $z => $dmy)
				$_POST['Checkbox'][$mc][$z]=1;
		}
	}
	// Checkbox
	if(!$err && !is_array($_POST['Checkbox'][$mc]))
	{
		error_msg(1642, false);	// 1642: Es wurden keine Einträge angehakt.
		$err=true;
	}
	// Teamname
	if(!strlen((string)trim((string)$_POST['pteam_name'])))
	{
		error_msg(279, false);	// 279: Für das Projekt-Team wurde <B>kein</B> Name vergeben!
		$err=true;
	}
	else
		$_POST['pteam_name']=trim((string)$_POST['pteam_name']);
}
?>