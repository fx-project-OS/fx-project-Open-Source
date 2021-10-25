<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : leistungsnachweis_strg_ini.inc                               //
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

$cpo114=true;	// Kalenderpopup-Icons anzeigen

// Prüfen, ob Programm von Zeiterfassung bzw. Reisekostenerfassung aufgerufen wurde?
$at='';
if(fxIsArray($_GET) && isset($_GET['at']))
	$at=$_GET['at'];
if($at && fxIsArray($GLOBALS['fxpglobals']['trstack']) && (($GLOBALS['fxpglobals']['trstack'][1] == 81) || ($GLOBALS['fxpglobals']['trstack'][1] == 150)))
{
	if(fxIsArray($GLOBALS['fxptdata']['backup']))
		extract($GLOBALS['fxptdata']['backup']);

	$_POST['Zeitspanne_von']		= get_entry($at, 'datum');
	$_POST['Zeitspanne_bis']		= get_entry($at, 'datum');

	$_POST['text_kuerzen']			= (string)$text_kuerzen;
	$_POST['projekt_kuerzen']		= (string)$projekt_kuerzen;

	$_POST['Projekt_ID']			= 0;
	$_POST['Kunde']					= 0;
	$_POST['Personen_ID']			= $GLOBALS['fxpglobals']['person'];

	$_POST['Anzeigeart']			= 1553;	// 1553=Detailiert; 917=Tag(e)

	$_POST['Zeilenumbruch']			= (string)$Zeilenumbruch;
	$_POST['zwischensumme']			= '';;
	$_POST['leer_anzeigen']			= '';
	$_POST['Vorgangsnummer']		= 1;
	$_POST['Reisedaten']			= 1;
	$_POST['unbekannt_anzeigen']	= 1;
	$_POST['Freigabe']				= '';

	$_POST['Button_Anzeigen']		= true;
	$Button_Anzeigen				= true;

	if($pe)
		$_POST['Personen_ID']=$pe;
	else if($Personen_ID)
		$_POST['Personen_ID']=$Personen_ID;
}

// Monatsnavigation?
if(fxIsArray($_POST) && isset($_POST['nav']) && fxIsArray($_GET) && isset($_GET['navy']) && isset($_GET['navm']))
{
	$aym=substr('0000'.$_GET['navy'],-4).substr('00'.$_GET['navm'],-2);

	$_POST['Zeitspanne_von']=get_entry($aym.'01', 'datum');
	$_POST['Zeitspanne_bis']=get_entry(ultimo($aym.'01'), 'datum');

	$_POST['Button_Anzeigen']=true;
	$Button_Anzeigen=true;
}

// Backup-Variable füllen
if($_POST['Button_Anzeigen'])
{
	unset($GLOBALS['fxptdata']['backup']);

	$GLOBALS['fxptdata']['backup']['Zeitspanne_von']		= (string)$_POST['Zeitspanne_von'];
	$GLOBALS['fxptdata']['backup']['Zeitspanne_bis']		= (string)$_POST['Zeitspanne_bis'];

	$GLOBALS['fxptdata']['backup']['text_kuerzen']			= (string)$_POST['text_kuerzen'];
	$GLOBALS['fxptdata']['backup']['projekt_kuerzen']		= (string)$_POST['projekt_kuerzen'];

	$GLOBALS['fxptdata']['backup']['Projekt_ID']			= (string)$_POST['Projekt_ID'];
	$GLOBALS['fxptdata']['backup']['Kunde']					= (string)$_POST['Kunde'];
	$GLOBALS['fxptdata']['backup']['Personen_ID']			= (string)$_POST['Personen_ID'];
	$GLOBALS['fxptdata']['backup']['Anzeigeart']			= (string)$_POST['Anzeigeart'];

	$GLOBALS['fxptdata']['backup']['Zeilenumbruch']			= (string)$_POST['Zeilenumbruch'];
	$GLOBALS['fxptdata']['backup']['zwischensumme']			= (string)$_POST['zwischensumme'];
	$GLOBALS['fxptdata']['backup']['leer_anzeigen']			= (string)$_POST['leer_anzeigen'];
	$GLOBALS['fxptdata']['backup']['Vorgangsnummer']		= (string) $_POST['Vorgangsnummer'];
	$GLOBALS['fxptdata']['backup']['Reisedaten']			= (string)$_POST['Reisedaten'];
	$GLOBALS['fxptdata']['backup']['unbekannt_anzeigen']	= (string)$_POST['unbekannt_anzeigen'];
	$GLOBALS['fxptdata']['backup']['Freigabe']				= (string)$_POST['Freigabe'];

	extract($GLOBALS['fxptdata']['backup']);
}

// Einstellungen aus Backup holen
if(!$at)
	$Personen_ID=$_POST['Personen_ID'];

// Vorbelegung der Felder für die Maske (#192)
// -------------------------------------------
// Startdatum: Keine Benutzereingaben, dann als Startdatum den ersten Wochentag der akt. Kalenderwoche
if(!$Zeitspanne_von)
	$von=substr($GLOBALS['datetime'],0,6).'01000000';
// Startdatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$von=checkdatum('Zeitspanne_von');

// Enddatum: Keine Benutzereingaben, dann Enddatum=Anzahl der Tage entspr. den Benutzereinstellungen
if(!$Zeitspanne_bis)
{
	$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
	$bis=$GLOBALS['datetime'];
	$jahr=(int)substr($bis, 0,4);
	$monat=(int)substr($bis, 4,2);
	$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $jahr));
	if($lj)
		$mo_tage[2]=29;
	$bis=substr($bis,0,6).$mo_tage[$monat].'240000';
}
// Enddatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$bis=checkdatum('Zeitspanne_bis');

// Fehlerprüfung, ob Enddatum vor Beginndatum liegt
if(!$fehler && ($bis < $von))
{
	error_msg(1065, false); // Endzeit vor Startzeit
	$fehler=true;
}

// Filtericon
$maske295_flt=0;

// Feld: Vorgang
$Projekt_ID_arg=array('build'=>true, 'use_actual'=>true, 'start_date'=>$von, 'end_date'=>$bis, 'customers'=>$Kunde, 'persons'=>$Personen_ID, 'with_all'=>true, 'with_projects'=>true);
$Proj_Arr=auswahl_Projekt_ID(1);
$Projekt_ID_arg=array('override'=>$Proj_Arr);

// Feld: Person
$Pers_Arr=auswahl_Personen_ID(1);
if(sizeof($Pers_Arr) > 1)
	$Personen_ID_ds=1410;	// Alle Personen
else
	$Personen_ID_ds=0;
$Personen_ID_arg=array('override'=>$Pers_Arr, 'Personen_ID'=>$GLOBALS['fxpglobals']['person']);

$maske295_flt=$GLOBALS['mskfilter'];

$heute=$GLOBALS['datetime'];
?>