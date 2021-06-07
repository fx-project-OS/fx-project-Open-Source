<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzer_ini.inc                                             //
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
 * PF 30: User Definition
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['fxptdata']['ignore_date_boundaries']=true;

// Benutzer_ID
$Benutzer_ID=0;
if(isset($_POST['Benutzer_ID']))
{
	$Benutzer_ID=(int)$_POST['Benutzer_ID'];
	$GLOBALS['fxptdata']['backup']['Benutzer_ID']=$Benutzer_ID;
}
if(($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && isset($GLOBALS['fxptdata']['backup']))
	extract($GLOBALS['fxptdata']['backup']);

// Validierung
if($_POST['Button_Multi'] && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
{
	// Benutzername
	if(!strlen($_POST['Name_Benutzer']))
	{
		$err=true;
		error_msg(472, -1, 'Name_Benutzer'); // 472: Das Feld $var1 muss gefüllt sein. Bitte geben Sie einen Wert ein.
	}
	else
	{
		$Name_Benutzer=strtolower($_POST['Name_Benutzer']);

		// Prüfen, ob Benutzername bereits belegt ist
		$sql  = "SELECT b.benutzer_id, p.vorname, p.pname FROM benutzer b, personen p";
		$sql .= " WHERE ".db_like('b.name_benutzer', $Name_Benutzer)." AND p.personen_id=b.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && ($Benutzer_ID))
			$sql .= " AND b.benutzer_id<>".$Benutzer_ID;
		$check_ben=db_values($sql);
		if(fxIsArray($check_ben))
		{
			$err=true;
			$pname=trim($check_ben['pname']);
			$vname=trim($check_ben['vorname']);
			if(strlen($vname))
				$pname=$vname.' '.$pname;
			error_msg(1918, -2, 'Name_Benutzer', $Name_Benutzer, $pname); // 1918: Der Benutzername [<b>$var1</b>] ist bereits der Person [<b>$var2</b>] zurgeordnet! Bitte wählen Sie einen anderen Namen aus.
		}
	}

	// E-Mail-Adresse
	$verr=false;
	$_POST['E_Mail_Adresse']=validate_entry($_POST['E_Mail_Adresse'], $verr, 'email');
	if($verr)
	{
		$err=true;
		error_msg($verr, -1, 'E_Mail_Adresse');
	}

	// Gültigkeit von
	$d1err=false;
	$_POST['gueltig_von']=validate_entry($_POST['gueltig_von'], $d1err, 'datum');
	if($d1err)
	{
		$err=true;
		error_msg($d1err, -1, 'gueltig_von');
	}
	// Gültigkeit bis
	$d2err=false;
	$_POST['gueltig_bis']=validate_entry($_POST['gueltig_bis'], $d2err, 'datum');
	if($d2err)
	{
		$err=true;
		error_msg($d2err, -1, 'gueltig_bis');
	}
	if(!$d1err && !$d2err && strlen($_POST['gueltig_von']) && strlen($_POST['gueltig_bis']) && ($_POST['gueltig_von'] > $_POST['gueltig_bis']))
	{
		$swap_date=$_POST['gueltig_von'];
		$_POST['gueltig_von']=$_POST['gueltig_bis'];
		$_POST['gueltig_bis']=$swap_date;
	}

	// Passwort
	$pw1='';
	if(isset($_POST['Passwort_alias']) && strlen($_POST['Passwort_alias']))
		$pw1=fxDecodeValue($_POST['Passwort_alias']);
	$pw2='';
	if(isset($_POST['Passwort_check']) && strlen($_POST['Passwort_check']))
		$pw2=fxDecodeValue($_POST['Passwort_check']);
	if($pw1 != $pw2)
	{
		$err=true;
		error_msg(860, array('Passwort_alias', 'Passwort_check')); // 860: Das neue Passwort stimmt nicht mit der Wiederholung überein.
	}

	// zugeh. Person
	$benutzerpersid=(int)$_POST['benutzerpersid'];
	if(!$benutzerpersid)
	{
		$err=true;
		error_msg(789, -1, 'benutzerpersid'); // 789: Im Feld "$var1" muss ein Eintrag ausgewählt werden!
	}

}

// Prüfen, ob der Administrator gelöscht werden soll?
if(($Benutzer_ID == 1) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	unset($Benutzer_ID);
	$err=true;
	error_msg(167, false); // 167: Administratoren dürfen nicht gelöscht werden!
}

// Bei Fehler oder falls auf "Auswählen" geklickt wurde DB-Programm nicht ausführen
if($err || $_POST['Button_Select'])
	$db_update=false;
?>