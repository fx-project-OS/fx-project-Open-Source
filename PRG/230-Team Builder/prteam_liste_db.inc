<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prteam_liste_db.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-11-16                                                   //
// Last Change : 2020-11-16                                                   //
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

if(!$err && $_POST['Button_Multi'])
{
	$subst=explode(',', meldung(16, false));
	$verb=explode(',', meldung(14, false));

	$add=0;
	$chg=0;
	$del=0;

	// Team-Name erstellen
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	{
		$wtid=id_max('wertetab', 'wertetabid', '', $GLOBALS['fxpglobals']['client']);
		$sql  = "INSERT INTO wertetab (mandanten_id, wertetabid, id_sprache, id_feld, tabwert, position)";
		$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$wtid.", ".$GLOBALS['fxpglobals']['lang'].", 1586, '".convert_string($_POST['pteam_name'], 'todb')."', 0)";
		$err=db_x($sql);
fxDebug($err, $sql, 2);
		if(!$err)
		{
			$la=fxGetAllLanguages();
			if(fxIsArray($la))
			{
				$nname='§'.convert_string($_POST['pteam_name'], 'todb');
				foreach($la as $lid => $a)
				{
					if($lid != $GLOBALS['fxpglobals']['lang'])
					{
						$sql  = "INSERT INTO wertetab (mandanten_id, wertetabid, id_sprache, id_feld, tabwert, position)";
						$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$wtid.", ".$lid.", 1586, '".$nname."', 0)";
						$err=db_x($sql);
fxDebug($err, $sql, 2);
						if($err)
						{
							error_msg(281, false, $subst[1]);	// 281: Fehler beim [Anlegen] des Team-Namens!
							break;
						}
					}
				}
			}
		}
		else
			error_msg(281, false, $subst[1]);	// 281: Fehler beim [Anlegen] des Team-Namens!
	}
	// Team-Name ändern
	else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $_POST['projektteam_id'])
	{
		if($_POST['pteam_name'] != $_POST['old_pteam_name'])
		{
			$wtid=$_POST['projektteam_id'];
			$sql="UPDATE wertetab SET tabwert='".convert_string($_POST['pteam_name'], 'todb')."' WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND wertetabid=".$wtid." AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
			$err=db_x($sql);
fxDebug($err, $sql, 2);
			if($err)
				error_msg(281, false, $subst[2]);	// 281: Fehler beim [Ändern] des Team-Namens!
		}
	}
	// Team-Name löschen
	else if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['projektteam_id'])
	{
		$wtid=$_POST['projektteam_id'];
		$sql="DELETE FROM wertetab WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND wertetabid=".$wtid;
		$err=db_x($sql);
fxDebug($err, $sql, 2);
		if($err)
			error_msg(281, false, $subst[3]);	// 281: Fehler beim [Löschen] des Team-Namens!
	}

	// Team-Member
	if(!$err && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	{
		$chk="SELECT COUNT(personen_id) FROM projektteam WHERE wertetabid=".$wtid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$del=db_value($chk);
		if($del)
		{
			$sql="DELETE FROM projektteam WHERE wertetabid=".$wtid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
fxDebug($err, $sql, 2);
		}
		if(!$del || $err)
			error_msg(286, false);	// 286: Fehler beim Löschen des Projekt-Teams!
	}
	else if(!$err)
	{
		foreach($_POST['pname'][$mc] as $z => $pn)
		{
			if($_POST['leiter'][$mc][$z])
				$ltr=1;
			else
				$ltr=0;

			// Neuanlage
			if($_POST['Checkbox'][$mc][$z] && !$_POST['oldcheck'][$mc][$z])
			{
				$sql  = "INSERT INTO projektteam (mandanten_id, wertetabid, personen_id, leiter)";
				$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$wtid.", ".$_POST['Personen_ID'][$mc][$z].", $ltr)";
				$err=db_x($sql);
fxDebug($err, $sql, 2);
				if(!$err)
					$add++;
				else
					error_msg(282, false, $subst[1], '<b>'.$_POST['pname'][$mc][$z].'</b>');	// 281: Fehler beim [Anlegen] des Team-Mitglieds [$var2]!
			}
			// Löschen
			else if(!$_POST['Checkbox'][$mc][$z] && $_POST['oldcheck'][$mc][$z])
			{
				$sql="DELETE FROM projektteam WHERE wertetabid=".$wtid." AND personen_id=".$_POST['Personen_ID'][$mc][$z]." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
fxDebug($err, $sql, 2);
				if(!$err)
					$del++;
				else
					error_msg(282, false, $subst[3], '<b>'.$_POST['pname'][$mc][$z].'</b>');	// 281: Fehler beim [Löschen] des Team-Mitglieds [$var2]!
			}
			// Teamleiter ernennen oder entziehen
			else if($_POST['leiter'][$mc][$z] != $_POST['oldleiter'][$mc][$z])
			{
				$sql="UPDATE projektteam SET leiter=".$ltr." WHERE wertetabid=".$wtid." AND personen_id=".$_POST['Personen_ID'][$mc][$z]." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
fxDebug($err, $sql, 2);
				if(!$err)
					$chg++;
				else
					error_msg(282, false, $subst[2], '<b>'.$_POST['pname'][$mc][$z].'</b>');	// 281: Fehler beim [Ändern] des Team-Mitglieds [$var2]!
			}

			if($err)
				break;
		}
	}

	// Erfolgsmeldungen
	if(!$err)
	{
		error_msg(280, false, $verb[$GLOBALS['fxpglobals']['action']-1]);		// 280: Der Team-Name wurde erfolgreich $var1.
		if($chg)
			error_msg(285, false, '<b>'.$chg.'</b>');	// 285: Bei $var1 Person(en) wurde erfolgreich der Teamleiter-Status geändert.
		if($del)
			error_msg(284, false, '<b>'.$del.'</b>');	// 284: Es wurde(n) erfolgreich $var1 Person(en) aus dem Projekt-Team genommen.
		if($add)
			error_msg(283, false, '<b>'.$add.'</b>');	// 283: Es wurde(n) erfolgreich $var1 Person(en) dem Projekt-Team zugeordnet.

		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
		{
			$prid=$_POST['Projekt_ID'];
			$_POST=array();
			if($prid)
				$_POST['Projekt_ID']=$prid;
			$maske468_nv=true;
			$show_liste=false;
		}
	}
}
?>