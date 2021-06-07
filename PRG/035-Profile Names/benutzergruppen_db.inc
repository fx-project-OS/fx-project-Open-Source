<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzergruppen_db.inc                                       //
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
 * PF 35: Profile Names - Database actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(fxIsArray($_POST) && isset($_POST['Button_Multi']) && isset($_POST['Benutzergr_ID']))
{
	// Get all languages
	$la=fxGetAllLanguages();

	// Action counters
	$new_cnt=0;
	$change_cnt=0;
	$del_cnt=0;
	$act_cnt=0;
	$deact_cnt=0;

	// Submit key
	reset($_POST['Benutzergr_ID']);
	$akey=key($_POST['Benutzergr_ID']);

	// Cycle through all entries
	foreach($_POST['Benutzergr_ID'][$akey] as $l => $gid)
	{
		if(!strlen($gid))
			continue;

		$gid=(int)$gid;
		$nid=(int)$_POST['Gruppenname_ID'][$akey][$l];

		$grpname=trim($_POST['Gruppenname'][$akey][$l]);
		$grpname_http=$grpname;
		$_POST['Gruppenname'][$akey][$l]=str_replace('\\\\', '\\', $grpname_http);
		if(strlen($grpname_http))
			$grpname_db=convert_string($grpname_http, 'todb');
		else
			$grpname_db='NULL';

		// Create new profile group name
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && fxIsArray($la) && !$nid && ($grpname_db != 'NULL'))
		{
			// Check if group with this name already exists
			$sql="SELECT tabwert FROM wertetab WHERE id_feld=660 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND ".db_like('tabwert', $grpname_db);
			$chk=db_value($sql);
			if($chk)
			{
				// Sie können die Benutzerprofilgruppe [$var1] kein zweitesmal anlegen, da sie bereits vorhanden ist.
				error_msg(1659, false, $grpname_db);
				$err=true;
				break;
			}
			else
			{
				// Get new group name id
				$nid=id_max('wertetab', 'wertetabid', '', $GLOBALS['fxpglobals']['client']);

				// Insert new profile name
				foreach($la as $lang => $lname)
				{
					if($lang == $GLOBALS['fxpglobals']['lang'])
						$name=$grpname_db;
					else
						$name='§'.$grpname_db;

					$sql  = "INSERT INTO wertetab";
					$sql .= " (mandanten_id, wertetabid, id_feld, id_sprache, position, tabwert, satzvers, archiv) ";
					$sql .= "VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$nid.", 660, ".$lang.", 0, '".$name."', 0, 0)";
					$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
					if($err)
						break;
				}
				if($err)
				{
					// Fehler beim Anlegen der Benutzerprofilgruppe [$var1].
					error_msg(2073, false, $grpname_db);
					break;
				}

				$new_cnt++;
				$_POST['Checkbox'][$akey][$l]=1;
				$_POST['Gruppenname_ID'][$akey][$l]=$nid;
				$_POST['Name_Wert'][$akey][$l]=$grpname_http;
			}
		}

		// Change profile group name
		if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && fxIsArray($la) && $nid && ($grpname_db != 'NULL') && ($grpname_http != $_POST['Name_Wert'][$akey][$l]))
		{
			foreach($la as $lang => $lname)
			{
				$chk="SELECT tabwert FROM wertetab WHERE wertetabid=".$nid." AND id_sprache=".$lang." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$lgname=db_value($chk);
fxDebug($lgname,'$lgname<br />'.$chk, 3);

				$sql="";
				if(!$lgname)
				{
					$name=$grpname;
					if($lang != $GLOBALS['fxpglobals']['lang'])
						$name='§'.$name;

					$sql  = "INSERT INTO wertetab";
					$sql .= " (mandanten_id, wertetabid, id_feld, id_sprache, position, tabwert, satzvers, archiv) ";
					$sql .= "VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$nid.", 660, ".$lang.", 0, '".$name."', 0, 0)";
				}
				else
				{
					$name='';
					if($lang == $GLOBALS['fxpglobals']['lang'])
						$name=$grpname;
					else if(mb_substr($lgname,0,1) == '§')
						$name='§'.$grpname;
					if(strlen($name))
					{
						$sql  = "UPDATE wertetab";
						$sql .= " SET tabwert='".$name."' WHERE wertetabid=".$nid." AND id_sprache=".$lang." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					}
				}
				if(strlen($sql))
				{
					$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
					if($err)
						break;
				}
			}
			if($err)
			{
				// Fehler beim Ändern der Benutzerprofilgruppe [$var1] nach [$var2].
				error_msg(1660, false, $_POST['Name_Wert'][$akey][$l], $grpname);
				break;
			}
			else
			{
				$change_cnt++;
				$_POST['Name_Wert'][$akey][$l]=$grpname;
			}
		}

		// Activate profile group
		if((($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && $_POST['Checkbox'][$akey][$l] && !$gid && $nid)
		{
			$gid=id_max('benutzergruppen', 'benutzergr_id', '', '', 100);

			$sql  = "INSERT INTO benutzergruppen";
			$sql .= " (mandanten_id, benutzergr_id, gruppenname_id) ";
			$sql .= "VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$gid.", ".$nid.")"; 
			$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
			if($err)
			{
				// Fehler beim Aktivieren der Benutzerprofilgruppe [$var1].
				error_msg(1866, false, $_POST['Name_Wert'][$akey][$l]);
				break;
			}
			else
				$act_cnt++;
		}

		// Deactivate or Delete profile group
		if(((($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && !$_POST['Checkbox'][$akey][$l]) || (($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Checkbox'][$akey][$l])))
		{
			// Delete name?
			if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $nid)
			{
				$sql  = "DELETE FROM wertetab";
				$sql .= " WHERE wertetabid=".$nid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
				if($err)
				{
					// Fehler beim Löschen der Benutzerprofilgruppe [$var1].
					error_msg(2074, false, $_POST['Name_Wert'][$akey][$l]);
					break;
				}
				else
					$del_cnt++;
			}

			// Delete group?
			if($gid)
			{
				// ...delete all user assignments to group
				$chk="SELECT id_bgz FROM benutzergruppenzo WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$cvl=(int)db_value($chk);
				if($cvl)
				{
					$sql  = "DELETE FROM benutzergruppenzo";
					$sql .= " WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
				}
				// ...delete all program function assignments to group
				if(!$err)
				{
					$chk="SELECT trans_id FROM rechte_trans WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$cvl=(int)db_value($chk);
					if($cvl)
					{
						$sql  = "DELETE FROM rechte_trans";
						$sql .= " WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
					}
				}
				// ...delete group
				if(!$err)
				{
					$chk="SELECT gruppenname_id FROM benutzergruppen WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$cvl=(int)db_value($chk);
					if($cvl)
					{
						$sql  = "DELETE FROM benutzergruppen";
						$sql .= " WHERE benutzergr_id=".$gid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
fxDebug($err,'$err<br />'.$sql, 2);
					}
				}

				if($err)
				{
					// Fehler beim Deaktivieren der Benutzerprofilgruppe [$var1].
					error_msg(1867, false, $_POST['Name_Wert'][$akey][$l]);
					break;
				}
				else
					$deact_cnt++;
			}
		}
	}

	// Success messages
	if(!$err)
	{
		if($deact_cnt || $act_cnt || $change_cnt || $del_cnt || $new_cnt)
		{
			if($act_cnt)
				error_msg(1664, false, $act_cnt);		// 1664: Es wurde(n) $var1 Benutzerprofilgruppe(n) erfolgreich aktiviert.
			if($new_cnt)
				error_msg(1661, false, $new_cnt);		// 1661: Es wurde(n) $var1 neue Benutzerprofilgruppe(n) erfolgreich angelegt.
			if($change_cnt)
				error_msg(1662, false, $change_cnt);	// 1662: Es wurde(n) $var1 Benutzerprofilgruppen-Bezeichnung(en) erfolgreich geändert.
			if($del_cnt)
				error_msg(1663, false, $del_cnt);		// 1663: Es wurde(n) $var1 Benutzerprofilgruppen-Bezeichnung(en) erfolgreich gelöscht.
			if($deact_cnt)
				error_msg(1665, false, $deact_cnt);		// 1665: Es wurde(n) $var1 Benutzerprofilgruppe(n) erfolgreich deaktiviert und alle dazugehörigen Rechte gelöscht.
		}
		else
			error_msg(160);								// 160: Es wurden keine Änderungen durchgeführt.
	}
//$err=true;
}
?>