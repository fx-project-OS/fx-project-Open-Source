<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_c_daten_ini.inc                                          //
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

$maske95_bt=true;

// "Freigabe" (= approval)
$app=prjTableHasApproval();

// Sorgt dafür, dass die Projektemaske beim Ändern sofort angezeigt wird
if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && !$_POST['Button_Zurueck'] && !$_POST['Button_okay'] && !$_POST['Button_Anpassen'])
{
	$_POST['Button_Submit']=true;
	$Button_Submit=true;
}

// Variable für die Anzeigebedigung der Masken wird gefüllt
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || $_POST['Button_Submit'] || $_POST['Button_okay'] || $_POST['Button_Anpassen'])
	$show=true;

//	Nach Klick auf diese Buttons oder Links soll noch kein manipulierender DB-Zugriff erfolgen
if(isset($db_update) && $db_update && (fxIsArray($_GET) || $_POST['Button_Zurueck'] || $Button_ProAnzeigen || $suche))
	$db_update=false;

// Variablen für Ressourcenplanung
$res=array();
$sor=0;
if($Projekt_ID && ($vPART == FXP_PRJ_TASK) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
{
	$res=prj_get_resources($Projekt_ID);
	$sor=sizeof($res);

	// 1900: Die Ressourcenplanung darf nur geändert werden, wenn höchtens eine Ressource der Aufgabe zugeordnet ist. Es wurden aber bereits $var1 Ressourcen zugeteilt! Bitte benutzen Sie hierfür die ausführliche Ressourcenplanungs-Transaktion.
	if($db_update && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && isset($_POST['Personen_ID']) && ($sor > 1))
	{
		error_msg(1900, false, $sor);
		$err=true;
	}
}
//fxDebug($res, __FILE__.' ['.__LINE__.'] -- $res: ('.$sor.')');

// Beim Löschen prüfen ob dies möglich ist
$errDEL=false;
if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $Projekt_ID)
{
	$deleteable=checkDelProject($Projekt_ID);
	if(!$deleteable)
		$errDEL=true;
}
if($errDEL)
	$Button_Loeschen_nv=true;

// Variablen für rahmen.inc, die Anzeigebedingungen kontrollieren
if(!$no_project)
	$not_disabled=true;

// Sind überhaupt MAs zugeordnet? => Ja, dann wird Maske angezeigt
$MaplanSQL='';
if($Projekt_ID && $infvn && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
{
	$mpsql="SELECT maplanung_id FROM maplanung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id IN (".$infvn.")";
	$MaplanSQL=db_value($mpsql);
}

// Dies sind Steuervariablen, die gesetzt werden.
// Sie sollen aber nur gesetzt sein, wenn OK zweimal hintereinander betätigt wird
if(!$_POST['Button_Anpassen'] && !$_POST['Button_okay'])
{
	$update=false;
	$stretch_s=false;
	$stretch_e=false;

	protectVar($update, 'update');
	protectVar($stretch_s, 'stretch_s');
	protectVar($stretch_e, 'stretch_e');

	$GLOBALS['fxptdata']['backup']['Backup']['kor_bud']=false;
	$GLOBALS['fxptdata']['backup']['Backup']['chk_we_start']=false;
	$GLOBALS['fxptdata']['backup']['Backup']['chk_we_end']=false;
}

// Projektart vorerst nicht änderbar
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
{
	$maske222_nv=true;
	$Projektart_sf=2;
	$Elter_sf=2;
}

// Prüfen, ob Ressourcenplanungs-Button angezeigt werden soll...
$maske95_nv=true;
// ...Vorgang muss existieren; Anzeige- oder Änderungsmodus; Recht auf Ressourcentransaktion
if(($Projekt_ID > 0) && (($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)) && tr_rechte(43))
{
	$sql="SELECT projekt_status, projektleiter, vertreter FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$spv=db_values($sql);
	if(fxIsArray($spv))
	{
		if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && isset($_POST) && isset($_POST['Projekt_Status']))
			$c_pst=(int)$_POST['Projekt_Status'];
		else
			$c_pst=(int)$spv['projekt_status'];
		// ...geplanter oder aktiver Vorgang
		if(($c_pst == FXP_PS_PLANNED) || ($c_pst == FXP_PS_ACTIVE))
		{
			$c_pl=(int)$spv['projektleiter'];
			$c_spl=(int)$spv['vertreter'];
			// ...Projektrecht
			if($GLOBALS['fxpglobals']['persdat']['roles'][-3] || ($GLOBALS['fxpglobals']['persdat']['roles'][1] && (($c_pl == $GLOBALS['fxpglobals']['person']) || ($c_spl == $GLOBALS['fxpglobals']['person']))) || $GLOBALS['fxpglobals']['persdat']['roles'][2])
				$maske95_nv=false;
		}
	}
}

// Prüfen, ob bereits Rechnungen erstellt wurden zu irgendeinem untergeordneten Vorgang
$invoice_act=false;
$invoice_sub=false;
if(($Projekt_ID > 0) && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	$sql  = "SELECT p.rechnungs_id FROM rechnungsposition p, rechnung r WHERE p.projekt_id=".$Projekt_ID." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND r.rechnungs_id=p.rechnungs_id AND (r.archiv IS NULL OR r.archiv=0) AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=(int)db_value($sql);
	if($tmp)
		$invoice_act=true;
	else
	{
		$sql="SELECT rechnungs_id FROM rechnung WHERE mutterprojekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (archiv IS NULL OR archiv=0)";
		$tmp=(int)db_value($sql);
		if($tmp)
			$invoice_act=true;
	}

	$pvn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$sql  = "SELECT p.rechnungs_id FROM rechnungsposition p, rechnung r";
	$sql .= " WHERE p.projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pvn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND r.rechnungs_id=p.rechnungs_id AND (r.archiv IS NULL OR r.archiv=0) AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=(int)db_value($sql);
	if($tmp)
		$invoice_sub=true;
}
?>