<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaubsplan_ini.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2021-01-15                                                   //
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
 * PF 92: Absence + Timetable Chart
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$pb_debug=0;		// Debuginfos, die angezeigt werden sollen (s.o.)
$fehler=false;		// Fehlervariable, falls Fehler auftritt
$feiertage=true;	// Feiertage in Kalender darstellen

$err=false;
$today=$GLOBALS['datetime'];
// Kalenderpopup-Icons anzeigen
$cpo92=true;

// Farblegende-Infos holen (Texte, Farbnamen, etc.)
if(!is_array($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]))
	farblegende('vars');
extract($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]);

// Datumsfeld: Von (Zeitspanne_von)
if(fxIsArray($_POST) && isset($_POST['Zeitspanne_von']))
	$_POST['Zeitspanne_von']=trim((string)$_POST['Zeitspanne_von']);
else
	$_POST['Zeitspanne_von']='';
if(!strlen((string)$_POST['Zeitspanne_von']) && !isset($_POST['Button_Anzeigen']))
	$_POST['Zeitspanne_von']=get_entry(substr((string)$today, 0, 4).'0101', "datum");
$verr=false;
$von=validate_entry($_POST['Zeitspanne_von'], $verr, "datum", 1);
if($verr)
{
	error_msg($verr, false, 'Zeitspanne_von');
	$err=true;
}
else
	$von=substr((string)$von, 0, 8);

// Datumsfeld: Bis (Zeitspanne_bis)
if(fxIsArray($_POST) && isset($_POST['Zeitspanne_bis']))
	$_POST['Zeitspanne_bis']=trim((string)$_POST['Zeitspanne_bis']);
else
	$_POST['Zeitspanne_bis']='';
if(!strlen((string)$_POST['Zeitspanne_bis']) && !isset($_POST['Button_Anzeigen']))
	$_POST['Zeitspanne_bis']=get_entry(substr((string)$today, 0, 4).'1231', "datum");
$verr=false;
$bis=validate_entry($_POST['Zeitspanne_bis'], $verr, "datum", 1);
if($verr)
{
	error_msg($verr, false, 'Zeitspanne_bis');
	$err=true;
}
else
	$bis=substr((string)$bis, 0, 8);

// ...falls Startdatum > Enddatum, dann tauschen
if(!$err && ($von > $bis))
{
	$bck=$von;
	$von=$bis;
	$bis=$bck;
}

$maskenfeldwerte['zeitspanne_von']=$von;
$maskenfeldwerte['zeitspanne_bis']=$bis;

// Auswahlfeld: Kal.-Anzeige (Kalenderanzeigemodus)
if(isset($_POST['Kalenderanzeigemodus']))
	$_POST['Kalenderanzeigemodus']=(int)$_POST['Kalenderanzeigemodus'];
else if(!isset($_POST['Button_Anzeigen']))
	$_POST['Kalenderanzeigemodus']=1255;
else
	$_POST['Kalenderanzeigemodus']=1260;
// ...automatischer Kalenderanzeigemodus
if(!$_POST['Kalenderanzeigemodus'] || ($_POST['Kalenderanzeigemodus'] == 1260))
{
	$_POST['Kalenderanzeigemodus']=1260;
	if(!$err)
	{
		$td=fxp_date_diff($von, $bis, 'D');
		if($td < 39)
			$ke='D';
		else if($td < 235)
			$ke='W';
		else if($td < 725)
			$ke='M';
		else
			$ke='Y';
	}
	else
		$ke='D';
}
// ...je nach Kalenderanzeigemodus die Darstellungsart festlegen
else
{
	$ke_array=array(1255=>"D", 1256=>"W", 1257=>"M", 1258=>"Y");
	$ke=$ke_array[$_POST['Kalenderanzeigemodus']];
}

$Kalenderanzeigemodus_ds=1260;	// Default-Select für Kalender: "Automatisch"
$maskenfeldwerte['kalenderanzeigemodus']=$_POST['Kalenderanzeigemodus'];

// Checkboxen: Urlaub (Urlaub) / Schulung (Schulung) / Krankheit (Krank) / Gleittag (Gleitzeit) / Ist-Stunden / Aktive MA (akt_ma)
$cb_array=array('Urlaub', 'Schulung', 'Krank', 'Gleitzeit', 'ISTStunden', 'akt_ma');
foreach($cb_array as $cbf)
{
	if(isset($_POST[$cbf]) && $_POST[$cbf])
		$_POST[$cbf]=1;
	else if(isset($_POST['Kalenderanzeigemodus']))
		$_POST[$cbf]='';
	else if(!isset($_POST['Button_Anzeigen']))
		$_POST[$cbf]=1;
	else
		$_POST[$cbf]='';

	$maskenfeldwerte[strtolower((string)$cbf)]=$_POST[$cbf];
}

// Auswahlfeld: Abteilung (Abt_ID)
if(isset($_POST['Abt_ID']))
	$_POST['Abt_ID']=(int)$_POST['Abt_ID'];
else
	$_POST['Abt_ID']=0;
$Abt_ID_ds=0;
$maskenfeldwerte['abt_id']=$_POST['Abt_ID'];

// Programm
if(!$err)
{
	// Texte aus Datenbank holen
	$pb_string=array(
		'zeitspanne'		=> meldung(1105, true),		// Zeitspanne
		'mitarbeiter'		=> meldung(298, true),		// Mitarbeiter
		'info'				=> meldung(1106, true),		// Info
		'ka'				=> meldung(1107, true),		// (Keine Angaben)
		'resturlaub'		=> meldung(1560, true),		// Übertr. / Jahres- / Resturl.
		'resturlaub_tj'		=> meldung(2792, true),		// Übertr. / Teil- (Jahres-) / Resturl. (Jahr)
		'urlaub'			=> meldung(1489, true),		// (Antr.) Urlaub
		'sonderurlaub'		=> meldung(2883, true),		// (Antr.) Sonderurl.
		'schulung'			=> meldung(1500, true),		// (Antr.) Schulung
		'krank'				=> meldung(1501, true),		// (Antr.) Krank
		'gleit'				=> meldung(1356, true),		// (Antr.) Gleittag
		'abwesend'			=> meldung(2884, true),		// Abwesend
		'istd'				=> get_text(914, '20'),		// IST-Stunden
		'zeit'				=> meldung(879, true),		// Zeit
		'tage'				=> meldung(917, true),		// Tag(e)
		'jahr'				=> meldung(1170, true),		// Jahr

		'tt_headline'		=> meldung(2793, true),		// Urlaubsberechnung für
		'tt_vacationyear'	=> meldung(2794, true),		// Eingetragener Gesamtjahresurlaubsanspruch für
		'tt_pvacation'		=> meldung(2795, true),		// Teilurlaubsanspruch für
		'tt_carryvacation'	=> meldung(2796, true),		// Urlaubsübertrag aus Vorjahr
		'tt_totalvacation'	=> meldung(2797, true),		// Gesamturlaubsanspruch für
		'tt_vacationtaken'	=> meldung(2798, true),		// Urlaub bisher genommen im Jahr
		'tt_remvacation'	=> meldung(2799, true),		// Resturlaub bis Ende

		'tt_tvacation'		=> meldung(2800, true)		// Gesamturlaubsanspruch für
	);

	// Werte für Farbbalken aus Datenbank holen
	$farbnamen=array();
	$farbbalken=db_values("SELECT wertetabid, tabwert, position FROM wertetab WHERE id_feld=582 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
	if(fxIsArray($farbbalken))
	{
		foreach($farbbalken as $value)
			$farbnamen[$value['wertetabid']]=$value['tabwert'];
	}

	// 1354: Für die graphische Anzeige muss zumindest Urlaub, Krank, Schulung, Gleittag oder Ist-Stunden selektiert sein!
	if($_POST['Button_Anzeigen'] && !$_POST['Urlaub'] && !$_POST['Krank'] && !$_POST['Schulung'] && !$_POST['Gleitzeit'] && !$_POST['ISTStunden'])
	{
		error_msg(1354, false);
		$err=true;
	}
}
?>