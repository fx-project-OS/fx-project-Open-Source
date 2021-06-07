<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : netzplan_ausw_pre.inc                                        //
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
 * PF 226: Net Plan Diagram (NPD)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$maske462_sf=true;
${'cpo'.$tr}=true;

// Auswahlfelder:
// ...Mutterprojekt (Mutterprojekt_ID)
$Mutterprojekt_ID_ds=0;
// ...Auftraggeber (Kunde)
$Kunde_ds=1422;	// "Alle"
$Kunde_arg=array('pr_check' => true);
// ...Projektmanager (Projektmanager)
$Projektmanager_ds=1422;	// "Alle"
// ...Projektleiter (Projektleiter)
$Projektleiter_ds=1422;	// "Alle"
$Projektleiter_arg=array('pl_check' => true);

// ...als Auftraggeber angemeldet => keine Auswahl auf fremde Kunden ermöglichen
if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
	$Kunde_ds=0;

// Feldliteral überschreiben
$kritisch_lit=meldung(232, false);	// Mit Krit. Pfad

// Werte setzen
foreach($tva as $tfn => $tvn)
{
	$vp=strpos($tvn, '|');

	$tt=strtolower(substr($tfn, 0, 1));
	$tfn=substr($tfn, 1);
	$tfnl=strtolower($tfn);

	$tdv=substr($tvn, $vp+1);
	$tvn=substr($tvn, 0, $vp);

	$maskenfeldwerte[$tfnl]=${$tvn};
}
?>