<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_check_db.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2020-12-14                                                   //
// Last Change : 2020-12-14                                                   //
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
 * 37: Project Checker
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(($mpid >= 0) && $_POST['Button_Anpassen'] && (fxIsArray($GLOBALS['fxptdata']['backup']['pc']) || fxIsArray($GLOBALS['fxptdata']['backup']['bc']) || fxIsArray($GLOBALS['fxptdata']['backup']['zc']) || fxIsArray($GLOBALS['fxptdata']['backup']['tc'])))
{
	$err=false;

	foreach($GLOBALS['fxptdata']['backup'] as $typ => $arr)
	{
		// Keine Änderungen notwendig, dann NEXT
		if(!fxIsArray($arr))
			continue;

		foreach($arr as $pid => $fa)
		{
			$chk="";
			$sql="";
			$where=" WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			switch($typ)
			{
				case 'pc';	// Änderungen an Projekten durchführen
					$chk="SELECT projekt_id FROM projekte".$where;
					$sql="UPDATE projekte SET ";
				break;

				case 'bc';	// Änderungen an Budgets durchführen
					$chk="SELECT projekt_id FROM budget_summe".$where;
					$sql="UPDATE budget_summe SET ";
				break;

				case 'zc';	// Änderungen an Zeiten durchführen
					$where .= " AND zeitart=".FXP_TE_PROJECT;
					$chk="SELECT zeit_id FROM zeitdaten".$where;
					$sql="UPDATE zeitdaten SET ";
				break;

				case 'tc';	// Neue Aufgabe erstellen und TP-Zeiterfassungen darauf buchen
					$chk="SELECT projekt_id FROM projekte".$where;
				break;
			}

			// Nicht unterstützter Typ, dann NEXT
			if(!strlen($chk))
				continue;

			// Datensatz nicht mehr vorhanden, dann NEXT
			$exists=(int)db_value($chk);
			if(!$exists)
				continue;

			// Historie schreiben?
			$make_history=false;
			if($_POST['historie'])
				$make_history=true;

			// Update-SQL zusammenbauen
			if(strlen($sql))
			{
				$fc=0;
				foreach($fa as $field => $value)
				{
					if($fc)
						$sql .= ",";
					$sql .= $field."=".$value;

					$fc++;
				}
				$sql .= $where;
fxDebug($sql, $typ.': '.$pid, 2);

				// SQL ausführen
				$err=db_x($sql, 0, false, true, $make_history);
				if($err)
					break;
			}
		}
	}

//$err=true;
	if($err)
		error_msg(2132, false);	// Beim Datenbankzugriff trat ein Fehler auf! Die bereits durchgeführten Änderungen wurden wieder rückgängig gemacht.
	else
		error_msg(1519, false);	// Alle Datensätze wurden erfolgreich geändert.
}
?>