<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faefaehigkeiten_val.inc                                      //
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
 * PF 17: Skills
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(!$err && $MaArt_ID && fxIsArray($_POST) && fxIsArray($_POST['MaFaehigkeit_ID']) && isset($_POST['Button_Multi']))
{
//fxDebug('_POST');

	reset($_POST['MaFaehigkeit_ID']);
	$mc=key($_POST['MaFaehigkeit_ID']);

	$add=0;
	foreach($_POST['MaFaehigkeit_ID'][$mc] as $hn => $id)
	{
		$val_zeilennummer=$hn;

		$id=(int)$id;
		if(!$id)
			continue;

		if(!fxIsArray($_POST['MaFaehigkeit_Name']) || !fxIsArray($_POST['MaFaehigkeit_Name'][$mc]) || !isset($_POST['MaFaehigkeit_Name'][$mc][$hn]) || is_null($_POST['MaFaehigkeit_Name'][$mc][$hn]) || !strlen((string)$_POST['MaFaehigkeit_Name'][$mc][$hn]))
		{
			if(($id > 0) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
			{
				// The field "$var1" must be filled. Please enter a value.
				error_msg(472, false, "MaFaehigkeit_Name");
				$err=true;
			}
		}
		else if(($id < 0) && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
			$add++;
	}

	if(!$err && !$add && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	{
		// No data was entered for creating.
		error_msg(168);
		$err=true;
	}
}
?>