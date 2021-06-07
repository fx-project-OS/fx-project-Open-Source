<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : std_rep_projekt_ini.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2020-11-18                                                   //
// Last Change : 2020-11-18                                                   //
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
 * @version 21.1
 */

if($GLOBALS['fxpglobals']['tr'] == 85)
{
	$sort_travel_hide=true;
}

// Transaktion 150: Reisekostenreport Projekte
if(($GLOBALS['fxpglobals']['tr'] == 150) && ($gw || $gm || $gj))
{
	$Anzeigeart=917;			// Tage

	$Kunde=0;					// Kunde (Alle)
	$Personen_ID=(int)$vb_pid;	// Person
	$Projekt_ID=0;				// Vorgang (Alle)

	$leer_anzeigen=1;			// Leereinträge anz.
	$unbekannt_anzeigen=1;		// Unbekannt anz.
	$leistungsarten='';			// Leistungsarten anz.

	$fehlzeiten='';				// Fehlzeiten anz.
	$leerzuordnung='';			// Leerzuord. anz.
	$schaetzung='';				// Schätzung anz.
	$MaxStdTag='';				// mit max. verrechenb. Stunden

	$sort_travel='';			// Sortieren nach Reise

	$Freigabe=2871;				// Freigabe (Alle Daten anzeigen)
	$verrechenbar=2874;			// Verrechenbar (Alle Daten anzeigen)
	$abgerechnet=3234;			// Abgerechnet (Alle Daten anzeigen)
	$Reisekosten=2139;			// Reisekosten (Reisekosten allein)
	$tagesangabe=3243;			// Alle Tagesangaben zu Kosten hinzuaddieren

	// Geschäftsführer, Verwaltung oder Admin
	if($GLOBALS['fxpglobals']['persdat']['roles'][2] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || ($GLOBALS['fxpglobals']['user'] == 1))
		$zahlungsart=2845;	// Alle Belege zu Kosten hinzuaddieren
	else
		$zahlungsart=2844;	// Nur Belege der Zahlungsart [Privat] zu Kosten hinzuaddieren

	// Wochenübersicht oder Detailübersicht einer Reise
	if($gw)
	{
		if(strlen($gw) == 16)
		{
			$von=substr($gw, 0, 8).'000000';
			$bis=substr($gw, 8, 8).'240000';
		}
		else
		{
			$j=(int)substr($gw, 0, 4);
			$m=(int)substr($gw, 4, 2);
			$t=(int)substr($gw, 10, 2);

			$von=substr($gw, 0, 6).substr($gw, 10, 2).'000000';
			$bis=fx_date('Ymd240000', fx_mktime(0, 0, 0, $m, $t+6, $j));
		}
	}
	// Monatsübersicht
	if($gm)
	{
		$j=(int)substr($gm, 0, 4);
		$m=(int)substr($gm, 4, 2);

		$von=substr($gm, 0, 6).'01000000';
		$bis=fx_date('Ymd240000', fx_mktime(0, 0, 0, $m+1, 0, $j));
	}
	// Jahresübersicht
	else if($gj)
	{
		$Anzeigeart=918;	// Monate

		$von=substr($gj, 0, 4).'0101000000';
		$bis=substr($gj, 0, 4).'1231240000';
	}

	// Arrays
	$knd_array=auswahl_Kunde(1);
	$prs_array=auswahl_Personen_ID(1);
	$prj_array=auswahl_Projekt_ID(1);

	// Klick auf Button simulieren
	$_POST['Button_Anzeigen']=true;
}

// Transaktion 190: Zeitkostenreport Projekte
if($GLOBALS['fxpglobals']['tr'] == 190)
{
	$hr0_nv=true;
	$sort_travel_nv=true;

	$Reisekosten_nv=true;
	$tagesangabe_nv=true;
	$zahlungsart_nv=true;

	$Reisekosten=2140;		// Reisekosten nicht miteinrechnen
	$tagesangabe=3242;		// Keine Tagesangaben zu Kosten hinzuaddieren
	$zahlungsart=2842;		// Keine Belege zu Kosten hinzuaddieren

	$MwSt_ncs=true;
	$Bruttobetrag_ncs=true;
}

// Transaktion 191: Reisekostenreport Projekte
if($GLOBALS['fxpglobals']['tr'] == 191)
{
	$fehlzeiten='';			// Fehlzeiten anz.
	$leerzuordnung='';		// Leerzuord. anz.
	$schaetzung='';			// Schätzung anz.
	$MaxStdTag='';			// mit max. verrechenb. Stunden

	$Reisekosten=2139;		// Reisekosten alleine anzeigen
}

// Transaktion 150+191: Reisekostenreport Projekte
if(($GLOBALS['fxpglobals']['tr'] == 150) || ($GLOBALS['fxpglobals']['tr'] == 191))
{
	$Zeit_ID_ncs=true;
	$SOLLStunden_ncs=true;
	$ISTStunden_ncs=true;
	$Delta_ncs=true;
	$EK_ncs=true;
	$Ver_Std_ncs=true;
	$VK_ncs=true;
	$ertrag_ncs=true;
	$Marge_verbr_ncs=true;
}
?>