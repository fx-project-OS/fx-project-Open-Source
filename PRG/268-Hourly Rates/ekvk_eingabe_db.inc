<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ekvk_eingabe_db.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2020-11-17                                                   //
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
 * 268: Hourly Rates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err && $db_update && fxIsArray($_POST) && isset($_POST['Button_Speichern']))
{
	// Neuanlage
	$ins=0;
	if(fxIsArray($new_array))
	{
//fxDebug($new_array, '$new_array');

		foreach($new_array as $data)
		{
			$cols="mandanten_id";
			$vals=$GLOBALS['fxpglobals']['client'];
			foreach($data as $col => $val)
			{
				if($col != 'line')
				{
					$cols .= ",".$col;
					$vals .= ",".$val;
				}
			}
			$sql="INSERT INTO ekvk (".$cols.") VALUES (".$vals.")";
			$ser=db_x($sql);
fxDebug($ser,$sql, 2);
			if($ser)
			{
				// 1521: Fehler beim Anlegen des Datensatzes '$var1'!
				error_msg(1521, false, '<b>'.$_POST['nv_text2'][$mc][$data['line']].'</b>');
				$err=true;
			}
			else
				$ins++;
		}
	}

	// Ändern
	$upd=0;
	if(fxIsArray($upd_array))
	{
//fxDebug($upd_array, '$upd_array');

		foreach($upd_array as $data)
		{
			$sql  = "UPDATE ekvk";
			$sql .= " SET ek=".$data['ek'].",vk=".$data['vk'];
			$sql .= " WHERE id=".$data['id']." AND feldtyp=".$data['feldtyp'];
			$sql .= " AND maart_id=".$data['maart_id']." AND mafaehigkeit_id=".$data['mafaehigkeit_id'];
			$sql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ser=db_x($sql);
fxDebug($ser,$sql, 2);
			if($ser)
			{
				// 1522: Fehler beim Ändern des Datensatzes '$var1'!
				error_msg(1522, false, '<b>'.$_POST['nv_text2'][$mc][$data['line']].'</b>');
				$err=true;
			}
			else
				$upd++;
		}
	}

	// Löschen
	$del=0;
	if(fxIsArray($del_array))
	{
//fxDebug($del_array, '$del_array');

		foreach($del_array as $data)
		{
			$sql  = "DELETE FROM ekvk";
			$sql .= " WHERE id=".$data['id']." AND feldtyp=".$data['feldtyp'];
			$sql .= " AND maart_id=".$data['maart_id']." AND mafaehigkeit_id=".$data['mafaehigkeit_id'];
			$sql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$ser=db_x($sql);
fxDebug($ser,$sql, 2);
			if($ser)
			{
				// 1523: Fehler beim Löschen des Datensatzes '$var1'!
				error_msg(1523, false, '<b>'.$_POST['nv_text2'][$mc][$data['line']].'</b>');
				$err=true;
			}
			else
				$del++;
		}
	}

	// Erfolgsmeldungen
	if(!$err)
	{
		if($ins == 1)
			error_msg(117, false);			// Es wurde ein Datensatz erfolgreich angelegt.
		else if($ins)
			error_msg(114, false, $ins);	// Es wurden $var1 Datensätze erfolgreich angelegt.

		if($upd == 1)
			error_msg(118, false);			// Es wurde ein Datensatz erfolgreich geändert.
		else if($upd)
			error_msg(115, false, $upd);	// Es wurden $var1 Datensätze erfolgreich geändert.

		if($del == 1)
			error_msg(119, false);			// Es wurde ein Datensatz erfolgreich gelöscht.
		else if($del)
			error_msg(116, false, $del);	// Es wurden $var1 Datensätze erfolgreich gelöscht.
	}
//$err=true;
}
?>