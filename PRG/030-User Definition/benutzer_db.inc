<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzer_db.inc                                              //
// Version     : 21.2                                                         //
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
 * PF 30: User Definition
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($_POST['Button_Multi'])
{
	// Benutzernamen immer in Kleinbuchstaben
	$Name_Benutzer=strtolower($Name_Benutzer);
	$Name_Benutzer=convert_string($Name_Benutzer, 'todb');

	$E_Mail_Adresse_db="NULL";
	if(strlen($_POST['E_Mail_Adresse']))
		$E_Mail_Adresse_db="'".convert_string($_POST['E_Mail_Adresse'], 'todb')."'";

	$gueltig_von_db="NULL";
	if(strlen($_POST['gueltig_von']))
		$gueltig_von_db="'".substr($_POST['gueltig_von'], 0,8)."000000'";
	$gueltig_bis_db="NULL";
	if(strlen($_POST['gueltig_bis']))
		$gueltig_bis_db="'".substr($_POST['gueltig_bis'], 0,8)."240000'";

	$badmin="NULL";
	if($_POST['b_admin'])
		$badmin="1";

	$pw1='';
	if(isset($_POST['Passwort_alias']) && strlen($_POST['Passwort_alias']))
		$pw1=fxDecodeValue($_POST['Passwort_alias']);

	switch($GLOBALS['fxpglobals']['action'])
	{
		case FXP_CREATE:
			$spr=(int)$_POST['Sprache'];

			$benutzerid=id_max('Benutzer', 'Benutzer_ID');
			$pw=fxf_ecPasswd($pw1, '', $GLOBALS['fxpglobals']['client'], $benutzerid);

			$sql  = "INSERT INTO benutzer";
			$sql .= " (benutzer_id, name_benutzer, mandanten_id, personen_id, e_mail_adresse, passwort,";
			$sql .= " b_admin, kosten, gueltig_von, gueltig_bis)";
			$sql .= " VALUES";
			$sql .= " (".$benutzerid.", '".$Name_Benutzer."', ".$GLOBALS['fxpglobals']['client'].", ".$_POST['benutzerpersid'].", ".$E_Mail_Adresse_db.", '".$pw."',";
			$sql .= " ".$badmin.", ".$_POST['Kosten_Benutzer'].", ".$gueltig_von_db.", ".$gueltig_bis_db.")";
			$err=db_x($sql);
fxDebug($err,$sql, 2);

			// Benutzer zu Gruppe ordnen
			if(!$err && $_POST['Benutzergr_ID'])
			{
				$bgid=id_max('benutzergruppenzo', 'id_bgz');
				$sql  = "INSERT INTO benutzergruppenzo (mandanten_id, id_bgz, benutzer_id, benutzergr_id)";
				$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$bgid.", ".$benutzerid.", ".$_POST['Benutzergr_ID'].")";
				$err=db_x($sql);
fxDebug($err,$sql, 2);
			}

			// Fremdsprache als Standardsprache des Benutzers einstellen
			$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE mandanten_id=0 AND einstellungs_id=1";
			$sys_lng=(int)db_value($sql);
			if(!$err && ($spr != $sys_lng))
			{
				$ewid=id_max('s_einstellungswerte', 'einstellungswerte_id', '', '', 401);
				$sql  = "INSERT INTO s_einstellungswerte (mandanten_id, benutzer_id, einstellungswerte_id, einstellungs_id, einstellungswert)";
				$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$benutzerid.", ".$ewid.", 1, ".$spr.")";
				$err=db_x($sql);
fxDebug($err,$sql, 2);
			}

			if(!$err)
			{
				error_msg(457); // 457: Der Datensatz wurde erfolgreich angelegt.
				$Benutzer_ID=$benutzerid;
				$GLOBALS['fxptdata']['backup']['Benutzer_ID']=$Benutzer_ID;

				$_POST=array();
			}
		break;

		case FXP_CHANGE:
			$benutzerid=$Benutzer_ID;

			$sql="UPDATE benutzer SET name_benutzer='".$Name_Benutzer."'";
			if(strlen($pw1))
			{
				$pw=fxf_ecPasswd($pw1, '', $GLOBALS['fxpglobals']['client'], $benutzerid);
				$sql .= ", status_benutzer=0, passwort='".$pw."'";
			}
			$sql .= ", e_mail_adresse=".$E_Mail_Adresse_db.", personen_id=".$benutzerpersid.", b_admin=".$badmin.", kosten=".$_POST['Kosten_Benutzer'];
			$sql .= ", gueltig_von=".$gueltig_von_db.", gueltig_bis=".$gueltig_bis_db;
			$sql .= " WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
fxDebug($err,$sql, 2);
		break;

		case FXP_DELETE:
			$benutzerid=$Benutzer_ID;

			$sql="DELETE FROM benutzer WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
fxDebug($err,$sql, 2);
			if(!$err)
			{
				// Benutzer aus "BenutzerGruppenZO" löschen
				$chk=db_value("SELECT benutzer_id FROM benutzergruppenzo WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($chk)
				{
					$sql="DELETE FROM benutzergruppenzo WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
fxDebug($err,$sql, 2);
				}
			}
			if(!$err)
			{
				// Benutzer aus "S_Einstellungswerte" löschen
				$chk=db_value("SELECT benutzer_id FROM s_einstellungswerte WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($chk)
				{
					$sql="DELETE FROM s_einstellungswerte WHERE benutzer_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
fxDebug($err,$sql, 2);
				}
			}
			if(!$err)
			{
				// Benutzer aus "Rechte_Referenz" löschen
				$chk=db_value("SELECT benutzergr_id FROM rechte_referenz WHERE gruppeart=2163 AND benutzergr_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($chk)
				{
					$sql="DELETE FROM rechte_referenz WHERE gruppeart=2163 AND benutzergr_id=".$benutzerid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
fxDebug($err,$sql, 2);
				}
			}

			if(!$err)
			{
				error_msg(461); // Der Datensatz wurde erfolgreich gelöscht.
				$maske19_nv=true;
				unset($Benutzer_ID);
				if(isset($GLOBALS['fxptdata']['backup']['Benutzer_ID']))
					unset($GLOBALS['fxptdata']['backup']['Benutzer_ID']);
			}
		break;
	}
//$err=true;
}
?>