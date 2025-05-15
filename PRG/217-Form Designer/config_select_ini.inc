<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_select_ini.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-10-15                                                   //
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
 * PF 217: Form Designer - Initialization
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['no_headers']=true;

// Texte
$text_icons			= explode('|', meldung(2023, false));	// Bild hochladen|Bild einbinden|Kalkulator|Farbe|Bild|Text|Box|Rechteck|Linie
$js_save_changes	= meldung(2024, false);					// Änderungen wurden an der aktuellen Konfigurationsdatei durchgeführt. Änderungen speichern?
$js_delete_picture	= meldung(2025, false);					// Bild wirklich löschen?
$js_delete_config	= meldung(2045, false);					// Konfigurationsdatei wirklich löschen?
$js_delete_ok		= meldung(2026, false);					// Bild wurde erfolgreich gelöscht.
$unit_mm			= meldung(2027, false);					// Einheit in [mm]
$unit_pdf			= meldung(2028, false);					// Einheit in [PDF Pkt.]

// Text für JavaScript Abfrage
echo('<div id="tset" style="display:none;">');
echo('savechanges=S'.fxReplaceSpecialCharacters($js_save_changes,false));
echo('|deletepicture=S'.fxReplaceSpecialCharacters($js_delete_picture,false));
echo('|deleteconfig=S'.fxReplaceSpecialCharacters($js_delete_config,false));
echo('|deleteok=S'.fxReplaceSpecialCharacters($js_delete_ok,false));
echo('</div>'.$nl);

// Edit-Maske anzeigen
$display_edit = false;
if(fxIsArray($_POST) && ($_POST['Button_Bearbeiten'] || $_POST['Text_varLang'] || $_POST['delete']))
	$display_edit = true;

// Today
$today=substr((string)$GLOBALS['date'],0,4).'-'.substr((string)$GLOBALS['date'],4,2).'-'.substr((string)$GLOBALS['date'],6,2);

// Konfiguration speichern?
if(fxIsArray($_POST))
{
	// ...JA
	if($_POST['Button_Speichern'] || ($_POST['force_save'] == 1))
	{
		$nltr=array("\r\n"=>'<:nl:>', "\r"=>'<:nl:>', "\n"=>'<:nl:>');
		$cexp=explode('<:nl:>', strtr($_POST['Text_varLang'], $nltr));
		$conf='';
		foreach($cexp as $cline)
			$conf .= $cline.$nl_b;
//echo('<pre>'.fxHtmlEncode($conf).'</pre>');

		// Adjust change date
		$cpos=strpos($conf,'// Last Change : ');
		if($cpos !== false)
			$conf=substr((string)$conf,0,$cpos+17).$today.substr((string)$conf,$cpos+27);

		$fname=trim((string)$_POST['cpath']);
		$err=fxSave($fname, $conf, true);
		if($err)
		{
			// 220: Fehler beim Schreiben der Konfigurationsdatei!
			error_msg(220);
		}
		else
		{
			// 219: Die Konfigurationsdatei wurde erfolgreich geschrieben.
			error_msg(219);

			if($_POST['Button_Speichern'])
				$display_edit = false;
		}
	}

	// ...NEIN
	else if($_POST['force_save'] < 0)
	{
		// 160: Es wurden keine Änderungen durchgeführt.
		error_msg(160);
	}
}

// Konfiguration löschen?
if(fxIsArray($_POST) && isset($_POST['delete']) && isset($_POST['cpath']) && strlen((string)$_POST['cpath']))
{
	$fname = trim((string)$_POST['cpath']);
	if(file_exists($fname))
		unlink($fname);

	if(file_exists($fname))
	{
		// 2044: Die Konfigurationsdatei konnte nicht gelöscht werden!
		error_msg(2044);
		$err = true;
	}
	else
	{
		// 2043: Die Konfigurationsdatei wurde erfolgreich gelöscht.
		error_msg(2043);

		$display_edit = false;
	}
}

// Konfigurationsdatei deaktivieren
if(fxIsArray($_POST) && isset($_POST['inact']) && isset($_POST['cpath']) && strlen((string)$_POST['cpath']))
{
	$fname=trim((string)$_POST['cpath']);
	$nname=substr((string)$fname,0,-4).'!'.substr((string)$fname,-4);
//echo('$fname='.$fname.'<br />$nname='.$nname.'<hr />');
	rename($fname, $nname);
	if(file_exists($nname))
	{
		error_msg(3057);	// Die Konfigurationsdatei wurde erfolgreich deaktiviert.
		$_POST['kdatei']=$nname;
		$_POST['cpath']=$nname;
	}
	else
	{
		error_msg(3058);	// Die Konfigurationsdatei konnte nicht deaktiviert werden!
		$err=true;
	}
}

// Konfigurationsdatei reaktivieren
if(fxIsArray($_POST) && isset($_POST['react']) && isset($_POST['cpath']) && strlen((string)$_POST['cpath']))
{
	$fname=trim((string)$_POST['cpath']);
	$nname=substr((string)$fname,0,-5).substr((string)$fname,-4);
//echo('$fname='.$fname.'<br />$nname='.$nname.'<hr />');
	rename($fname, $nname);
	if(file_exists($nname))
	{
		error_msg(3059);	// Die Konfigurationsdatei wurde erfolgreich reaktiviert.
		$_POST['kdatei']=$nname;
		$_POST['cpath']=$nname;
	}
	else
	{
		error_msg(3060);	// Die Konfigurationsdatei konnte nicht reaktiviert werden!
		$err=true;
	}
}
?>