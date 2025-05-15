<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_bew_liste_val.inc                                        //
// Version     : 24.1                                                         //
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
 * @version 24.1
 */

$chg_lst=array();
if($_POST['Button_Multi'])
{
	reset($_POST['ID']);
	$mc=key($_POST['ID']);

	// Alle Aufwandseinträge durchgehen
	foreach($_POST['ID'][$mc] as $z => $id)
	{
		$val_zeilennummer=$z;

		if(!$id || !isset($_POST['nv_zahl1'][$mc][$z]))
			continue;

		$verr=false;
		$_POST['nv_zahl1'][$mc][$z]=validate_entry($_POST['nv_zahl1'][$mc][$z], $verr, 'dezimal');
		if($verr || ($_POST['nv_zahl1'][$mc][$z] < 0.00))
		{
			error_msg(1524, array('nv_zahl1'), 'SOLLStunden');	// 1524: Bitte geben Sie in das Feld $var1 eine gültige positive Zahl ein!
			$err=true;
		}
		else
		{
			$as_alt=(int)get_double($_POST['nv_zahl5'][$mc][$z]);
			$as_neu=(int)(get_double($_POST['nv_zahl1'][$mc][$z]) * 3600);
//echo('<B>Zeile '.$z.', ID='.$id.'</B> -> Alt: '.$as_alt.' -- Neu: '.$as_neu.'<BR>');
			if($as_alt != $as_neu)
			{
//echo('---> <B>changed...</B><BR>');
				$chg_lst[$id]=$as_neu-$as_alt;
			}
		}
	}

	// Wurde etwas verändert?
	if(!sizeof($chg_lst))
		error_msg(160, false); // 160: Es wurden keine Änderungen durchgeführt.
}
?>