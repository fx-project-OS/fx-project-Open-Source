<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzerstatus_db.inc                                        //
// Version     : 21.1                                                         //
// Begin       : 2020-10-26                                                   //
// Last Change : 2021-04-08                                                   //
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
 * PF 79: User Accounts
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
	if(isset($_POST['Checkbox']) && fxIsArray($_POST['Checkbox']))
	{
		reset($_POST['Checkbox']);
		$akey=key($_POST['Checkbox']);

		$userchanged=array();
		foreach($_POST['Checkbox'][$akey] as $i => $chk)
		{
			if($chk)
			{
				switch($Modus)
				{
					case 1:	// Passwort generieren
					case 2:	// Sperre aufheben
						$serr=send_password($_POST['Benutzer_ID'][$akey][$i]);	// 1165: User status and password have been changed successfully.
						error_msg($serr, false, $_POST['nb'][$akey][$i]);
					break;

					case 3:	// Benutzer aktivieren
					case 4:	// Benutzer deaktivieren
						if($Modus == 3)
							$nstat=1;
						else
							$nstat=2;
						$sql="UPDATE benutzer SET status_benutzer=".$nstat." WHERE benutzer_id=".$_POST['Benutzer_ID'][$akey][$i]." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
fxDebug($err,$sql, 2);
						if($err)
							error_msg(2307, false, $_POST['nb'][$akey][$i]);
						else
							$userchanged[$i]=true;
					break;
				}
			}
		}
	}
	else
		error_msg(160); // 160: Es wurden keine Änderungen durchgeführt.
}
?>