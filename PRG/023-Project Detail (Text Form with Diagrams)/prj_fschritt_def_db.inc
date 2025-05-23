<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_fschritt_def_db.inc                                      //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$Projekt_ID=(int)$Projekt_ID;
if($Projekt_ID < 1)
	$Projekt_ID=-666;
if(!$err && ($Projekt_ID < 1) && ($_POST['Button_okay'] || $_POST['Button_Anpassen']))
	$Projekt_ID = id_max('projekte', 'projekt_id');

if($fs_aktiv)
{
//echo(__FILE__.': <B>$Projekt_ID: '.$Projekt_ID.'</B><HR>');
	if(!$err && ($Projekt_ID > 0))
	{
		$sql='';
		$fsgrad=0.0;

		$vMT=(int)$_POST['methode'];
		if(!$vMT)
			$vMT=1784;
		$vZP=(int)$_POST['zeitpunkt'];
		if(!$vZP)
			$vZP=1790;
		$vTG=(int)$_POST['tage'];

		$err=projektfortschritt($Projekt_ID, 0, 0.0, $vMT, $vZP, $vTG, $vPART);
fxDebug($err, 'projektfortschritt('.$Projekt_ID.', 0, 0.0, '.$vMT.', '.$vZP.', '.$vTG.', '.$vPART.');', 3);
		if($err)
			error_msg($err, false);
	}
}
?>