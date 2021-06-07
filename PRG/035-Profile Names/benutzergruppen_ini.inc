<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzergruppen_ini.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2020-10-12                                                   //
// Last Change : 2020-10-12                                                   //
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
 * PF 35: Profile Names - Initialization
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!fxIsArray($_GET) && fxIsArray($_POST) && isset($_POST['Button_Multi']) && fxIsArray($_POST['Benutzergr_ID']) && ($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY))
{
//fxDebug('_POST');

	// Submit key
	reset($_POST['Benutzergr_ID']);
	$akey=key($_POST['Benutzergr_ID']);

	$found=0;

	foreach($_POST['Benutzergr_ID'][$akey] as $l => $gid)
	{
		if(!strlen($gid))
			continue;

		$grpname=trim($_POST['Gruppenname'][$akey][$l]);
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && !strlen($grpname))
			continue;
		if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && !$_POST['Checkbox'][$akey][$l])
			continue;

		$found++;

		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			continue;

		$val_zeilennummer=$l;
		if(!strlen($grpname))
		{
			// Das Feld $var1 darf nicht leer sein. Bitte geben Sie einen Text ein!
			error_msg(749, false, 'Gruppenname');
			$err=true;
		}
	}

	if(!$err && !$found)
	{
		if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		{
			// Es wurden keine Daten für die Neuanlage eingegeben.
			error_msg(168, false);
			$err=true;
		}
		else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		{
			// Es wurden keine Einträge angehakt.
			error_msg(1642, false);
			$err=true;
		}
	}
}
else
	$dbaktion=false;
?>