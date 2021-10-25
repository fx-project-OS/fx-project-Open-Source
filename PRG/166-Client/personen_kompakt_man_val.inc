<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_man_val.inc                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2020-10-06                                                   //
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
 * PF 166: Client validation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$_ncerr=false;
$err_man=false;
$deact_user=array();

// New client number
$_ncid=0;
if(fxIsArray($_POST) && isset($_POST['Mandanten_ID']))
	$_ncid=(int)$_POST['Mandanten_ID'];

// Source client number
$_scid=0;
if(fxIsArray($_POST) && isset($_POST['Nachschlage_Art']))
	$_scid=(int)$_POST['Nachschlage_Art'];

// Base validation for creating a new client
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// Client number must be > 1 ...
	if(!$err_man && ($_ncid <= 1))
	{
		// The new client number must be bigger 1!
		error_msg(1914, -1, 'mandanten_id');
		$err_man=true;
		$_ncerr=394;
		if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$_ncerr]))
			$GLOBALS['fxpvars']['lookups'][$_ncerr]['Mandanten_ID']=$GLOBALS['fxpvars']['lookups'][$_ncerr]['mandanten_id'];
	}

	// ... and cannot be taken already
	if(!$err_man && ($_ncid > 1))
	{
		$sql="SELECT pname FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$_ncid;
		$mpn=db_value($sql);
		if($mpn)
		{
			// The desired new client number is already occupied by [$var1]!
			error_msg(1915, -2, 'mandanten_id', '<b>'.$mpn.'</b>');
			$err_man=true;
			$_ncerr=394;
			if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$_ncerr]))
				$GLOBALS['fxpvars']['lookups'][$_ncerr]['Mandanten_ID']=$GLOBALS['fxpvars']['lookups'][$_ncerr]['mandanten_id'];
		}
	}
}

// Validation only for Create or Change modes
if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	// Name
	if(!strlen($Pname))
	{
		// The field "$var1" cannot be empty. Please enter a text!
		error_msg(749, false, 'Pname');
		$err_man=true;
	}

	if($_cid != $GLOBALS['fxpglobals']['client'])
	{
		// Valid from
		$d1err=false;
		$_POST['gueltig_von']=validate_entry($_POST['gueltig_von'], $d1err, 'datum');
		if($d1err)
		{
			$err_man=true;
			error_msg($d1err, -1, 'gueltig_von');
		}
		// Valid to
		$d2err=false;
		$_POST['gueltig_bis']=validate_entry($_POST['gueltig_bis'], $d2err, 'datum');
		if($d2err)
		{
			$err_man=true;
			error_msg($d2err, -1, 'gueltig_bis');
		}
		if(!$d1err && !$d2err && strlen($_POST['gueltig_von']) && strlen($_POST['gueltig_bis']) && ($_POST['gueltig_von'] > $_POST['gueltig_bis']))
		{
			$swap_date=$_POST['gueltig_von'];
			$_POST['gueltig_von']=$_POST['gueltig_bis'];
			$_POST['gueltig_bis']=$swap_date;
		}
		$gueltig_von=$_POST['gueltig_von'];
		$gueltig_bis=$_POST['gueltig_bis'];
	}

	// User name
	if(!strlen($Name_Benutzer))
	{
		// The field "$var1" cannot be empty. Please enter a text!
		error_msg(749, false, 'Name_Benutzer');
		$err_man=true;
	}

	// Email address
	if(strlen($E_Mail_Adresse))
	{
		$E_Mail_Adresse=validate_entry($E_Mail_Adresse, $verr, 'email');
		if($verr)
		{
			$err_man=true;
			error_msg($verr, array('E_Mail_Adresse'));
		}
	}

	// Password(s) (decode)
	$Passwort_alias='';
	if(isset($_POST['Passwort_alias']) && strlen($_POST['Passwort_alias']))
		$Passwort_alias=fxDecodeValue($_POST['Passwort_alias']);
	$Passwort_check='';
	if(isset($_POST['Passwort_check']) && strlen($_POST['Passwort_check']))
		$Passwort_check=fxDecodeValue($_POST['Passwort_check']);
	if($Passwort_alias != $Passwort_check)
	{
		// The "new" password does not correspond to the repeated entry.
		error_msg(860, array('Passwort_alias', 'Passwort_check'));
		$err_man=true;
	}
}

if($err_man)
	$err=true;

if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && $err)
	$_pid=0;
?>