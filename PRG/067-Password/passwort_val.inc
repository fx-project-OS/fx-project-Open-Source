<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : passwort_val.inc                                             //
// Version     : 21.1                                                         //
// Begin       : 2020-09-22                                                   //
// Last Change : 2020-09-22                                                   //
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
 * PF 67: Password - Validation file for checking the submitted input fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!fxIsArray($_GET) && fxIsArray($_POST) && isset($_POST['Button_Aendern']))
{
	foreach($_POST as $pkey => $pval)
	{
		if(substr($pkey,0,9) == 'Passwort_')
		{
			if(strlen($pval))
				$_POST[$pkey]=fxDecodeValue($pval);
			else
				$_POST[$pkey]='';
		}
	}

	// Does the old password match the stored password?
	// ...old password
	$opw=$_POST['Passwort_alt'];
fxDebug($opw,'$opw', 3);
	// ...stored password
	$sql="SELECT passwort FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$dpw=db_value($sql);
fxDebug($dpw,'$dpw: '.$sql, 3);
	// ...don't match?
	$cpw=fxf_ecPasswd($opw, $dpw, $GLOBALS['fxpglobals']['client'], $GLOBALS['fxpglobals']['user']);
fxDebug($cpw,'$cpw: $opw='.$opw.', $dpw='.$dpw, 2);
	if(!$cpw)
	{
		error_msg(1495, -1, 'Passwort_alt'); // The old password is not correct! Please enter it again.
		$err=true;
	}
	else
		$maskenfeldwerte['passwort_alt']=$opw;

	// Does the new password match the repetition?
	// ...new password
	$npw=$_POST['Passwort_alias'];
fxDebug($npw,'$npw', 3);
	// ...repeat password
	$rpw=$_POST['Passwort_check'];
fxDebug($rpw,'$rpw', 3);
	// ...don't match?
	if($npw !== $rpw)
	{
		error_msg(860, -1, 'Passwort_check'); // The "new" password does not correspond to the repeated entry.
		$err=true;
		$maskenfeldwerte['passwort_alias']=$npw;
	}
}
?>