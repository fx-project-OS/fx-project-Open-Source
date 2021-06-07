<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faezuordnung_db.inc                                          //
// Version     : 21.1                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * PF 22: Employee's Roles
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!$err && $pid && $_POST['Button_Multi'])
{
fxDebug($aa, '$aa', 3);
	if(fxIsArray($aa))
	{
		// Inserts
		$ins=0;
		if(fxIsArray($aa['ins']))
		{
			foreach($aa['ins'] as $l => $a)
			{
				$ni=id_max('Ma_Art_Faehigkeiten', 'Ma_Art_Faehigkeit_ID');

				$sql  = "INSERT INTO ma_art_faehigkeiten";
				$sql .= " (mandanten_id, ma_art_faehigkeit_id, personen_id, maart_id, mafaehigkeit_id, dauer, kommentar, bewertung, aenderungs_zeit) ";
				$sql .= "VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$ni.", ".$pid.", ".$a['gi'].", ".$a['si'].", ".$a['experience'].", ".$a['comment'].", ".$a['rating'].", '".$GLOBALS['datetime']."')";

				$e=db_x($sql);
fxDebug($e,$sql, 2);
				if($e)
				{
					$err=true;
					$ea[$l]=true;
					error_msg(1521, false, '<u>'.$a['gn'].'</u> &gt; <b>'.$a['sn'].'</b>'); // 1521: Fehler beim Anlegen des Datensatzes $var1!
				}
				else
					$ins++;
			}
		}
		// Updates
		$upd=0;
		if(fxIsArray($aa['upd']))
		{
			foreach($aa['upd'] as $l => $a)
			{
				$sql  = "UPDATE ma_art_faehigkeiten";
				$sql .= " SET dauer=".$a['experience'].", kommentar=".$a['comment'].", bewertung=".$a['rating'].", aenderungs_zeit='".$GLOBALS['datetime']."'";
				$sql .= " WHERE ma_art_faehigkeit_id=".$a['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

				$e=db_x($sql);
fxDebug($e,$sql, 2);
				if($e)
				{
					$err=true;
					$ea[$l]=true;
					error_msg(1522, false, '<u>'.$a['gn'].'</u> &gt; <b>'.$a['sn'].'</b>'); // 1522: Fehler beim Ändern des Datensatzes $var1!
				}
				else
					$upd++;
			}
		}
		// Delete
		$del=0;
		if(fxIsArray($aa['del']))
		{
			foreach($aa['del'] as $l => $a)
			{
				$sql  = "DELETE FROM ma_art_faehigkeiten";
				$sql .= " WHERE ma_art_faehigkeit_id=".$a['id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

				$e=db_x($sql);
fxDebug($e,$sql, 2);
				if($e)
				{
					$err=true;
					$ea[$l]=true;
					error_msg(1523, false, '<u>'.$a['gn'].'</u> &gt; <b>'.$a['sn'].'</b>'); // 1523: Fehler beim Löschen des Datensatzes $var1!
				}
				else
					$del++;
			}
		}

		// Success
//$err=true;
		if(!$err)
		{
			$ea=array();

			if($ins == 1)
				error_msg(117,false);		// 117: Es wurde ein Datensatz erfolgreich angelegt.
			else if($ins > 1)
				error_msg(114,false,$ins);	// 114: Es wurden $var1 Datensätze erfolgreich angelegt.

			if($upd == 1)
				error_msg(118,false);		// 118: Es wurde ein Datensatz erfolgreich geändert.
			else if($upd > 1)
				error_msg(115,false,$upd);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.

			if($del == 1)
				error_msg(119,false);		// 119: Es wurde ein Datensatz erfolgreich gelöscht.
			else if($del > 1)
				error_msg(116,false,$del);	// 116: Es wurden $var1 Datensätze erfolgreich gelöscht.

			setPRole();
			$persids=get_persons();
		}
	}
	else
		error_msg(160);		// 160: Es wurden keine Änderungen durchgeführt.
}
?>