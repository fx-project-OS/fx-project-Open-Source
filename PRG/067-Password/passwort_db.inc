<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : passwort_db.inc                                              //
// Version     : 21.2                                                         //
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
 * PF 67: Password - Database file for saving the new password
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Encode the new password...
$epw=fxf_ecPasswd($npw, '', $GLOBALS['fxpglobals']['client'], $GLOBALS['fxpglobals']['user']);
fxDebug($epw,'$epw: $npw='.$npw, 2);
// ...and save it (also (re)activating the user by setting the status to 0)
$sql="UPDATE benutzer SET status_benutzer=0, passwort='".$epw."' WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$err=db_x($sql);
fxDebug($err,'$err: '.$sql, 1);

// Display status message if it failed ($err) or succeeded
if($err)
{
	error_msg(460);	// Error while trying to edit the record.
	$maskenfeldwerte['passwort_alt']=$opw;
	$maskenfeldwerte['passwort_alias']=$npw;
	$maskenfeldwerte['passwort_check']=$npw;
}
else
{
	error_msg(459); // The record has been changed successfully.
	$maskenfeldwerte['passwort_alt']=$npw;
}
?>