<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : madaten_ini.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2020-10-20                                                   //
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
 * PF 7: Employee's Contract Data
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Int. Mitarbeiter (für Personenauswahl)
$PersonenTyp=FXP_PT_EMPLOYEE;

// Im Popupkalender keine Datumsbegrenzung
$GLOBALS['fxptdata']['ignore_date_boundaries']=true;

if(isset($db_update) && ((fxIsArray($_GET) && isset($_GET['uid']) && ($_GET['uid'] == 'newtr')) || (fxIsArray($_POST) && (isset($_POST['Person_waehlen']) || !isset($_POST['Button_Multi'])))))
	unset($db_update);

//echo('Personen_ID (BEFORE)='.$Personen_ID.'<br>');
$prsarr=array();
if(isset($_POST['Personen_ID']))
{
	$up=strpos($_POST['Personen_ID'],'_');
	if($up)
		$Personen_ID=(int)substr((string)$_POST['Personen_ID'],$up+1);
	else
		$Personen_ID=(int)$_POST['Personen_ID'];

	if($Personen_ID)
		$prsarr=db_values("SELECT personen_id, pname, vorname, personentyp, geschlecht, personen_initialen FROM personen WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
}
else if(!$Personen_ID)
	$Personen_ID=0;
//echo('Personen_ID (FINAL)='.$Personen_ID.'<br>');

// Vertragsdaten Feldarrays
$verr=false;
$cbarr=array('Schwerbehindert', 'Gleitzeit', 'Zeiterfassung', 'Zeitbeschraenkung', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag', 'Sonntag');
$dtarr=array('Geburtsdatum', 'MaEintrittsDatum', 'MaAustrittsDatum');
$dzarr=array('EK', 'VK', 'WochenArbeitStd', 'Urlaubsanspruch', 'Resturlaub');
$ztarr=array('Arbeitsbeginn');
$zsarr=array('Pause');
$afarr=array('Religion', 'Firm_Adress', 'Mobilbereich');
$txarr=array('Geburtsname', 'BLZ', 'Kontonummer', 'IBAN', 'BIC', 'AnmerkungIntern');
?>