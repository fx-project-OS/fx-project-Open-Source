<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personalstammblatt_strg_pre.inc                              //
// Version     : 21.1                                                         //
// Begin       : 2020-12-17                                                   //
// Last Change : 2020-12-17                                                   //
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
 * PF 205: Employee's Main Sheet
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

${'cpo'.$GLOBALS['fxpglobals']['tr']}=true;
// Eingabe in der Maske 424 immer ermöglichen, auch wenn im Anzeigemodus
$maske424_sf=true;

// Feld: Personen
$Personen_ID_arg=array('modus'=>0, 'ptyp'=>FXP_PT_EMPLOYEE);
if($GLOBALS['fxpglobals']['tr'] == 204)	// ext.
	$Personen_ID_arg['ptyp']=FXP_PT_CONTRACTOR;
else if($GLOBALS['fxpglobals']['tr'] != 205)	// int.
	$Personen_ID_arg['ptyp'] .= ','.FXP_PT_CONTRACTOR;
if(!isset($Personen_ID))
	$Personen_ID=0;
$maskenfeldwerte['personen_id']=$Personen_ID;

// Felder: Zeitspanne von + Zeitspanne bis
$maskenfeldwerte['zeitspanne_von']=$Zeitspanne_von;
$maskenfeldwerte['zeitspanne_bis']=$Zeitspanne_bis;

// Auswahlfeld: Zeitdaten
$Anz_Arb_ds=1504; // Keine
if(!isset($Anz_Arb))
	$Anz_Arb=$Anz_Arb_ds;
$maskenfeldwerte['anz_arb']=$Anz_Arb;
// Auswahlfeld: Urlaub
$Anz_Url_ds=1504;
if(!isset($Anz_Url))
	$Anz_Url=$Anz_Url_ds;
$maskenfeldwerte['anz_url']=$Anz_Url;
// Auswahlfeld: Krankheit
$Anz_Kr_ds=1504;
if(!isset($Anz_Kr))
	$Anz_Kr=$Anz_Kr_ds;
$maskenfeldwerte['anz_kr']=$Anz_Kr;
// Auswahlfeld: Schulung
$Anz_Schu_ds=1504;
if(!isset($Anz_Schu))
	$Anz_Schu=$Anz_Schu_ds;
$maskenfeldwerte['anz_schu']=$Anz_Schu;

// Checkbox-Feld: Vertragsdaten
if(isset($Anz_Vd) && $Anz_Vd)
	$maskenfeldwerte['anz_vd']='1';
// Checkbox-Feld: Bankdaten
if(isset($Anz_Ba) && $Anz_Ba)
	$maskenfeldwerte['anz_ba']='1';
// Checkbox-Feld: Adressen
if(isset($Anz_Adr) && $Anz_Adr)
	$maskenfeldwerte['anz_adr']='1';
// Checkbox-Feld: Kommunikation
if(isset($Anz_Kom) && $Anz_Kom)
	$maskenfeldwerte['anz_kom']='1';
// Checkbox-Feld: (nicht) verrechenbare Std.
if(isset($nicht_abrechenbar) && $nicht_abrechenbar)
	$maskenfeldwerte['nicht_abrechenbar']='1';
// Checkbox-Feld: Fähigkeiten
if(isset($Anz_Fae) && $Anz_Fae)
	$maskenfeldwerte['anz_fae']='1';
// Checkbox-Feld: Personenhierarchie
if(isset($Anz_PersHier) && $Anz_PersHier)
	$maskenfeldwerte['anz_pershier']='1';

// Auswahlfeld(er): Prj.-Kategoriegruppe
pk_getCategories(true, false, true);
//fxDebug($GLOBALS['pg'], 'pg');

$snw=meldung(594, true);					// Auswahl in neuem Fenster öffnen
$sop=explode('|', meldung(2785, true));	// Nicht|Summe|Detailiert (ohne Projekte)|Detailiert (mit Projekten)

$lbl=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=424 AND eingabefeld=1492 AND anzeigeart=3");	// Literal Breite: Links
$lbr=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=424 AND eingabefeld=1491 AND anzeigeart=3");	// Literal Breite: Rechts
$sb=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=424 AND eingabefeld=1491 AND anzeigeart=8");		// Select Breite
//echo('$lbl='.$lbl.', lbr='.$lbr.', $sb='.$sb.'<hr />');

$cn=0;
$ls=false;

$pgs='<table id="mt424_z25" cellpadding="0" cellspacing="0">'.$nl;
foreach($GLOBALS['pg'] as $grp)
{
//	if(sizeof($grp['cat']))
	{
		if(($cn%3 == 0) && !$ls)
		{
			$pgs .= '<tr>'.$nl;
			$ls=true;
			$align='';
		}
		else
			$align=' align="right"';

		if($cn%3 == 0)
			$lb=$lbl;
		else
			$lb=$lbr;

		$s_id	= 'sid_'.$grp['id'];
		$s_name	= 'pcg_'.$grp['id'];
		$s_val	= (int)${$s_name};

		$GLOBALS[$s_name.'_ds']=0;
		$GLOBALS[$s_name.'_ds_array']=$sop;

		// Literal
		$pgs .= '<td id="td_lit_'.$s_name.'" nowrap width="'.$lb.'"'.$align.' style="border:0;border-top:2px solid transparent;border-bottom:2px solid transparent;border-right:1px solid transparent;"><span id="lit_'.$s_name.'" class="lit">'.$grp['val'].':&nbsp;<br></span></td>'.$nl;
		// Auswahlfeld
		$pgs .= '<td id="td_'.$s_name.'" nowrap width="'.$sb.'" style="border:0;border-top:2px solid transparent;border-bottom:2px solid transparent;border-right:1px solid transparent;">'.fieldSelect($s_name,$s_val,$sb).'</td>'.$nl;

		if(($cn%3 == 2) && $ls)
		{
			$pgs .= '</tr>'.$nl;
			$ls=false;
		}

		$cn++;
	}
}
if($ls)
{
	$pgs .= '</tr>'.$nl;
	$ls=false;
}
$pgs .= '</table>'.$nl;
$maskenfeldwerte['nv_text1']='~'.$pgs;
?>