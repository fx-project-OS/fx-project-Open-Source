<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : settings_email_ini.inc                                       //
// Version     : 21.2                                                         //
// Begin       : 2020-10-14                                                   //
// Last Change : 2020-10-14                                                   //
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
 * PF 246: Automated Emails - Initialization
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$bit=array();
$cs=0;	// Colspan
$mcs=16;	// max. Anzahl an Checkboxen=Personengruppen

// Personengruppen
$sql="SELECT wertetabid, aktionscode FROM settings_email WHERE mandanten_id=0 AND kategorie_id=1 ORDER BY feld";
$pgr=get_array($sql, 'aktionscode', 'wertetabid');
fxDebug($pgr,$sql, 2);

if(fxIsArray($pgr))
{
	// Checkbox-Texte umändern
	foreach($pgr as $ac => $gn)
	{
		$bit[$cs]=$ac;
		${'nv_check'.$cs.'_lit'}='<span id="nvhl'.$cs.'" tooltip="'.get_text($gn, '20').'" style="cursor:help;">'.get_text($gn, '5').'</span>';

		$cs++;
	}

	// Restliche Checkboxen ausblenden
	$c=$cs;
	while($c < $mcs)
	{
		${'nv_check'.$c.'_nv'}=true;

		$c++;
	}
}
?>