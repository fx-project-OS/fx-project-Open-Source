<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_bew_liste_db.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2021-01-14                                                   //
// Last Change : 2021-01-14                                                   //
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
 * PF 245: Project Plan Statuses
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if($_POST['Button_Multi'] && fxIsArray($chg_lst))
{
	$chg=0;
	foreach($chg_lst as $id => $as_diff)
	{
		// Aufwand-Soll verändern
		if(!$err)
			$err=Verschiebung($id, $as_diff);
		if($err)
			break;
		else
			$chg++;
	}

	// Erfolgsmeldung
	if(!$err)
	{
		if($chg == 1)
			error_msg(118, false);			// 118: Es wurde ein Datensatz erfolgreich geändert.
		else
			error_msg(115, false, $chg);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.
	}
}
?>