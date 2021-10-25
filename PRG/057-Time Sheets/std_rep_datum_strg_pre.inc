<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_datum_strg_pre.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2020-07-22                                                   //
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
 * ???FILE???
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$maske192_sf=true;

// "Person" (SELECT: Personen_ID)
// ...wird standardmäßig gefüllt mit Int. (275) und Ext. (840) Mitarbeitern
$maskenfeldwerte['personen_id']=$Personen_ID;

// "Anzeigeart" (SELECT: Anzeigeart)
// ...Automatisch, Tag(e), Monat(e) + Jahre(e)
$maskenfeldwerte['anzeigeart']=$Anzeigeart;
$Anzeigeart_ds=1260;	// Automatisch
$Anzeigeartselect="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid>=917 and wertetabid<=919 and id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position";
$Anzeigeartselectfeld='tabwert';
$Anzeigeartselectwert='wertetabid';

// "Schätzung": als Auswahlfeld
$schaetzung_werte=meldung(3278);	// Ohne|Als Schätzwerte|Als Realwerte
$schaetzung_ds=0;
$schaetzung_array=explode('|',$schaetzung_werte);	// 0=Ohne, 1=Als Schätzwerte + 2=Als Realwerte
//	unset($schaetzung_array[2]);	// "Als Realwerte" in Auswahl vorerst entfernen, da nicht beauftragt wurde - würde aber bereits funktionieren
//fxDebug($schaetzung_array, $schaetzung_werte);
$maskenfeldwerte['schaetzung']=$schaetzung;

/*
	// "Schätzung": Umändern als Checkbox
	$schaetzung_lit='';
	$checked=false;
	if($schaetzung)
		$checked=true;
	$cb=fieldCheckbox('schaetzung', 1, $checked, false, false, false, '', '', '', false, 'margin-right:5px;');
	$maskenfeldwerte['schaetzung']='~<div style="float:right;"><div class="lit" style="float:left;margin-top:2px;">'.get_text(2074,'10').':&nbsp;</div>'.$cb;
*/

// "Zeitraum" (INPUT: Zeitspanne_von + Zeitspanne_bis)
// ...Validierung bereits in "std_rep_datum_strg_ini.inc"
$maskenfeldwerte['zeitspanne_von']=$von;
$maskenfeldwerte['nv_text1']='~-';
$maskenfeldwerte['zeitspanne_bis']=$bis;

// "Leereinträge", "Stunden", "Arb.Tage", "Urlaub", "Sonderurl.", "Schulung", "Krank", "Abwesend" + "Gleittag"
// (CHECKBOX: leer_anzeigen, Stunden, Arbeitstage, Urlaub, sonderurlaub, Schulung, Krank, abwesenheit + Gleitzeit)
// ...Array bereits definiert in "std_rep_datum_strg_ini.inc"
foreach($cb_fields as $cb_field => $dummy)
{
	$field=strtolower($cb_field);
	if(${'cb_'.$field})
		$maskenfeldwerte[$field]='1';
	else
		$maskenfeldwerte[$field]='';
}
?>