<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personalstammblatt_strg_ini.inc                              //
// Version     : 21.1                                                         //
// Begin       : 2020-12-17                                                   //
// Last Change : 2020-12-17                                                   //
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
 * PF 205: Employee's Main Sheet
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$fa=array(
	'sPersonen_ID', 'sAnz_Arb', 'sAnz_Url', 'sAnz_Kr', 'sAnz_Schu',
	'tZeitspanne_von', 'tZeitspanne_bis',
	'cAnz_Vd', 'cAnz_Ba', 'cAnz_Adr', 'cAnz_Kom', 'cAnz_Fae', 'cAnz_PersHier', 'cnicht_abrechenbar'
);
foreach($fa as $fn)
{
	$t=substr($fn,0,1);
	$f=substr($fn,1);
	switch($t)
	{
		case 's':
			${$f}=0;
			if(fxIsArray($_POST) && isset($_POST[$f]))
				${$f}=(int)$_POST[$f];
		break;
		case 'c':
			if(isset(${$f}))
				unset(${$f});
			if(fxIsArray($_POST) && isset($_POST[$f]) && $_POST[$f])
				${$f}='1';
		break;
		default:
			${$f}='';
			if(fxIsArray($_POST) && isset($_POST[$f]))
				${$f}=$_POST[$f];
		break;
	}
}

// Feld: Zeitspanne von (Zeitspanne_von)
if(strlen($Zeitspanne_von))
	$Zeitspanne_von=checkdatum('Zeitspanne_von');

// Feld: Zeitspanne bis (Zeitspanne_bis)
if(strlen($Zeitspanne_bis))
	$Zeitspanne_bis=checkdatum('Zeitspanne_bis');

// Daten umkehren, falls keine Datumsfehler aufgetreten sind und Enddatum vor dem Startdatum liegt
if(strlen($Zeitspanne_von) && strlen($Zeitspanne_bis) && (substr($Zeitspanne_von, 0, 1) != '^') && (substr($Zeitspanne_bis, 0, 1) != '^') && ($Zeitspanne_von > $Zeitspanne_bis))
{
	$swap=$Zeitspanne_von;
	$Zeitspanne_von=$Zeitspanne_bis;
	$Zeitspanne_bis=$swap;
}

// Prüfen, ob eine Person ausgewählt wurde
if(!$Personen_ID && $_POST['Button_Anzeig'])
	error_msg(1071, -1, 'Personen_ID');	// 1071: Es wurde noch keine Person ausgewählt!
?>