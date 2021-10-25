<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_eingabe_pre.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2018-11-11                                                   //
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
 * PF 81: Time Recording (Day)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// KW
$maskenfeldwerte['kw']=$aw;

// Datum
if(!$sum)
	$maskenfeldwerte['datum']=$datum;
// Start- und Enddatum
else
{
	$maskenfeldwerte['startdatum']=$startdatum;
	$maskenfeldwerte['enddatum']=$enddatum;
}

// Abweichung
if(!$sum)
{
	$maskenfeldwerte['ist_verzoeg']=$Ist_Verzoeg;
	$maskenfeldwerte['zeiteinheit_ist_verz']=$Zeiteinheit_Ist_Verz;
}

// abgeschlossen
if($err && $_POST['fertig'])
	$maskenfeldwerte['fertig']=$_POST['fertig'];
else if($fertig)
	$maskenfeldwerte['fertig']=$fertig;
else
	unset($maskenfeldwerte['fertig']);

// Nicht verrechenbar
if($err && $_POST['nicht_abrechenbar'])
	$maskenfeldwerte['nicht_abrechenbar']=$_POST['nicht_abrechenbar'];
else if($nicht_abrechenbar)
	$maskenfeldwerte['nicht_abrechenbar']=$nicht_abrechenbar;
else
	unset($maskenfeldwerte['nicht_abrechenbar']);

// Rel. Fertigst.
if($err && strlen($_POST['fg_relativ']))
	$maskenfeldwerte['fg_relativ']='^'.$_POST['fg_relativ'];
else if(strlen($fg_relativ))
{
	if($Projekt_ID)
	{
		$pma=get_pmethod($Projekt_ID);
		if($pma['methode'] == 1785)	// Restaufwand in Stunden => Wert von Sekunden in Stunden umrechnen
			$fg_relativ /= 3600;
	}
	$maskenfeldwerte['fg_relativ']=$fg_relativ;
}
else
	unset($maskenfeldwerte['fg_relativ']);

// Freigabe
if(!$sum)
{
	if(isset($Freigabe))
	{
		if($Freigabe > $GLOBALS['fxpglobals']['persdat']['role'])
			$maskenfeldwerte['freigabe']='~<img src="'.$GLOBALS['gfxpath'].'tl_green.png">';
		else
			$maskenfeldwerte['freigabe']='~<img src="'.$GLOBALS['gfxpath'].'tl_red.png">';
	}
	else
		$Freigabe_nv=true;
}

// Zeitdaten
if(!$sum)
{
	if(strlen($Ist_Beg_Zeit))
		$maskenfeldwerte['ist_beg_zeit']=validate_entry($Ist_Beg_Zeit,$err,'zeit');
	if(strlen($Ist_End_Zeit))
		$maskenfeldwerte['ist_end_zeit']=validate_entry($Ist_End_Zeit,$err,'zeit');
	if(strlen($Pause))
		$maskenfeldwerte['pause']=validate_entry($Pause,$err,'zeitspanne');
}
if(strlen($Zeitaufwand))
	$maskenfeldwerte['zeitaufwand']=validate_entry($Zeitaufwand,$err,'zeitspanne');

// Tätigkeit + Anmerkungen
$uca=replaceUnicode(strtr($Taetigkeit_Text,$tr_table));
$maskenfeldwerte['taetigkeit_text']=$uca['result'];
$uca=replaceUnicode(strtr($Anmerkung,$tr_table));
$maskenfeldwerte['anmerkung']=$uca['result'];

// Aufgaben
$Projekt_ID_arg=array('timereg'=>true, 'with_unknown'=>true, 'project'=>$Projekt_ID, 'person'=>$vb_pid);
$tarray=auswahl_Projekt_ID(1, '', 'Projekt_ID');
//fxDebug($tarray, '$tarray');

if(fxIsArray($GLOBALS['fxptdata']['backup']) && isset($GLOBALS['fxptdata']['backup']['aufids']))
	unset($GLOBALS['fxptdata']['backup']['aufids']);
$GLOBALS['fxptdata']['backup']['aufids']=$GLOBALS['aufids'];

if(!$Projekt_ID || !isset($tarray[$Projekt_ID]))
	$Projekt_ID=0;
$Projekt_ID_ds=0;
$Projekt_ID_arg=array('override'=>$tarray);
$maskenfeldwerte['projekt_id']=$Projekt_ID;
if($lart_id)
	$maskenfeldwerte['projekt_id'] .= '_'.$lart_id;
else if($todo_id)
	$maskenfeldwerte['projekt_id'] .= '-'.$todo_id;

// Dummy2: Aufwandsübersicht aller ausgewählten Aufgaben
$maskenfeldwerte['dummy2']='~'.fieldLinkImage('icon_overview', $GLOBALS['gfxpath'].'im_effort.png', 'loader.php?url=stunden_info_aufgaben.inc&__tr=81&vb_pid='.$vb_pid.'&mid='.$GLOBALS['fxpglobals']['client'], meldung(1565));

// Kunde
if($z_kunde)	// Kunde muss angegeben werden
{
	$Kunde_ds=0;
	$pr_check=false;
}
else
	$pr_check=true;
if(!$Kunde && $Projekt_ID && !fxIsArray($Projekt_ID))
	$Kunde=db_value("SELECT kunde FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
if(!$Kunde)
	$Kunde=0;
$Kunde_arg=array('pr_check'=>$pr_check, 'kunde_muss'=>$Kunde);
$maskenfeldwerte['kunde']=$Kunde;

// Methode
$fg_pid=(int)$Projekt_ID;
$req_fn=fxf_fn_reqFilename('stunden_tag_fg', 81);
if(strlen($req_fn))
	require($req_fn);
$maskenfeldwerte['nv_text1']='~&nbsp;<i id="fg_method" style="font-size:6pt;">'.$fg_methode.'</i>';

// -------------------------------------------------------------------------
// --  Änderungs-Modus                                                    --
// -------------------------------------------------------------------------
if($Dummy == FXP_CHANGE)
{
	// im Änderungsmodus dürfen bereits abgeschlossene (301) Projekte nicht mehr markiert werden.
	if($Projekt_ID)
	{
		$pr_stat=db_value("SELECT projekt_status FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if($pr_stat == FXP_PS_COMPLETED) // abgeschlossen
		{
			$maskenfeldwerte['fertig']=1;
			$fertig_sf=2;
		}
	}

	$maskenfeldwerte['ist_beg_zeit']=validate_entry($Ist_Beg_Zeit, $err, 'zeit');
	$maskenfeldwerte['ist_end_zeit']=validate_entry($Ist_End_Zeit, $err, 'zeit');
	if($Zeitaufwand)
		$maskenfeldwerte['zeitaufwand']=validate_entry($Zeitaufwand, $err, 'zeitspanne');
	else
		unset($maskenfeldwerte['zeitaufwand']);
	if($Pause)
		$maskenfeldwerte['pause']=validate_entry($Pause, $err, 'zeitspanne');
	else
		unset($maskenfeldwerte['pause']);

	// Tätigkeit + Anmerkungen
	$uca=replaceUnicode(strtr($Taetigkeit_Text,$tr_table));
	$maskenfeldwerte['taetigkeit_text']=$uca['result'];
	$uca=replaceUnicode(strtr($Anmerkung,$tr_table));
	$maskenfeldwerte['anmerkung']=$uca['result'];

	if(isset($icon_aendern_x))
		$verz_abr=db_values("SELECT ist_verzoeg, nicht_abrechenbar FROM zeitdaten WHERE zeit_id=".(int)$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	// Auf "Ändern" wurde geklickt
	if(is_array($verz_abr))
	{
		// Ist-Verzögerung
		if((int)$verz_abr['ist_verzoeg'] > 0)
		{
			$maskenfeldwerte['ist_verzoeg']=$verz_abr['ist_verzoeg']/3600;
			// Zeiteinheitenfeld wird auf Stunden gesetzt
			$maskenfeldwerte['zeiteinheit_ist_verz']=484;
		}
		else
		{
			unset($maskenfeldwerte['ist_verzoeg']);
			unset($maskenfeldwerte['zeiteinheit_ist_verz']);
		}

		// Nicht abrechenbar
		$maskenfeldwerte['nicht_abrechenbar']=$verz_abr['nicht_abrechenbar'];
	}

	$maskenfeldwerte['dummy']='3|'.(int)$Zeit_ID.'|'.(int)$Taetigkeit_ID;
}

// -------------------------------------------------------------------------
// --  Löschen-Modus                                                      --
// -------------------------------------------------------------------------
if($Dummy == FXP_DELETE)
{
	// Tätigkeit + Anmerkungen
	$uca=replaceUnicode(strtr($Taetigkeit_Text,$tr_table));
	$maskenfeldwerte['taetigkeit_text']=$uca['result'];
	$uca=replaceUnicode(strtr($Anmerkung,$tr_table));
	$maskenfeldwerte['anmerkung']=$uca['result'];
}

if(($Dummy == FXP_DELETE) && is_array($zd)) // Löschen
{
	// Ist-Verzögerung
	if((int)$zd['ist_verzoeg'] > 0)
	{
		$maskenfeldwerte['ist_verzoeg']=$zd['ist_verzoeg']/3600;
		// Zeiteinheitenfeld wird auf Stunden gesetzt
		$maskenfeldwerte['zeiteinheit_ist_verz']=484;
	}
	else
	{
		unset($maskenfeldwerte['ist_verzoeg']);
		unset($maskenfeldwerte['zeiteinheit_ist_verz']);
	}

	// Nicht abrechenbar
	$maskenfeldwerte['nicht_abrechenbar']=$zd['nicht_abrechenbar'];
}

if($Dummy != FXP_CHANGE) // Neuanlage
	$maskenfeldwerte['dummy']='2';

if($GLOBALS['fxpglobals']['tr'] == 81)
{
	if($gd)
		echo(maske(218));
	$maske218_nv=true;
}
?>