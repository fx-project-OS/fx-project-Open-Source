<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_select_pre.inc                                            //
// Version     : 24.1                                                         //
// Begin       : 2021-01-18                                                   //
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
 * PF 28: Ressourceplanner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske96_sf=true;

// Sachmittelplanung
if($GLOBALS['fxpglobals']['tr'] == 102)
{
	$Projekt_ID_ds=1422;	// Alle
	$Sachmittel_ID_ds=1422;
	$Sachmitteltyp_ds=1422;

	$Sachmittel_ID_arg=array('ressource'=>true);

	$Personen_ID_nv=true;
	$Sachmittelart_nv=true;

	if($GLOBALS['fxpglobals']['action'] == 2)
	{
		$Zeitspanne_von_nv=true;
		$Zeitspanne_bis_nv	= true;

		$Projekt_ID_muss=true;
		$Projekt_ID_ds=0;
		$Projekt_ID_arg=array('task_only'=>true);
	}
}
// Aufruf der MA-Planung -> Feld Sachmitteltyp überflüssig => Personenauswahl ausführen
else if($GLOBALS['fxpglobals']['tr'] == 28)
{
	$Name_Sachmittel_nv=true;
	$Sachmittel_ID_nv=true;
	$Sachmitteltyp_nv=true;
	$Sachmittelart_nv=true;
}
else
{
	$Personen_ID_nv=true;
	$Name_Sachmittel_nv=true;
	$Sachmittel_ID_nv=true;
	$Zeitspanne_von_nv=true;
	$Zeitspanne_bis_nv=true;
}

// Felder wieder mit abgeschickten Werten füllen
if(isset($Projekt_ID) && $Projekt_ID) 
	$maskenfeldwerte['projekt_id']=$Projekt_ID;
if(!is_null($_POST['Personen_ID']) && strlen((string)$_POST['Personen_ID']))
	$maskenfeldwerte['personen_id']=$_POST['Personen_ID'];
if(!is_null($_POST['Sachmittel_ID']) && strlen((string)$_POST['Sachmittel_ID']))
	$maskenfeldwerte['sachmittel_id']=$_POST['Sachmittel_ID'];
if(isset($Sachmitteltyp) && $Sachmitteltyp) 
	$maskenfeldwerte['sachmitteltyp']=$Sachmitteltyp;
if(isset($Sachmittelart) && $Sachmittelart) 
	$maskenfeldwerte['sachmittelart']=$Sachmittelart;
if(isset($Zeitspanne_von) && $Zeitspanne_von && ($zp_v != '00000000000000'))
	$maskenfeldwerte['zeitspanne_von']=$zp_v;
if(isset($Zeitspanne_bis) && $Zeitspanne_bis && ($zp_b != '99999999999999'))
	$maskenfeldwerte['zeitspanne_bis']=$zp_b;

// Standardwert="Alle" statt "-leer-"
if(($GLOBALS['fxpglobals']['tr'] == 28) || ($GLOBALS['fxpglobals']['tr'] == 101) || (($GLOBALS['fxpglobals']['tr'] == 102) && ($GLOBALS['fxpglobals']['action'] != 2)))
	$Projekt_ID_ds=1422;

// "-leer-" - Eintrag in Selectfeld nicht ermöglichen
$Sachmittelart_ds=0;
?>