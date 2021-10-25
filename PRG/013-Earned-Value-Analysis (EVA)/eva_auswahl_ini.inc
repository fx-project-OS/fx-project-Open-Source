<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : eva_auswahl_ini.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2021-01-13                                                   //
// Last Change : 2021-10-06                                                   //
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
 * PF 13: Earned-Value-Analysis (EVA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$fields=array(
	'sProjekt_ID', 'sKunde', 'sProjektmanager', 'sProjektleiter',
	'cpmenu_gepla', 'cpmenu_aktiv', 'cpmenu_inakt', 'cpmenu_abges',
	'sAnzeigeart', 'cTabelle'
);
// Cycle through all fields
foreach($fields as $f)
{
	// ...Get value...
	$field_type	= substr($f, 0, 1);
	$field_name	= substr($f, 1);
	$fname_low=strtolower($field_name);

	// Global
	if(fxIsArray($_POST) && isset($_POST[$field_name]))
		${$field_name}=(int)$_POST[$field_name];
	// Select
	else if($field_type == 's')
		${$field_name}=0;
	// Checkbox
	else
		${$field_name}='';

	// ...and prepare for selection mask
	$maskenfeldwerte[$fname_low]=${$field_name};
}

// If the "Display" button was pressed, then show list/diagram
$show_list=false;
if(fxIsArray($_POST) && isset($_POST['Button_Anzeigen']))
	$show_list=true;
?>