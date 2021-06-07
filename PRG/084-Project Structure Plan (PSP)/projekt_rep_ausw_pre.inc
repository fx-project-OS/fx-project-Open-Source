<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_rep_ausw_pre.inc                                     //
// Version     : 21.1                                                         //
// Begin       : 2020-12-23                                                   //
// Last Change : 2020-12-23                                                   //
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
 * PF 84: Project Structure Plan (PSP)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

${'cpo'.$GLOBALS['fxpglobals']['tr']}=true;

// Kosten-Checkbox ausblenden, falls Benutzer keine Kosten sehen darf
$sql="SELECT kosten FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$bkst=(int)db_value($sql);
if($bkst == 2161)		// Keine Kosten
{
	$bkst=0;

	$Kosten_rep_nv=true;
	$Kosten_rep='';
}
else if($bkst == 2162)	// Alle Kosten
	$bkst=3;
else					// Mitarbeiterrolle
{
	// Geschäftsführer [2] oder Budgetgenehmigung [-4] dürfen immer ALLE Kosten sehen
	if(isset($GLOBALS['fxpglobals']['persdat']['roles'][2]) || isset($GLOBALS['fxpglobals']['persdat']['roles'][-4]))
		$bkst=3;
	else
		$bkst=-1;
}
//fxDebug($bkst, $sql);

if(($GLOBALS['fxpglobals']['tr'] == 118) || ($GLOBALS['fxpglobals']['tr'] == 152) || ($GLOBALS['fxpglobals']['tr'] == 84))
{
	// "Alle" als Standard-Wert in Projekt-Auswahlfeld in Projektstatusreport
	if($GLOBALS['fxpglobals']['tr'] == 152)
	{
		if(sizeof($Proj_Arr) > 1)
			$ProjektID_ds=1077;	// Alle Vorgänge
		else
			$ProjektID_ds=0;
		$ProjektID_arg=array('override'=>$Proj_Arr);
	}
	else if($GLOBALS['fxpglobals']['tr'] == 84)
	{
		$ProjektID_ds=0;
		$ProjektID_arg=array('mp_only'=>true);
	}
	$maske302_sf=true;

	// Maskenfelder füllen: "Projekt-Status"
	// Beim 1.Aufruf oder wenn Report=Startseite ist => füllen per Projektauswahl-Array
	if($auto_invoke || !fxIsArray($_POST) || !isset($_POST['ProjektID']))
	{
		// Projektstati (pmenu_aktiv, pmenu_inakt, pmenu_gepla, pmenu_abges)
		$maskenfeldwerte['pmenu_gepla']		= $GLOBALS['fxpglobals']['filter']['projektstatus_297'];	// geplant
		$maskenfeldwerte['pmenu_inakt']		= $GLOBALS['fxpglobals']['filter']['projektstatus_299'];	// inaktiv
		$maskenfeldwerte['pmenu_aktiv']		= $GLOBALS['fxpglobals']['filter']['projektstatus_300'];	// aktiv
		$maskenfeldwerte['pmenu_abges']		= $GLOBALS['fxpglobals']['filter']['projektstatus_301'];	// abgesschl.

		// Projekt-Zeitraum (pmenu_von, pmenu_bis)
		$maskenfeldwerte['pmenu_von']		= $GLOBALS['fxpglobals']['filter']['proj_menu_von'];
		$maskenfeldwerte['pmenu_bis']		= $GLOBALS['fxpglobals']['filter']['proj_menu_bis'];
		// bis Unt.-Ebene (Level)
		$maskenfeldwerte['level']			= '';

		// Vorgang (ProjektID)
		$maskenfeldwerte['projektid']		= $GLOBALS['fxpglobals']['project'];
		// Auftraggeber (Kunde)
		$maskenfeldwerte['kunde']			= $GLOBALS['fxpglobals']['filter']['kunde'][0];
		// Projektmanager (Projektmanager)
		$maskenfeldwerte['projektmanager']	= $GLOBALS['fxpglobals']['filter']['projektmanager'][0];
		// Projektleiter (Projektleiter)
		$maskenfeldwerte['projektleiter']	= $GLOBALS['fxpglobals']['filter']['projektleiter'][0];
		// Fäh.-Gruppe (MaArt_ID)
		$maskenfeldwerte['maart_id']		= $GLOBALS['fxpglobals']['filter']['maart_id'][0];
	}
	else
	{
		if(strlen($pmenu_von) && (strlen($pmenu_von) != 14))
			$pmenu_von=validate_entry($pmenu_von, $derr, 'datum');
		if(strlen($pmenu_bis) && (strlen($pmenu_bis) != 14))
			$pmenu_bis=validate_entry($pmenu_bis, $derr, 'datum');

		// Projektstati (pmenu_aktiv, pmenu_inakt, pmenu_gepla, pmenu_abges)
		$maskenfeldwerte['pmenu_aktiv']			= $pmenu_aktiv;
		$maskenfeldwerte['pmenu_inakt']			= $pmenu_inakt;
		$maskenfeldwerte['pmenu_gepla']			= $pmenu_gepla;
		$maskenfeldwerte['pmenu_abges']			= $pmenu_abges;
		// ohneNummer (ohne_Nummer)
		$maskenfeldwerte['ohne_nummer']			= $ohne_Nummer;
		// Kritisch (kritisch)
		$maskenfeldwerte['kritisch']			= $kritisch;
		// Aufg. rek. (recalc_tasks)
		$maskenfeldwerte['recalc_tasks']		= $recalc_tasks;
		// Kosten (kosten_rep)
		$maskenfeldwerte['kosten_rep']			= $Kosten_rep;

		// Projekt-Zeitraum (pmenu_von, pmenu_bis)
		$maskenfeldwerte['pmenu_von']			= $pmenu_von;
		$maskenfeldwerte['pmenu_bis']			= $pmenu_bis;
		// bis Unt.-Ebene (Level)
		$maskenfeldwerte['level']				= $Level;
		// Aufgaben anz. (Aufgaben)
		$maskenfeldwerte['aufgaben']			= $Aufgaben;
		// Projekthistorie (Projekt_HistoryID)
		$maskenfeldwerte['projekt_historyid']	= $Projekt_HistoryID;
		// Max.-Proj. (projekt_kuerzen)
		$maskenfeldwerte['projekt_kuerzen']		= $projekt_kuerzen;
		// Ausführl. (ausfuehrlich)
		$maskenfeldwerte['ausfuehrlich']		= $ausfuehrlich;
		// Max.Textzeichen (text_kuerzen)
		$maskenfeldwerte['text_kuerzen']		= $text_kuerzen;
		// anteilig (anteilig)
		$maskenfeldwerte['anteilig']			= $anteilig;
		// Aufwand (Aufwaende)
		$maskenfeldwerte['aufwaende']			= $Aufwaende;

		// Mutterprojekt (Mutterprojekt_ID)
		$maskenfeldwerte['mutterprojekt_id']	= $Mutterprojekt_ID;
		// Vorgang (ProjektID)
		$maskenfeldwerte['projektid']			= $ProjektID;
		// Auftraggeber (Kunde)
		$maskenfeldwerte['kunde']				= $Kunde;
		// Projektmanager (Projektmanager)
		$maskenfeldwerte['projektmanager']		= $Projektmanager;
		// Projektleiter (Projektleiter)
		$maskenfeldwerte['projektleiter']		= $Projektleiter;
		// Fäh.-Gruppe (MaArt_ID)
		$maskenfeldwerte['maart_id']			= $MaArt_ID;
		// Abteilung (Abt_ID)
		$maskenfeldwerte['abt_id']				= $Abt_ID;
	}

	// als "Ansprechpartner Kunde" angemeldet => keine Auswahl auf fremde Kunden möglich
	if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
		$Kunde_ds=0;
	// Default-Select "Alle"
	else
		$Kunde_ds=1422;
	$Kunde_arg=array('pr_check' => true);

	// Default-Select "Alle"
	$Projektleiter_ds	= 1422;
	$Projektleiter_arg	= array('pl_check' => true);

	// Default-Selects hier auch auf "Alle" setzen
	$Projektmanager_ds	= 1422;
	$MaArt_ID_ds		= 1422;
	$Abt_ID_ds			= 1422;

	// Projektauswahlfeld füllen
	if(!strlen($maskenfeldwerte['projektid']))
		$maskenfeldwerte['projektid']=$GLOBALS['fxpglobals']['project'];

	// Chechboxen "Kosten" und "Aufwand" vorselektieren bei Aufruf, wenn Felder nicht versteckt sind
	if(!$Button_Submit)
	{
		if(!$Kosten_rep_nv)
			$maskenfeldwerte['kosten_rep']=1;
		if(!$Aufwaende_nv)
			$maskenfeldwerte['aufwaende']=1;

		if(($GLOBALS['fxpglobals']['tr'] == 84) && !$Projekt_HistoryID_nv && !isset($Projekt_HistoryID))
			$maskenfeldwerte['projekt_historyid']=1;
		if(($GLOBALS['fxpglobals']['tr'] == 84) && !$Level_nv && !isset($Level))
			$maskenfeldwerte['level']=0;
	}
}

// Anzeigearten des PSP
if($GLOBALS['fxpglobals']['tr'] == 84)
{
	$deftx=meldung(1894,false);	// Default
	$radta=explode('|', meldung(3185,false));	// Vertikale Darstellung mit kombinierten Projekt- und Aufgabenebenen|Vertikale Darstellung mit getrennten Projekt- und Aufgabenebenen
	if(fxIsArray($_POST) && isset($_POST['dmode']))
		$dmode=(int)$_POST['dmode'];
	if(!isset($dmode))
		$dmode=0;
	$space='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
	$ra=array();
	foreach($radta as $rmode => $rtxt)
	{
		$cls='darkgrey';
		if($rmode == $dmode)
			$cls='black';
		$ra[$rmode]=$space.'<font class="'.$cls.'">'.$rtxt.'</font>';
		if(!$rmode)
			$ra[$rmode] .= ' <i class="lightgrey">('.$deftx.')</i>';
	}
	$rf=fieldRadio('dmode', $dmode, $ra, 'v', 10);

	$maskenfeldwerte['textzeile'] ='~<div class="lit" style="float:left;width:99px;">'.get_text(163,'20').':</div>'.$rf;
}

// Mussfelder
switch($GLOBALS['fxpglobals']['tr'])
{
	case 84:	// Controlling->Projektdaten->Projektstrukturplan (PSP)
	case 118:	// Controlling->Projektdaten->Vorgangsreport
		$ProjektID_muss			= true;
	break;
}

// Felder verstecken
switch($GLOBALS['fxpglobals']['tr'])
{
	case 84:	// Controlling->Projektdaten->Projektstrukturplan (PSP)
		$Mutterprojekt_ID_nv	= true;

		$Aufgaben_nv			= true;

		$projekt_kuerzen_nv		= true;

		$ohne_Nummer_nv			= true;
		$kritisch_nv			= true;
		$recalc_tasks_nv		= true;
		$ausfuehrlich_nv		= true;
		$anteilig_nv			= true;
		$text_kuerzen_nv		= true;
		$MaArt_ID_nv			= true;

		$Kosten_rep_nv			= true;
		$Aufwaende_nv			= true;
		$Abt_ID_nv				= true;
	break;

	case 118:	// Controlling->Projektdaten->Vorgangsreport
		$ohne_Nummer_nv			= true;
		$kritisch_nv			= true;

		$Aufgaben_nv			= true;
		$projekt_kuerzen_nv		= true;
		$text_kuerzen_nv		= true;
		$anteilig_nv			= true;

		$Mutterprojekt_ID_nv	= true;

		$hr5_nv					= true;
		$Textzeile_nv			= true;

		$maskenfeldwerte['recalc_tasks']='~'.fieldCheckbox('rtd', 0, false, false, true);
	break;

	case 152:	// Controlling->Kosten->Projektstatusreport (inern)
		$Abt_ID_nv				= true;
		$Mutterprojekt_ID_nv	= true;
		$Aufgaben_nv			= true;
		$Kosten_rep_nv			= true;
		$Aufwaende_nv			= true;
		$kritisch_nv			= true;
		$Projekt_HistoryID_nv	= true;
		$text_kuerzen_nv		= true;
		$ausfuehrlich_nv		= true;
		$ohne_Nummer_nv			= true;
		$hr5_nv					= true;
		$Textzeile_nv			= true;
	break;

	default:
		$ProjektID_nv			= true;
		$Level_nv				= true;
		$projekt_kuerzen_nv		= true;
		$Kosten_rep_nv			= true;
		$Aufwaende_nv			= true;
		$kritisch_nv			= true;
		$recalc_tasks_nv		= true;
		$Projekt_HistoryID_nv	= true;
		$text_kuerzen_nv		= true;
		$anteilig_nv			= true;
		$ohne_Nummer_nv			= true;
		$hr5_nv					= true;
		$Textzeile_nv			= true;
	break;
}

// Feldliterale überschreiben
switch($GLOBALS['fxpglobals']['tr'])
{
	case 84:	// Controlling->Projektdaten->Projektstrukturplan (PSP)
		$Projekt_HistoryID_lit	= meldung(232, false);	// Mit Krit. Pfad
		$Level_lit				= meldung(2830, false);	// Puffertag(e)
	break;
}
?>