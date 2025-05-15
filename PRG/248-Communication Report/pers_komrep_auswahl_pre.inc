<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pers_komrep_auswahl_pre.inc                                  //
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
 * PF 248: Communication Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

setForm(false);

// Keine Indizierung
if(isset($index))
	unset($index);

// Auswahlmaske (493) immer im Änderungsmodus + Datumsfelder inkl. Popup-Icon
$maske493_sf=true;
$GLOBALS['cpo'.$tr]=true;

// Kommunikationstyp (KommunikationsTyp)
$KommunikationsTyp_ds=0;
$maskenfeldwerte['kommunikationstyp']=$_POST['KommunikationsTyp'];

// Zugeordnet zu Obj. (Elter)
$Elter_ds_array=array(0=>'['.meldung(1410, true).']', -1=>'['.meldung(1504, true).']');	// 0: Alle Personen, -1: Keine
$Elter_arg=array('Modus'=>1);
$maskenfeldwerte['elter']=$_POST['Elter'];

// Suchtext (Suchtext)
$val='';
if(!is_null($_POST['Suchtext']) && strlen((string)$_POST['Suchtext']))
	$val=trim((string)$_POST['Suchtext']);
$maskenfeldwerte['suchtext']=$val;

// Postleitzahl Von (nv_text1)
$val='';
if(!is_null($_POST['nv_text1']) && strlen((string)$_POST['nv_text1']))
	$val=trim((string)$_POST['nv_text1']);
$maskenfeldwerte['nv_text1']=$val;

// (nv_text2)
$maskenfeldwerte['nv_text2']='~-';

// Postleitzahl Bis (nv_text3)
$val='';
if(!is_null($_POST['nv_text3']) && strlen((string)$_POST['nv_text3']))
	$val=trim((string)$_POST['nv_text3']);
$maskenfeldwerte['nv_text3']=$val;

// Länderkennzeichen (LandKZ)
$LandKZ_ds=1422;	// Alle
$maskenfeldwerte['landkz']=$_POST['LandKZ'];

// Anmerkung anzeigen (anmerkung_anz)
if(!fxIsArray($_POST) || !isset($_POST['Elter']))
	$_POST['anmerkung_anz']=1;
$maskenfeldwerte['anmerkung_anz']=$_POST['anmerkung_anz'];

// inaktiv (inaktiv)
if(!fxIsArray($_POST) || !isset($_POST['Elter']))
{
	if(!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fperson_inaktiv'])
		$_POST['inaktiv']=true;
	else
		$_POST['inaktiv']=$GLOBALS['fxpglobals']['filter']['inaktiv'];
}
$maskenfeldwerte['inaktiv']=$_POST['inaktiv'];

// Personen-Typ (PersonenTyp)
$maskenfeldwerte['personentyp']='';
if(fxIsArray($pt_arr))
{
	if(!$_POST['KommunikationsTyp'])
	{
		${'personentyp_'.FXP_PT_CONTACT}=1;		// Kontakt
		${'personentyp_'.FXP_PT_CONTACT_CP}=1;	// Kontakt - Ansprechpartner
	}

	$chk='';
	foreach($pt_arr as $pv)
	{
		if(!$ptr[$pv['wertetabid']])
			continue;

		$name='personentyp_'.$pv['wertetabid'];
		$checked='';
		if($maskenfeldwerte[$name] || $_POST[$name] || ${$name})
			$checked='1';
		$chk .= fieldCheckbox($name,'',$checked).'&nbsp;'.$pv['tabwert'].'<br>';
	}

	if($chk)
		$maskenfeldwerte['personentyp']='~'.$chk;
}
if(!$maskenfeldwerte['personentyp'])
{
	$PersonenTyp_nv=true;
	$sortierreihe_nv=true;
}

// Sortierreihenfolge (sortierreihe)
$sr_ab=meldung(1739, true);		// Absteigend
$sr_auf=meldung(1740, true);	// Aufsteigend
if(!$_POST['KommunikationsTyp'])
	$_POST['sortierreihe']='ab';
$ra=array('ab'=>$sr_ab, 'auf'=>$sr_auf);
$rad=fieldRadio('sortierreihe', $_POST['sortierreihe'], $ra, 'v');
$maskenfeldwerte['sortierreihe']='~'.$rad;
?>