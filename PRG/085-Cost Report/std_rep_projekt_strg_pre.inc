<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_projekt_strg_pre.inc                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-11-18                                                   //
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
 * PF 85: Cost Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske230_sf=true;

// Datumsfelder: "Zeitspanne von" (Zeitspanne_von) + "Zeitspanne bis" (Zeitspanne_bis)
$maskenfeldwerte['zeitspanne_von']=$von;
$maskenfeldwerte['zeitspanne_bis']=$bis;

// Auswahl: "Anzeigeart" (Anzeigeart) [Tage(3), Monat(e) + Jahre(e)]
$Anzeigeartselect="SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid>916 AND wertetabid<920 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
$Anzeigeartselectfeld='tabwert';
$Anzeigeartselectwert='wertetabid';
$Anzeigeart_ds=1260;	// Default: "Automatisch"
$maskenfeldwerte['anzeigeart']=$Anzeigeart;


// Auswahl: "Auftraggeber" (Kunde)
$knd_array=auswahl_Kunde(1);
$Kunde_ds=1545;	// Default: "Alle Auftraggeber"
$Kunde_arg=array('override' => $knd_array);
$maskenfeldwerte['kunde']=$Kunde;

// Auswahl: "Person" (Personen_ID)
$prs_array=auswahl_Personen_ID(1);
$Personen_ID_ds_array=array(0=>2141, -1=>2144);	// Default: "Alle Mitarbeiter", "Keine Mitarbeiter"
$Personen_ID_arg=array('override' => $prs_array);
$maskenfeldwerte['personen_id']=$Personen_ID;

// Auswahl: "Projekt" (Projekt_ID)
$prj_array=auswahl_Projekt_ID(1);
$Projekt_ID_ds=1077;	// "Alle Vorgänge"
$Projekt_ID_arg=array('override' => $prj_array);
$maskenfeldwerte['projekt_id']=$Projekt_ID;


// Checkboxen: "Leereinträge anz.", "Fehlzeiten anz.", "Unbekannt anz.", "Leerzuord. anz.", "Leistungsarten anz.", 'Schätzung anz.", "mit max. verrechenb. Stunden", "Sortieren nach Reise"
$cba=array('leer_anzeigen', 'fehlzeiten', 'unbekannt_anzeigen', 'leerzuordnung', 'leistungsarten', 'schaetzung', 'MaxStdTag', 'sort_travel');
foreach($cba as $cb)
{
	$cbl=strtolower((string)$cb);
	$maskenfeldwerte[$cbl]='';
	if(isset($GLOBALS[$cb]) && $GLOBALS[$cb])
		$maskenfeldwerte[$cbl]=1;
	else
		$GLOBALS[$cb]='';
}
// ..."Sortieren nach Reise" (sort_travel)
if(isset($sort_travel_hide) && $sort_travel_hide)
{
	$sort_travel_lit='';
	$maskenfeldwerte['sort_travel']='~';
}

// Auswahl: "Freigabe" (Freigabe)
if(!isset($Freigabe) || !$Freigabe)
	$Freigabe=2871;		// ...Alle Daten anzeigen
$Freigabeselect="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1285 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
$Freigabeselectfeld='tabwert';
$Freigabeselectwert='wertetabid';
$Freigabe_ds=0;
$maskenfeldwerte['freigabe']=$Freigabe;

// Auswahl: "Verrechenbar" (verrechenbar)
if(!isset($verrechenbar) || !$verrechenbar)
	$verrechenbar=2874;	// ...Alle Daten anzeigen
$verrechenbarselect="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1976 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
$verrechenbarselectfeld='tabwert';
$verrechenbarselectwert='wertetabid';
$verrechenbar_ds=0;
$maskenfeldwerte['verrechenbar']=$verrechenbar;

// Auswahl: "Abgerechnet" (abgerechnet)
if(!isset($abgerechnet) || !$abgerechnet)
	$abgerechnet=3234;	// ...Alle Daten anzeigen
$abgerechnetselect="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=2070 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
$abgerechnetselectfeld='tabwert';
$abgerechnetselectwert='wertetabid';
$abgerechnet_ds=0;
$maskenfeldwerte['abgerechnet']=$abgerechnet;

// Auswahl: "Reisekosten" (Reisekosten)
if(!isset($Reisekosten) || !$Reisekosten)
	$Reisekosten=1245;	// ...Reisekosten in eigener Zeile anzeigen
$Reisekosten_ds=0;
$maskenfeldwerte['reisekosten']=$Reisekosten;

// Auswahl: "Tagesangaben" (tagesangabe)
if(!isset($tagesangabe) || !$tagesangabe)
	$tagesangabe=3243;	// ...Alle Tagesangaben zu Kosten hinzuaddieren
$tagesangabe_ds=0;
$maskenfeldwerte['tagesangabe']=$tagesangabe;

// Auswahl: "Belege" (zahlungsart)
if(!isset($zahlungsart) || !$zahlungsart)
	$zahlungsart=2845;	// ...Alle Belege zu Kosten hinzuaddieren
$zahlungsart_ds=0;
$maskenfeldwerte['zahlungsart']=$zahlungsart;


// Transaktion 150+191: Reisekostenreport Projekte
if(($GLOBALS['fxpglobals']['tr'] == 150) || ($GLOBALS['fxpglobals']['tr'] == 191))
{
	$cbd=array('fehlzeiten', 'leerzuordnung', 'schaetzung', 'MaxStdTag');
	foreach($cbd as $cb)
	{
		$GLOBALS[$cb.'_nv']=true;
		$GLOBALS[$cb.'_lit']='';
		$maskenfeldwerte[strtolower((string)$cb)]='~';
	}

	$wfa=array('Freigabe', 'verrechenbar', 'abgerechnet', 'Reisekosten', 'tagesangabe', 'zahlungsart');
	foreach($wfa as $wf)
	{
		$GLOBALS[$wf.'_lwidth']=216;
		$GLOBALS[$wf.'_width']=544;
	}

	$Reisekosten_dis=true;
	$tagesangabe_lwidth=389;
	$zahlungsart_lwidth=389;
}


// Warnung, falls Auftraggeber leer?
if(!fxIsArray($knd_array))
{
	// 1640: Im Auswahlfeld [$var1] befinden sich keine gültigen Einträge! Evtl. müssen Sie Ihre Filtereinstellungen ändern oder zuerst Einträge erstellen.
	error_msg(1640, -2, 'Kunde', '<b>'.get_text(120, '15').'</b>');
	$Button_Anzeigen_nv=true;
}
// Warnung, falls Personen leer?
if(!fxIsArray($prs_array))
{
	// 1640: Im Auswahlfeld [$var1] befinden sich keine gültigen Einträge! Evtl. müssen Sie Ihre Filtereinstellungen ändern oder zuerst Einträge erstellen.
	error_msg(1640, -2, 'Personen_ID', '<b>'.get_text(21, '15').'</b>');
	$Button_Anzeigen_nv=true;
}
// Warnung, falls Projekte leer?
if(!fxIsArray($prj_array))
{
	// 1640: Im Auswahlfeld [$var1] befinden sich keine gültigen Einträge! Evtl. müssen Sie Ihre Filtereinstellungen ändern oder zuerst Einträge erstellen.
	error_msg(1640, -2, 'Projekt_ID', '<b>'.get_text(66, '15').'</b>');
	$Button_Anzeigen_nv=true;
}
?>