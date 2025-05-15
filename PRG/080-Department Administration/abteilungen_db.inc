<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : abteilungen_db.inc                                           //
// Version     : 24.1                                                         //
// Begin       : 2020-10-19                                                   //
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
 * PF 80: Department Administration
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$mc=$GLOBALS['_maskcounter'];
if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
	// Rekursives Löschen
	if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && fxIsArray($_POST['Checkbox'][$mc]))
	{
		foreach($_POST['Checkbox'][$mc] as $c => $v)
		{
			if($v)
			{
				$abid=$_POST['Abt_ID'][$mc][$c];
				while($abid)
				{
					$nabid='';
					foreach($_POST['Abt_ID'][$mc] as $i => $dmy)
					{
						if($_POST['Abt_ID_alias'][$mc][$i] == $abid)
						{
							$_POST['Checkbox'][$mc][$i]=1;
							if(!strlen((string)$nabid))
								$nabid=$dmy;
						}
					}
					$abid=$nabid;
				}
			}
		}
	}

	// Datenbankzugriff, wenn Abteilungsnummer angegeben
	$ins=0;
	$chg=0;
	$del=0;
	if(fxIsArray($_POST['Abt_ID']) && fxIsArray($_POST['Abt_ID'][$mc]))
	{
		foreach($_POST['Abt_ID'][$mc] as $i => $depid)
		{
			$abtid=0;

			// Neuanlage
			if(isset($_POST['Abt_Ebene'][$mc][$i]) && ($_POST['Abt_Ebene'][$mc][$i] !== NULL) && strlen((string)$_POST['Abt_Ebene'][$mc][$i]) && ($depid == -1))
			{
				$abtid=id_max('abteilung', 'abt_id', '', $GLOBALS['fxpglobals']['client']);

				$sql  = "INSERT INTO abteilung (mandanten_id, abt_id, zu_abteilung, abt_ebene, abt_name, abt_hperson)";
				$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", $abtid, ".$_POST['Abt_Unterebene'][$mc][$i].", '".convert_string($_POST['Abt_Ebene'][$mc][$i], 'todb')."', '".convert_string($_POST['Abt_Name'][$mc][$i], 'todb')."', ".$_POST['Abt_Hperson'][$mc][$i].")";
				if(!$err)
				{
					$err=db_x($sql);
fxDebug($err,$sql, 2);
				}

				$ins++;
			}
			// Ändern
			else if(isset($_POST['Abt_Ebene'][$mc][$i]) && ($_POST['Abt_Ebene'][$mc][$i] !== NULL) && strlen((string)$_POST['Abt_Ebene'][$mc][$i]) && ($depid > 0))
			{
				$abtid=$depid;

				// Bisherige Daten
				$sql="SELECT abt_ebene, abt_name, zu_abteilung, abt_hperson FROM abteilung WHERE abt_id=".$abtid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$dha=db_values($sql);
fxDebug($dha,$sql, 3);
				$dep_hlv=convert_string($dha['abt_ebene'],'todb');
				$dep_hnm=convert_string($dha['abt_name'],'todb');
				$dep_hpd=(int)$dha['zu_abteilung'];
				$dep_hdl=(int)$dha['abt_hperson'];

				// Neue Daten
				$dep_lv=convert_string($_POST['Abt_Ebene'][$mc][$i], 'todb');
				$dep_nm=convert_string($_POST['Abt_Name'][$mc][$i], 'todb');
				$dep_pd=(int)$_POST['Abt_Unterebene'][$mc][$i];
				$dep_dl=(int)$_POST['Abt_Hperson'][$mc][$i];

				// Änderungen?
				if(($dep_lv != $dep_hlv) || ($dep_nm != $dep_hnm) || ($dep_pd != $dep_hpd) || ($dep_dl != $dep_hdl))
				{
					$sql  = "UPDATE abteilung SET abt_ebene='".$dep_lv."', abt_name='".$dep_nm."', zu_abteilung=".$dep_pd.", abt_hperson=".$dep_dl;
					$sql .= " WHERE abt_id=".$abtid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					if(!$err)
					{
						$err=db_x($sql);
fxDebug($err,$sql, 2);
					}

					$chg++;
				}
			}
			// Löschen
			else if($_POST['Checkbox'][$mc][$i])
			{
				$abtid=$depid;

				$sql="DELETE FROM abteilung WHERE abt_id=".$abtid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				if(!$err)
				{
					$_POST['kostenstelle_id'][$mc][$i]=0;
					$err=db_check_delete('abteilung', 'abt_id', $abtid, $sql, false, "mandanten_id=".$GLOBALS['fxpglobals']['client']);
fxDebug($err,$sql, 2);
				}

				$del++;
			}

			// Kostenstelle
			if(!$err && $kr_aktiv && $abtid)
			{
				$gvon='00000000000000';
				$gbis='99999999999999';

				$oid=(int)db_value("SELECT kostenstelle_id FROM kostenzuordnung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id=".$abtid." AND kostenstellenart=920 AND kostendefinition=901");
				$nid=(int)$_POST['kostenstelle_id'][$mc][$i];
				// Werte verschieden, deshalb DB-Aktion
				if($oid != $nid)
				{
					// Neuanlage
					if(!$oid)
					{
						$sql  = "INSERT INTO kostenzuordnung";
						$sql .= " (mandanten_id, kostenstelle_id, id, kostenstellenart, kostendefinition, gueltig_von, gueltig_bis)";
						$sql .= " VALUES";
						$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$nid.", ".$abtid.", 920, 901, '".$gvon."', '".$gbis."')";
					}
					// Löschen
					else if(!$nid)
					{
						$sql  = "DELETE FROM kostenzuordnung";
						$sql .= " WHERE kostenstelle_id=".$oid." AND id=".$abtid." AND kostenstellenart=920 AND kostendefinition=901 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					}
					// Ändern
					else
					{
						$sql  = "UPDATE kostenzuordnung";
						$sql .= " SET kostenstelle_id=".$nid.", gueltig_von='".$gvon."', gueltig_bis='".$gbis."'";
						$sql .= " WHERE id=".$abtid." AND kostenstellenart=920 AND kostendefinition=901 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					}
					$err=db_x($sql);
fxDebug($err,$sql, 2);
				}
			}
		}
	}

	// Fehlermeldungen
	if($err)
	{
		if($ins == 1)
			error_msg(458);					// Fehler beim Anlegen des Datensatzes.
		else if($ins)
			error_msg(956);					// Fehler beim Anlegen der Datensätze.
		else if($chg == 1)
			error_msg(460);					// Fehler beim Ändern des Datensatzes.
		else if($chg)
			error_msg(952);					// Fehler beim Ändern der Datensätze.
		else if(!is_array($GLOBALS['fxpvars']['msgarr']))
		{
			if($del == 1)
				error_msg(462);				// Fehler beim Löschen des Datensatzes.
			else if($del)
				error_msg(953);				// Fehler beim Löschen der Datensätze.
		}
	}
	// Erfolgsmeldungen
	else
	{
		if($ins == 1)
			error_msg(117);					// Es wurde ein Datensatz erfolgreich angelegt.
		else if($ins)
			error_msg(114, false, $ins);	// Es wurden $var1 Datensätze erfolgreich angelegt.
		else if($chg == 1)
			error_msg(118);					// Es wurde ein Datensatz erfolgreich geändert.
		else if($chg)
			error_msg(115, false, $chg);	// Es wurden $var1 Datensätze erfolgreich geändert.
		else if($del == 1)
			error_msg(119);					// Es wurde ein Datensatz erfolgreich gelöscht.
		else if($del)
			error_msg(116, false, $del);	// Es wurden $var1 Datensätze erfolgreich gelöscht.
		else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			error_msg(1642);				// Es wurden keine Einträge angehakt.
		else
			error_msg(160);					// Es wurden keine Änderungen durchgeführt.

		// Rechte auf Personen neu ermitteln
		if($ins || $chg || $del)
			$persids=get_persons();
	}
}
?>