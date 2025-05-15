<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_strg_pre.inc                                  //
// Version     : 24.1                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$anzahl=0;
if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2163]))
	$anzahl=sizeof($vorh_referenzen[2163]);
// ...Checkbox
if(($anzahl && !$Button_OK && !$Button_Uebernehmen) || $_POST['benutzer_jn'] || (!$_POST['benutzer_ma_i'] && !$_POST['benutzer_ma_e'] && !$_POST['benutzer_kunde'] && !$_POST['benutzer_anspr']))
	$maskenfeldwerte['benutzer_jn']='1';
// ...Text
if($anzahl)
	$maskenfeldwerte['vorh_benutzer']='~&nbsp;<font class="blue">'.$anzahl.'</font>';
else
	$maskenfeldwerte['vorh_benutzer']='~&nbsp;<font class="lightred">'.$anzahl.'</font>';
if(!$_POST['benutzer_ma_i'] && !$_POST['benutzer_ma_e'] && !$_POST['benutzer_kunde'] && !$_POST['benutzer_anspr'])
	$maskenfeldwerte['benutzer_jn']='1';
// Mitarb. (int) / Mitarb. (ext) / Auftraggeber / Ansprechp.
$maskenfeldwerte['benutzer_ma_i']=$_POST['benutzer_ma_i'];
$maskenfeldwerte['benutzer_ma_e']=$_POST['benutzer_ma_e'];
$maskenfeldwerte['benutzer_kunde']=$_POST['benutzer_kunde'];
$maskenfeldwerte['benutzer_anspr']=$_POST['benutzer_anspr'];

// Benutzergruppen
$anzahl=0;
if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2164]))
	$anzahl=sizeof($vorh_referenzen[2164]);
// ...Checkbox
if($anzahl || $_POST['benutzergrp_jn'])
	$maskenfeldwerte['benutzergrp_jn']='1';
// ...Text
if($anzahl)
	$maskenfeldwerte['vorh_bengrp']='~&nbsp;<font class="blue">'.$anzahl.'</font>';
else
	$maskenfeldwerte['vorh_bengrp']='~&nbsp;<font class="lightred">'.$anzahl.'</font>';

// Abteilungen
$anzahl=0;
if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[2165]))
	$anzahl=sizeof($vorh_referenzen[2165]);
// ...Checkbox
if($anzahl || $_POST['abteilung_jn'])
	$maskenfeldwerte['abteilung_jn']='1';
// ...Text
if($anzahl)
	$maskenfeldwerte['vorh_abteilung']='~&nbsp;<font class="blue">'.$anzahl.'</font>';
else
	$maskenfeldwerte['vorh_abteilung']='~&nbsp;<font class="lightred">'.$anzahl.'</font>';

// Projekte (nur bei Projektdokumenten)
if($prjdoc)
{
	$anzahl=0;
	if(fxIsArray($vorh_referenzen) && fxIsArray($vorh_referenzen[1920]))
		$anzahl=sizeof($vorh_referenzen[1920]);
	// ...Checkbox
	if($anzahl || $_POST['projekt_jn'])
		$maskenfeldwerte['projekt_jn']='1';
	// ...Text
	if($anzahl)
		$maskenfeldwerte['vorh_projekt']='~&nbsp;<font class="blue">'.$anzahl.'</font>';
	else
		$maskenfeldwerte['vorh_projekt']='~&nbsp;<font class="lightred">'.$anzahl.'</font>';
}
else
{
	$projekt_jn_nv=true;
	$vorh_projekt_nv=true;
}
?>