<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektstrukturplan_ini.inc                                  //
// Version     : 21.1                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2021-01-11                                                   //
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
 * PF 84: Project Structure Plan (PSP)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$sel=array();
$buffer=14;
$cpath=true;
$top_pid=0;

if(isset($GLOBALS['fxptdata']['pdf']))
	unset($GLOBALS['fxptdata']['pdf']);

if($Button_Submit)
{
	// Erstellen der Auswahlbedingung
	// ...Projektstatus
	$sel['pst']='';
	$pst=array('gepla'=>FXP_PS_PLANNED, 'aktiv'=>FXP_PS_ACTIVE, 'inakt'=>FXP_PS_INACTIVE, 'abges'=>FXP_PS_COMPLETED);
	foreach($pst as $pvs => $sti)
	{
		$sel['pst'.$sti]=${'pmenu_'.$pvs};
		if(${'pmenu_'.$pvs})
		{
			if(strlen($sel['pst']))
				$sel['pst'] .= ',';
			$sel['pst'] .= $sti;
		}
	}
	// ...Zeitraum
	$sel['sd']=substr(trim($pmenu_von), 0, 8);
	$sel['ed']=substr(trim($pmenu_bis), 0, 8);
	// ...Hauptverantwortliche Personen
	$sel['cust']=(int)$Kunde;
	$sel['pl']=(int)$Projektleiter;
	$sel['pm']=(int)$Projektmanager;

	// Puffer
	$buffer=max(0, (int)$Level);

	// Krit. Pfad
	if(!$Projekt_HistoryID)
		$cpath=false;

	// Mutterprojekt-ID
	$top_pid=(int)$ProjektID;

	// Anzeigeart
	if(fxIsArray($_POST) && isset($_POST['dmode']))
		$dmode=(int)$_POST['dmode'];
	else if(!isset($dmode))
		$dmode=0;
}
?>