<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_a_open_ini.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Maske immer bearbeitbar
$maske317_sf=true;

$prjCheckWarning=true;
$Projekt_IDopen_ds=0;

// für "Projektauswahl"
if($GLOBALS['fxpglobals']['tr'] == 23)
{
	// Sind diese beiden Variablen bereits hier gesetzt, kommt der User von der "Projektsuche"
	if($suche)
	{
		$db_update=false;
		$Projekt_IDopen=$Projekt_ID;
	}

	// Kommt der Benutzer vom Projekt-Popupmenü
	if($GLOBALS['gproject'])
	{
		if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		{
			$Projekt_IDopen=0;
			$ppeltart=db_values("SELECT elter, projektart FROM projekte WHERE projekt_id=".$GLOBALS['gproject']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			if($ppeltart['projektart'] < FXP_PRJ_TASK)
			{
				$Elter=$GLOBALS['gproject'];
				if($ppeltart['projektart'] == FXP_PRJ_MAIN)
					$Projektart=FXP_PRJ_SUB;
				else
					$Projektart=FXP_PRJ_TASK;
			}
			else
			{
				$Elter=$ppeltart['elter'];
				$Projektart=FXP_PRJ_TASK;
			}
			$_POST['Button_Submit']=true;
			$Button_Submit=true;
		}
		else
			$Projekt_IDopen=$GLOBALS['gproject'];
	}

	// Kommt der Benutzer vom Kontrollzentrum etc.
	else if($_GET['_projektid'] && $_GET['inittr'])
		$Projekt_IDopen=$_GET['_projektid'];

	// Transaktionsaufruf
	else if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
		$Projekt_IDopen=$GLOBALS['fxpglobals']['project'];

	// Setzen der $Projekt_IDopen falls nicht gesetzt
	else if(!$Projekt_IDopen)
	{
		// Sezten des Feldes mit der zwischengespeicherten Projekt-ID
		if($GLOBALS['fxptdata']['backup']['projekt_array']['Backup']['Projekt_IDopen'])
			$Projekt_IDopen=$GLOBALS['fxptdata']['backup']['projekt_array']['Backup']['Projekt_IDopen'];
		// war keine gespeichert, dann evtl. mit "_projektid" füllen
		else if($GLOBALS['fxpglobals']['project'] && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && !$db_update)
			$Projekt_IDopen=$GLOBALS['fxpglobals']['project'];
	}

	// Project already checked out by another person?
	if(prjIsCheckedOut($Projekt_IDopen))
	{
		$Projekt_IDopen=0;
		@prjCheckIn();
	}

	// Prüfen, ob Recht auf Projekt mit dieser Aktion vorhanden
	if($Projekt_IDopen && (!$_GET['aktion'] || ($_GET['aktion'] != FXP_CREATE)))
	{
		$RProjekt_IDopen=check_projekt_id($Projekt_IDopen);
		if(!$RProjekt_IDopen)
		{
			$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".(int)$Projekt_IDopen." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$piv=db_values($sql);
//fxDebug($piv, $sql);
			if(fxIsArray($piv))
				$var1='<b>'.no_null($piv['vorgangsnummer']).' - '.$piv['name_projekt'].'</b>';
			else
				$var1='<i>ID: '.$Projekt_IDopen.'</i>';
			// Sie haben nicht die nötigen Berechtigungen, "$var1" zu ändern.
			error_msg(559, false, $var1);
			if(fxIsArray($GLOBALS['fxpvars']['msgarr']) && fxIsArray($GLOBALS['fxpvars']['msgarr'][0]))
			{
				$GLOBALS['fxpvars']['msgarr'][300000]=$GLOBALS['fxpvars']['msgarr'][0];
				unset($GLOBALS['fxpvars']['msgarr'][0]);
			}
		}
		if($RProjekt_IDopen < 1)
		{
			$err=true;
			$Projekt_IDopen=0;
		}
	}
	else
		$Projekt_IDopen=0;

	// Ist dann eine ID gesetzt, wird dieser Vorgang auch sofort geöffnet
	if($Projekt_IDopen && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && !$_POST['Button_okay'] && !$_POST['Button_Loeschen'] && !$_POST['Button_Anpassen'])
		$_POST['Button_ProAnzeigen']=true;

	if(($_POST['Button_okay'] || $_POST['Button_Loeschen'] || $_POST['Button_Anpassen']) && ($Projekt_IDopen != $GLOBALS['fxptdata']['backup']['projekt_array']['Backup']['Projekt_IDopen']))
	{
		if(isset($_POST['Button_okay']))
			unset($_POST['Button_okay']);
		if(isset($_POST['Button_Loeschen']))
			unset($_POST['Button_Loeschen']);
		if(isset($_POST['Button_Anpassen']))
			unset($_POST['Button_Anpassen']);
		$_POST['Button_ProAnzeigen']=true;
		// 2826: Der Vorgang im Auswahlfeld wurde verändert, ohne dass auf den Button [Anzeigen] geklickt wurde! Die Daten des gewählten Vorgangs wurden neu geladen.
		$err_open=true;
		error_msg(2826, false);
	}

	// Derzeitiges Projektauswahl-Array sichern für Projektauswahlfeld
	$GLOBALS['fxptdata']['backup']['projekt_array']=$GLOBALS['fxpglobals']['filter'];

	// Projekt aus Projektbaum wird nur dann vorselektiert, wenn es geändert wurde
	$maskenfeldwerte['projekt_idopen']=$Projekt_IDopen;

	// gewähltes Projekt speichern:
	//	=> immer in Projekt-Auswahlfeld verfügbar
	//	=> nie in Abhängigkeitsfeld anzeigen
	$GLOBALS['fxptdata']['backup']['projekt_array']['Backup']['Projekt_IDopen']=$Projekt_IDopen;
	// ... beim Anlegen spielt das keine Rolle
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && isset($GLOBALS['fxptdata']['backup']['elter_array']['Backup']['Projekt_IDopen']))
		unset($GLOBALS['fxptdata']['backup']['elter_array']['Backup']['Projekt_IDopen']);

	// keine Projektauswahl trotz Klick auf Button => Fehlermeldung
	if(!$err && !$Projekt_IDopen && $_POST['Button_ProAnzeigen'])
	{
		$err_open=true;
		if($GLOBALS['fxpglobals']['action'] != FXP_CREATE)
			error_msg(789, false, 'Name_Projekt');	// 789: Im Feld $var1 muss ein Eintrag ausgewählt werden!
	}

	// Bei Sysadmin/GL keine Rechteprüfung nötig
	if(($GLOBALS['fxpglobals']['persdat']['role'] != 2) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && $Projekt_IDopen)
	{
		$hatrecht=prights_check($Projekt_IDopen, pow(2, $GLOBALS['fxpglobals']['action']));
		if(!$hatrecht)
		{
			$err_VALI=true;		// Verhindert, dass die 4 Projektemasken angezeigt werden
			error_msg(2281, false);	// 2281: Sie haben kein Recht diesen Vorgang zu öffnen!
		}
	}

	// Projekt_ID setzen
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || !$Projekt_IDopen)
		$Projekt_ID=-666;
	else
		$Projekt_ID=$Projekt_IDopen;
}

// für "Warnmeldungen"
else if($GLOBALS['fxpglobals']['tr'] == 143)
{
	if($_POST['Button_ProAnzeigen'])
		$GLOBALS['fxptdata']['backup']['Projekt_IDopen']=$Projekt_IDopen;

	if($GLOBALS['fxptdata']['backup']['Projekt_IDopen'])
		$maskenfeldwerte['projekt_idopen']=$GLOBALS['fxptdata']['backup']['Projekt_IDopen'];

	$Projekt_IDopen_arg=array('rp_projektleiter'=>$GLOBALS['fxpglobals']['person']);
	$Proj_Arr=auswahl_Projekt_IDopen(1);
	if((sizeof($Proj_Arr) > 1) && ($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY))
		$Projekt_IDopen_ds=1077;	// Alle Vorgänge
	else
		$Projekt_IDopen_ds=0;
	$Projekt_IDopen_arg=array('override'=>$Proj_Arr);
}

// für "Projekte verschieben"
else
{
	if($_POST['Projekt_IDopen'])
		$GLOBALS['fxptdata']['backup']['Projekt_IDopen']=$_POST['Projekt_IDopen'];
	if($GLOBALS['fxptdata']['backup']['Projekt_IDopen'])
	{
		$maskenfeldwerte['projekt_idopen']=$GLOBALS['fxptdata']['backup']['Projekt_IDopen'];
		$Projekt_IDopen=$GLOBALS['fxptdata']['backup']['Projekt_IDopen'];
	}
	// Project already checked out by another person?
	if(prjIsCheckedOut($Projekt_IDopen))
	{
		$Projekt_IDopen=0;
		@prjCheckIn();
	}
	if($_POST['Button_ProAnzeigen'] && !$maskenfeldwerte['projekt_idopen'])
		error_msg(734, array('Projekt_IDopen'), 'Projekt_IDopen');
	if($_POST['Button_ProAnzeigen'])
		$db_update=false;
	if(($GLOBALS['fxpglobals']['tr'] == 155) && ($_POST['Button_Check'] || $_POST['Button_Submit']))
		$_POST['Button_ProAnzeigen']=true;
}

// Projektinformationen
$Button_ProAnzeigen=$_POST['Button_ProAnzeigen'];
if($_POST['Button_ProAnzeigen'])
{
	$GLOBALS['fxptdata']['backup']['apid']=(int)$Projekt_IDopen;
	$GLOBALS['fxptdata']['backup']['epid']=0;
	$GLOBALS['fxptdata']['backup']['part']=0;
}
else if($_POST['Button_Submit'])
{
	$GLOBALS['fxptdata']['backup']['apid']=0;
	$GLOBALS['fxptdata']['backup']['epid']=(int)$Elter;
	$GLOBALS['fxptdata']['backup']['part']=(int)$Projektart;
}
else
	$GLOBALS['fxptdata']['backup']['part']=(int)$Projektart;

// Logikzuordnung: Kein Elter bei einem Mutterprojekt
if($GLOBALS['fxptdata']['backup']['part'] == FXP_PRJ_MAIN)
{
	$Elter=0;
	$GLOBALS['fxptdata']['backup']['epid']=$Elter;
}

$vAPID=(int)$GLOBALS['fxptdata']['backup']['apid'];
$vEPID=(int)$GLOBALS['fxptdata']['backup']['epid'];
$vPART=(int)$GLOBALS['fxptdata']['backup']['part'];
//echo(__FILE__.' ['.__LINE__.'] -- <B>$vAPID:</B> '.$vAPID.', <B>$vEPID:</B> '.$vEPID.', <B>$vPART:</B> '.$vPART.'<BR>');

$infvn='';
$pinfo=array();
$budgetrahmen=array();
if(($vAPID || $vEPID) && ($_POST['Button_ProAnzeigen'] || $_POST['Button_Submit'] || $_POST['Button_okay'] || $_POST['Button_Loeschen'] || $_POST['Button_Anpassen']))
{
	if($vAPID && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
	{
		$infvn=inferior_vn($vAPID);
		$pinfo=projektinfo($vAPID, false, false, true);
//echo(__FILE__.' ['.__LINE__.'] -- $vAPID='.$vAPID.'<BR>');

		// load data of user defined fields, if any
		if(($GLOBALS['fxpglobals']['tr'] == 23) && fxIsArray($user_form_fields=get_user_form_fields()))
		{
			foreach($user_form_fields as $fields)
				$field_string .= ", ".$fields['feldname'];
			$user_form_data=db_values("SELECT ".substr($field_string, 2)." FROM ".substr($user_form_fields[0]['name_maske'], 1)." WHERE projekt_id=".$vAPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			// for each field-value pair, set the form control
			if(fxIsArray($user_form_data))
			{
				foreach($user_form_data as $fieldname => $value)
					$maskenfeldwerte[$fieldname]=$value;
			}
		}

		$vPART=$pinfo[$vAPID]['data']['projektart'];
		$vEPID=$pinfo[$vAPID]['data']['elter'];
	}
	else if($vEPID && ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	{
		$pinfo=projektinfo($vEPID, false, false, true);
//echo(__FILE__.' ['.__LINE__.'] -- $vEPID='.$vEPID.'<BR>');
		if(!$vPART)
		{
			$vPART=FXP_PRJ_TASK;
			if($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_MAIN)
				$vPART=FXP_PRJ_SUB;
			else if($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_TASK)
				$vEPID=$pinfo[$vEPID]['data']['elter'];
		}
	}

	// Budgetrahmen
	$budgetrahmen=get_budgetrahmen($vEPID, $vAPID);
}

$GLOBALS['fxptdata']['backup']['apid']=$vAPID;
$GLOBALS['fxptdata']['backup']['epid']=$vEPID;
$GLOBALS['fxptdata']['backup']['part']=$vPART;
//fxDebug($GLOBALS['fxptdata']['backup'], __FILE__.' ['.__LINE__.'] -- $GLOBALS[\'fxptdata\'][\'backup\']['.$GLOBALS['fxpglobals']['tr'].']');
//echo(__FILE__.' ['.__LINE__.'] -- <B>$vAPID:</B> '.$vAPID.', <B>$vEPID:</B> '.$vEPID.', <B>$vPART:</B> '.$vPART.'<BR>');

// Allgemeine Variablen
$pma=array('pers', 'mat');
$iea=array('int', 'ext');
$mwaehrung=' '.$GLOBALS['fxpglobals']['settings']['waehrung'];

// Prüfen, ob Benutzermaske angezeigt werden soll
$sql="SELECT tm.nummer FROM trans_maske tm, masken m WHERE m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.nummer=tm.nummer AND tm.transakt_id=23 AND tm.nummer<0";
$umi=(int)db_value($sql);
if(($umi < 0) && ($Projekt_ID < 1) && !$_POST['Button_ProAnzeigen'] && !$_POST['Button_Submit'] && !$_POST['Button_okay'] && !$_POST['Button_Loeschen'] && !$_POST['Button_Anpassen'])
	${'maske'.$umi.'_nv'}=true;

// Set maske{$maske_id}_nv for all forms but form 95, 222 and 317
if(($GLOBALS['fxpglobals']['tr'] == 23) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE) && ($Projekt_ID < 1))
{
	$hforms=hide_forms($GLOBALS['fxpglobals']['tr'], array(95, 222, 317));
	extract($hforms);
}

// Popup-Abfrage bei Projektkategorie, Projektleiter-, stellv. Projektleiter-, Projektmanager- und Ansprechpartneränderung
$sub_change=0;
if($vAPID && ($GLOBALS['fxpglobals']['tr'] == 23) && ($vPART != FXP_PRJ_TASK) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	// Prüfen, ob es überhaupt untergeordnete Vorgänge gibt
	$sql="SELECT projekt_id FROM projekte WHERE elter=".$vAPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$cpi=(int)db_value($sql);
	if($cpi)
	{
		if(isset($_POST) && is_array($_POST) && isset($_POST['aktionscode']) && $_POST['Button_okay'])
			$sub_change=(int)$_POST['aktionscode'];
		echo('<div id="subchange" style="display:none;">'.$sub_change.'</div>'.$nl);
	}
}

// Check out main project and all offsprings by this person
if(prjValidPF())
	$success=prjCheckOut($Projekt_IDopen);
if($Projekt_IDopen)
	$prjCheckWarning=false;
?>