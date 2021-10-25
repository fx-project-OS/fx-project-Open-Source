<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : abteilungen_val.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2020-10-19                                                   //
// Last Change : 2020-10-19                                                   //
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
 * PF 80: Department Administration
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$mc=$GLOBALS['_maskcounter'];
if(fxIsArray($_POST) && isset($_POST['Button_Multi']) && fxIsArray($_POST['Abt_Ebene']) && fxIsArray($_POST['Abt_Ebene'][$mc]))
{
	foreach($_POST['Abt_Ebene'][$mc] as $i => $abtebene)
	{
		$val_zeilennummer=$i;
		if($_POST['Abt_Ebene'][$mc][$i] || $_POST['Abt_Name'][$mc][$i] || $_POST['Abt_Hperson'][$mc][$i])
		{
			if(!strlen($_POST['Abt_Ebene'][$mc][$i]))
			{
				error_msg(749, false, 'Abt_Ebene');	// Das Feld $var1 darf nicht leer sein. Bitte geben Sie einen Text ein!
				$err=true;
			}
			if(!strlen($_POST['Abt_Name'][$mc][$i]))
			{
				error_msg(749, false, 'Abt_Name');	// Das Feld $var1 darf nicht leer sein. Bitte geben Sie einen Text ein!
				$err=true;
			}
			if($kr_aktiv && $kostenstelle_id_muss && !$_POST['kostenstelle_id'][$mc][$i])
			{
				error_msg(750, false, 'kostenstelle_id');	// Das Feld $var1 muss ausgewählt werden!
				$err=true;
			}
		}
		// Prüfen, ob übergeordnete Abteilung unterschiedlich zu aktueller Abteilung
		if(($_POST['Abt_ID'][$mc][$i] >= 0) && ($_POST['Abt_ID'][$mc][$i] == $_POST['Abt_Unterebene'][$mc][$i]))
		{
			error_msg(798, false, 'Abt_Unterebene');	// Das Feld $var1 darf nicht auf sich selbst verweisen!
			$err=true;
		}
	}
}
?>