<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektstatus_ini.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-12-23                                                   //
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
 * PF 26: Gantt
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$pa_debug=0;		// Debuginfos, die angezeigt werden sollen (s.o.)
$fehler=false;		// Fehlervariable, falls Fehler auftritt
$feiertage=true;	// Feiertage in Kalender darstellen

// Kalenderpopup-Icons anzeigen
${'cpo'.$GLOBALS['fxpglobals']['tr']}=true;

// Auswählen der Zeiteinheit
define('STUNDEN', 916);
define('TAGE', 917);
define('MONATE', 918);
define('JAHRE', 919);

// Kalenderwerte leeren
$GLOBALS['fxptdata']['cal']=array();

// Kommt der Benutzer aus Projektmenü-Popup
if($GLOBALS['gproject'])
{
	$_POST=array(
		"ProjektID"				=> $GLOBALS['gproject'],
		"Projektart"			=> 0,

		"Zeitspanne_von"		=> '',
		"Zeitspanne_bis"		=> '',
		"AutoZeitspanne"		=> '1',
		"Projekt_Status"		=> 0,
//		"zeitanzeige"			=> TAGE,

		"Level"					=> '',
		"nur_plandaten"			=> '',
		"Kalenderanzeigemodus"	=> 0,

		"Button_ProAnzeigen"	=> true
	);
}

// Texte
$pa_string=array
(
	STUNDEN				=> meldung(STUNDEN, true), 			// Stunde(n)
	TAGE				=> meldung(TAGE, true),				// Tag(e)
	MONATE				=> meldung(MONATE, true),			// Monat(e)
	JAHRE				=> meldung(JAHRE, true),			// Jahr(e)

	FXP_PS_PLANNED		=> meldung(FXP_PS_PLANNED, true),	// geplant
	FXP_PS_INACTIVE		=> meldung(FXP_PS_INACTIVE, true),	// inaktiv
	FXP_PS_ACTIVE		=> meldung(FXP_PS_ACTIVE, true),	// aktiv
	FXP_PS_COMPLETED	=> meldung(FXP_PS_COMPLETED, true),	// abgeschlossen

	FXP_PRJ_MAIN		=> meldung(585, true), 				// Projekt
	FXP_PRJ_SUB			=> meldung(582, true),				// Teilprojekt
	FXP_PRJ_TASK		=> meldung(621, true), 				// Aufgabe

	"pstatus"			=> meldung(1102, true),				// Projekt-Status
	"pname"				=> meldung(1103, true),				// Projekt-Name
	"pdauer"			=> meldung(1104, true),				// Projekt-Dauer
	"pkategorie"		=> get_text(1, '10'),				// Kategorie
	"zeitspanne"		=> meldung(1105, true),				// Zeitspanne
	"info"				=> meldung(1106, true),				// Info
	"ka"				=> meldung(1107, true),				// (Keine Angaben)
	"szeit"				=> meldung(1114, true),				// Soll-Zeitraum
	"izeit"				=> meldung(1115, true),				// Ist-Zeitraum
	"abh"				=> get_text(381, 'text'),			// Abhängigkeiten
	"abh_sub"			=> meldung(1259, true)				// Abhängige Projekte/Aufgaben
);

$problemtexte=array
(
	1289 => meldung(1289, true),							// Der tatsächliche Aufwand ($var1) ist grösser als der geplante Aufwand ($var2)!
	1290 => meldung(1290, true),							// Der tatsächliche Projektbeginn liegt über dem geplanten Projektende!
	1291 => meldung(1291, true),							// Der tatsächliche Projektbeginn liegt $var1 Tag(e) über dem geplanten Projekstart!
	1292 => meldung(1292, true),							// Das geplante Projektende wurde bereits überschritten. Die Aufagbe ist jedoch noch nicht fertiggestellt!
	1293 => meldung(1293, true),							// Das geplante Projektende wurde bereits überschritten. Die Aufgabe wurde jedoch noch nicht begonnen!
	1294 => meldung(1294, true),							// Es liegen bereits $var1 Tag(e) über dem geplanten Projektstart!
	1295 => meldung(1295, true)								// Es liegen bereits $var1 Tag(e) über dem geplanten Projektstart. Die Aufgabe wurde jedoch noch nicht begonnen!
);

$today=substr((string)$GLOBALS['datetime'], 0,8);	// aktuelles Datum ermitteln


/////////////////////////////////////////////////
// Vorbelegung der Felder für die Maske (#131) //
/////////////////////////////////////////////////

// Projekt (ProjektID) - Auswahlfeld
$ProjektID_ds=1077;	// Alle Projekte
if(fxIsArray($_GET) && isset($_GET['pid']))	// $pid wurde übergeben, d.h. es wurde auf "Projekt/Aufgaben"-Link gedrückt, deshalb das ganze ausgewählte Projekt anzeigen
{
	$pid=$_GET['pid'];
	if($pid == 'AP')
		$_POST['ProjektID']=0;
	else
		$_POST['ProjektID']=(int)$pid;

	$_POST['Button_ProAnzeigen']=true;
}
$ProjektID=0;
if(!$GLOBALS['gproject'] && ($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
	$ProjektID=$GLOBALS['fxpglobals']['project'];
else if(fxIsArray($_POST) && isset($_POST['ProjektID']))
	$ProjektID=(int)$_POST['ProjektID'];
$maskenfeldwerte['projektid']=$ProjektID;

// Projektart (Projektart) - Auswahlfeld
$Projektart_ds=0;
$Projektart_arg=array('arr'=>explode('|', meldung(3238)));	// 0=Alles anzeigen, 1=Nur Projekte anzeigen + 2=Nur Aufgaben anzeigen
$Projektart=0;
if(fxIsArray($_POST) && isset($_POST['Projektart']))
	$Projektart=(int)$_POST['Projektart'];
$maskenfeldwerte['projektart']=$Projektart;


// Autom. Zeitspanne (Autozeitspanne) - Checkbox
$auto=false;
if(fxIsArray($_POST) && isset($_POST['AutoZeitspanne']))
{
	$auto=true;
	$maskenfeldwerte['autozeitspanne']='1';
}
else
	$maskenfeldwerte['autozeitspanne']='';

// Status (Projekt_Status) - Auswahlfeld
$Projekt_Status_ds=1422;	// Alle
$Projekt_Status=0;
if(fxIsArray($_POST) && isset($_POST['Projekt_Status']))
	$Projekt_Status=(int)$_POST['Projekt_Status'];
$maskenfeldwerte['projekt_status']=$Projekt_Status;


// bis Unt.-Ebene (Level) - Textfeld
$lvl=666;
if(fxIsArray($_POST) && isset($_POST['Level']) && strlen((string)$_POST['Level']))
{
	$lvl=abs((int)$_POST['Level']);
	$maskenfeldwerte['level']=$lvl;
}
else
	$maskenfeldwerte['level']='';

// Nur Plandaten (nur_plandaten) - Checkbox
$jplan=false;
if(fxIsArray($_POST) && isset($_POST['nur_plandaten']))
{
	$jplan=true;
	$maskenfeldwerte['nur_plandaten']='1';
}
else
	$maskenfeldwerte['nur_plandaten']='';

// K-Anzeige (Kalenderanzeigemodus) - Auswahlfeld
$Kalenderanzeigemodus_ds=1260;	// Automatisch
$Kalenderanzeigemodus=0;
if(fxIsArray($_POST) && isset($_POST['Kalenderanzeigemodus']))
	$Kalenderanzeigemodus=(int)$_POST['Kalenderanzeigemodus'];
$maskenfeldwerte['kalenderanzeigemodus']=$Kalenderanzeigemodus;


// Zeitraum (Zeitspanne_von + Zeitspanne_bis) - Textfeld
$pstvon='';
$pstbis='';
if(fxIsArray($_POST) && isset($_POST['Zeitspanne_von']))
	$pstvon=trim((string)$_POST['Zeitspanne_von']);
if(fxIsArray($_POST) && isset($_POST['Zeitspanne_bis']))
	$pstbis=trim((string)$_POST['Zeitspanne_bis']);

$von=$pstvon;
$bis=$pstbis;

// Start- und Endzeitpunkt ermitteln
$req_fn=fxf_fn_reqFilename('projektstatus', 26);
if(strlen((string)$req_fn))
	require($req_fn);
//echo('$pstvon=['.$pstvon.'] &rarr; $von=<b>'.$von.'</b> -- $pstbis=['.$pstbis.'] &rarr; $bis=<b>'.$bis.'</b><hr />');

// Fehlerprüfung, ob Enddatum vor Beginndatum liegt
if(strlen((string)$pstvon) && strlen((string)$pstbis))
{
	$von=substr((string)$von,0,8);
	$bis=substr((string)$bis,0,8);
	if($bis < $von)
	{
		error_msg(1065, false); // Endzeit vor Startzeit
		$fehler=true;
	}
}

$maskenfeldwerte['zeitspanne_von']='';
if(strlen((string)$pstvon))
	$maskenfeldwerte['zeitspanne_von']=$von;
$maskenfeldwerte['nv_text1']='~-';
$maskenfeldwerte['zeitspanne_bis']='';
if(strlen((string)$pstbis))
	$maskenfeldwerte['zeitspanne_bis']=$bis;
?>