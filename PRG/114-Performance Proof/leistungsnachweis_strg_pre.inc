<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : leistungsnachweis_strg_pre.inc                               //
// Version     : 21.2                                                         //
// Begin       : 2021-01-18                                                   //
// Last Change : 2021-01-18                                                   //
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
 * 114: Performance Proof
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$maske295_sf=true;

// "Zeitspanne von" + "Zeitspanne bis"
$maskenfeldwerte['zeitspanne_von']=$von;
$maskenfeldwerte['zeitspanne_bis']=$bis;

// "Max. Textzeichen" (text_kuerzen)
if(!strlen($text_kuerzen))
{
	if($Button_Submit || ($_GET['tr'] == 114))
		$text_kuerzen='50';
	else
		$text_kuerzen='0';
}
$maskenfeldwerte['text_kuerzen']=$text_kuerzen;

// "Max. Projektb." (projekt_kuerzen)
if(!strlen($projekt_kuerzen))
	$projekt_kuerzen='0';
$maskenfeldwerte['projekt_kuerzen']=$projekt_kuerzen;

// "Vorgang" (Projekt_ID)
$maskenfeldwerte['projekt_id']=$Projekt_ID;

// "Auftraggeber" (Kunde)
$Kunde_ds=1545;	// Alle Kunden
$maskenfeldwerte['kunde']=$Kunde;

// "Person" (Personen_ID)
$maskenfeldwerte['personen_id']=$Personen_ID;

// "Anzeigeart" (Anzeigeart: )
$Anzeigeartselect="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid IN (1553,917,1554,918,919,740,741) AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
//$asa=db_values($Anzeigeartselect,'*'); fxDebug($asa,$Anzeigeartselect);
$Anzeigeartselectfeld='tabwert';
$Anzeigeartselectwert='wertetabid';
$Anzeigeart_ds=1260;// "Automatisch"
$maskenfeldwerte['anzeigeart']=$Anzeigeart;

// "Zeilenumbruch" (Zeilenumbruch)
if($Zeilenumbruch)
	$maskenfeldwerte['zeilenumbruch']=1;
else
	$maskenfeldwerte['zeilenumbruch']='';

// "Zwischensumme" (zwischensumme)
if($zwischensumme)
	$maskenfeldwerte['zwischensumme']=1;
else
	$maskenfeldwerte['zwischensumme']='';

// "Leereinträge" (leer_anzeigen)
if($leer_anzeigen)
	$maskenfeldwerte['leer_anzeigen']=1;
else
	$maskenfeldwerte['leer_anzeigen']='';

// "Projekt-Nr." (Vorgangsnummer)
if($Vorgangsnummer)
	$maskenfeldwerte['vorgangsnummer']=1;
else
	$maskenfeldwerte['vorgangsnummer']='';

// "Reisekosten" (Reisedaten)
if($Reisedaten)
	$maskenfeldwerte['reisedaten']=1;
else
	$maskenfeldwerte['reisedaten']='';

// "Unbekannt" (unbekannt_anzeigen)
if($unbekannt_anzeigen)
	$maskenfeldwerte['unbekannt_anzeigen']=1;
else
	$maskenfeldwerte['unbekannt_anzeigen']='';

// "Freigabe" (Freigabe)
$Check_Freigabe=true;
if($Freigabe)
	$maskenfeldwerte['freigabe']=1;
else
	$maskenfeldwerte['freigabe']='';
?>