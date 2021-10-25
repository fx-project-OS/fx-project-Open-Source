<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : zk_aend_strg_val.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 146: Time Transfers
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err && ($_POST['Button_Anzeigen'] || $_POST['Button_Multi']))
{
	// Ist-Beginn muss kleiner sein als Ist-Ende, vorausgesetzt es wurde ein Ist-Ende eingetragen
	if(strlen($_POST['Ist_Beg_Dtm']) && strlen($_POST['Ist_End_Dtm']) && ($_POST['Ist_Beg_Dtm'] > $_POST['Ist_End_Dtm']))
	{
		// Die zeitlichen Beziehungen zwischen den Feldern $var1 und $var2 sind unlogisch. Bitte überprüfen Sie Ihre Eingaben! $var3
		error_msg(651, array('Ist_Beg_Dtm', 'Ist_End_Dtm'), 'Ist_Beg_Dtm', 'Ist_End_Dtm');
		$err = true;
	}

	// Wurde eine Person oder alle ausgewählt?
	if(!$_POST['Personen_ID'])
	{
		// Es wurde noch keine Person ausgewählt!
		error_msg(1071, array('Personen_ID'));
		$err = true;
	}
}
?>