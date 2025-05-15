<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_bew_auswahl_pre.inc                                      //
// Version     : 24.1                                                         //
// Begin       : 2021-01-14                                                   //
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
 * PF 245: Project Plan Statuses
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(isset($index))
	unset($index);

// Variablen
$anz_monate=12;	// Wieviele Monate in der Historie maximal zurückgehen

// Auswahlmaske (487) immer im Änderungsmodus + Datumsfelder inkl. Popup-Icon
$maske487_sf=true;
$GLOBALS['cpo'.$tr]=true;

// Vorgang (Auswahlfeld)
if(!$_POST['Projekt_ID'] && !$_POST['Button_Anzeigen'] && !$_POST['Button_Multi'])
	$_POST['Projekt_ID']=$GLOBALS['fxpglobals']['project'];
else if(!$_POST['Projekt_ID'])
	$_POST['Projekt_ID']=0;
$Projekt_ID_ds=0;
$maskenfeldwerte['projekt_id']=$_POST['Projekt_ID'];

// Historie (Auswahlfeld)
$txt_monat=meldung(1239, true);		// Monat
$txt_monate=meldung(1257, true);	// Monate
if(!$_POST['Projekt_HistoryID'] && !$_POST['Button_Anzeigen'] && !$_POST['Button_Multi'])
	$_POST['Projekt_HistoryID']=-1;	// Komplettinfo
else if(!$_POST['Projekt_HistoryID'])
	$_POST['Projekt_HistoryID']=0;
$Projekt_HistoryID_ds=0;
$hist_array=array(0=>meldung(388, true));	// Ohne
$stl=strlen((string)$anz_monate);
for($m=1; $m<=$anz_monate; $m++)
{
	$fstl=$stl-strlen((string)$m);
	if($fstl)
		$txt_space=str_repeat('&nbsp;', ($fstl*2));
	else
		$txt_space='';
	$hist_array[$m]=$txt_space.$m.'&nbsp;';
	if($m == 1)
		$hist_array[$m] .= $txt_monat;	// 1 Monat
	else
		$hist_array[$m] .= $txt_monate;	// x Monate
}
$hist_array[-1]=meldung(1562, true);	// Komplettinfo
$Projekt_HistoryID_arg=array('override'=>$hist_array);
$maskenfeldwerte['projekt_historyid']=$_POST['Projekt_HistoryID'];

// ausfürlich (Checkbox)
if($_POST['ausfuehrlich'])
	$maskenfeldwerte['ausfuehrlich']=1;

// Bild (Checkbox)
if(isset($GLOBALS['_ext_chartdir']) && strlen((string)$GLOBALS['_ext_chartdir']))
{
	if(!$_POST['bild'] && !$_POST['Button_Anzeigen'] && !$_POST['Button_Multi'])
		$_POST['bild']=1;
	if($_POST['bild'])
		$maskenfeldwerte['bild']=1;
}
else
	$bild_nv=true;
?>