<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benzugr_db.inc                                               //
// Version     : 21.2                                                         //
// Begin       : 2020-10-13                                                   //
// Last Change : 2020-10-13                                                   //
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
 * PF 97: User Profile Assignment - Database Actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($_POST['Button_Multi'])
{
	reset($_POST['ID_BGZ']);
	$mc=key($_POST['ID_BGZ']);

	$ins=0;
	$del=0;

	if(fxIsArray($_POST['ID_BGZ'][$mc]) && $Benutzergr_ID)
	{
		// Alle Einträge in einer Schleife durchgehen
		foreach($_POST['ID_BGZ'][$mc] as $i => $idbgz)
		{
			if($err)
				break;

			if($Modi == 1)	// Benutzergruppenmodus
			{
				$bengr_id=$Benutzergr_ID;
				$ben_id=$_POST['Benutzer_ID'][$mc][$i];
			}
			else			// Benutzmodus
			{
				$bengr_id=$_POST['Benutzer_ID'][$mc][$i];
				$ben_id=$Benutzergr_ID;
			}

			// Hinzufügen
			if($_POST['Checkbox'][$mc][$i] && !$idbgz)
			{
				$NewID=id_max('benutzergruppenzo', 'id_bgz');

				$sql  = "INSERT INTO benutzergruppenzo (mandanten_id, id_bgz, benutzer_id, benutzergr_id)";
				$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$NewID.", ".$ben_id.", ".$bengr_id.")";
				$err=db_x($sql);
fxDebug($err,$sql, 2);
				if(!$err)
					$ins++;
			}

			// Rausnehmen
			else if(!$_POST['Checkbox'][$mc][$i] && $idbgz)
			{
				$sql="DELETE FROM benutzergruppenzo WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND benutzer_id=".$ben_id." AND benutzergr_id=".$bengr_id;
				$err=db_x($sql);
fxDebug($err,$sql, 2);
				if(!$err)
					$del++;
			}
		}
	}

	if(!$err)
	{
		if($ins || $del)
		{
			if($ins > 1)
				error_msg(114, false, $ins);	// 114: Es wurden $var1 Datensätze erfolgreich angelegt.
			else if($ins)
				error_msg(117);					// 117: Es wurde ein Datensatz erfolgreich angelegt.
			if($del > 1)
				error_msg(116, false, $del);	// 116: Es wurden $var1 Datensätze erfolgreich gelöscht.
			else if($del)
				error_msg(119);					// 119: Es wurde ein Datensatz erfolgreich gelöscht.
		}
		else
			error_msg(160);						// 160: Es wurden keine Änderungen durchgeführt.
	}
//$err=true;
}
?>