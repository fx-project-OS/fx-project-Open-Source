<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_f_info_pre.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2021-04-08                                                   //
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

enable_dummies();

// Variablen
$vSG=1184;	// Mittel

// Felder ermitteln
if($err)
{
	$vRN=$_POST['Ref_nummer'];

	$vWDH=$_POST['Wiederholungart'];
	$vA=$_POST['Anzahl'];

	$vANM=$_POST['Anmerkung'];
	$vANMI=$_POST['AnmerkungIntern'];
}
else if(fxIsArray($pinfo))
{
	if($vAPID)
	{
		$vRN=$pinfo[$vAPID]['data']['ref_nummer'];

		$vWDH=$pinfo[$vAPID]['data']['wiederholungart'];
		$vA=$pinfo[$vAPID]['data']['anzahl'];

		$vANM=$pinfo[$vAPID]['data']['anmerkung'];
		$vANMI=$pinfo[$vAPID]['data']['anmerkungintern'];
	}
	// Wird eine Aufgabe direkt unter ein Mutterprojekt gehängt dann zusätzliche Felder vorbelegen
	else if($vEPID && !$eltex && ($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_MAIN) && ($vPART == FXP_PRJ_TASK))
	{
		$vANM=$pinfo[$vEPID]['data']['anmerkung'];
		$vANMI=$pinfo[$vEPID]['data']['anmerkungintern'];
	}
}

if(fxIsArray($pinfo) && $vAPID)
{
	$vEP=$pinfo[$vAPID]['data']['angelegt_von'];
	$vED=$pinfo[$vAPID]['data']['erstelldatum'];
}
else
{
	$vEP=$GLOBALS['fxpglobals']['person'];
	$vED=$GLOBALS['datetime'];
}

// ...Referenznummer (Ref_nummer)
$maskenfeldwerte['ref_nummer']=$vRN;

// ...Angelegt von (Erstelldatum)
if(!$vEP)
	$pp='???';
else
	$pp=person_pic($vEP);
$maskenfeldwerte['erstelldatum']='~'.$pp.'&nbsp;&nbsp;<i class="grey">['.get_entry($vED, 'datetime').']</i>';

// ...Schwierigkeitsgrad (Schwierigkeitsgrad)
$maskenfeldwerte['schwierigkeitsgrad']=$vSG;
$Schwierigkeitsgrad_nv=true;

// ...Wiederholung (Wiederholungart)
if(!$vWDH && (($aktion == FXP_DISPLAY) || ($aktion == FXP_DELETE)))
	$maskenfeldwerte['wiederholungart']='~'.meldung(1161, true);
else
{
	$Wiederholungart_ds=1161;	// (Ohne Verschiebung)
	$maskenfeldwerte['wiederholungart']=$vWDH;
}

// ...Im Voraus (Anzahl)
$maskenfeldwerte['anzahl']=$vA;

// ...Anmerkung (Anmerkung)
$maskenfeldwerte['anmerkung']=$vANM;

// ...Interne Anmerkung (AnmerkungIntern)
$maskenfeldwerte['anmerkungintern']=$vANMI;


// OK- und Zurücksetzen-Button benötigen wir nur im Neuanlage- oder Ändern-Modus
if(($aktion != FXP_CREATE) && ($aktion != FXP_CHANGE))
{
	$Button_okay_nv=true;
	$Button_Reset_nv=true;
}
// Lösch-Button benötigen wir nur im Lösch-Modus
if($aktion != FXP_DELETE)
	$Button_Loeschen_nv=true;
// Anpassen-Button benötigen wir nur bei Zeitkonflikten zwischen Vorgang u. Elter
if(!$stretch_s && !$stretch_e)
	$Button_Anpassen_nv=true;

// EK/VK-Popup?
$budgetart=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1));
if((($aktion == FXP_CREATE) || ($aktion == FXP_CHANGE)) && ($budgetart != 'N') && ($vPART == FXP_PRJ_TASK))
{
	$bs='b';
	if(($GLOBALS['fxpglobals']['settings']['waehrung'] == '€') || ($GLOBALS['fxpglobals']['settings']['waehrung'] == '&euro;'))
		$bs='e';
	else if($GLOBALS['fxpglobals']['settings']['waehrung'] == '$')
		$bs='d';
	$tt=meldung(3041);	// Recalculate budget planning fields on the basis of planned effort
	$oc='onclick="var href='.fxf_jsFunction('gProgram', '!name').'(\'popup_ppsp\', \'md=0&sf=1\')+\'&\'+'.fxf_jsFunction('ppsp_param', '!name').'()+'.fxf_jsFunction('gParam', '!name').'(); var pop='.fxf_jsFunction('PPSPPopup', '!name').'(href,-1,1);"';
	$ppsp_img=fieldLinkImage('ppsp_recalc', 'im_'.$bs.'calc', '', $tt.'" '.substr($oc,0,-1), '', 'cursor:pointer;');

	$GLOBALS['fxpvars']['ibuffer'][]=$ppsp_img;
}
?>