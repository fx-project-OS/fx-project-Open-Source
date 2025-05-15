<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : filter_pre.inc                                               //
// Version     : 24.1                                                         //
// Begin       : 2020-11-02                                                   //
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
 * PF 223: Filter
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$maske454_sf=true;
$maske454_flt=0;
setForm('filter');

// Literale 72: Personenfilter,Zeitfilter,Vorgangsfilter
$literals=explode(',', meldung(72, false));
// 73: Treffer mit aktuellem Filter
$treffer_txt=meldung(73, false);
// 272: Filtereinstellungen sind
$feinstellung_txt=meldung(272, false);
// 299: inaktiv + 300: aktiv (ohne Einschränkung)
$finaktiv_txt='<font class="red">'.meldung(299, false).'</font>';
$faktiv_txt='<font class="green">'.meldung(300, false).'</font>';
$faktiv_oe_txt='<font class="blue">'.meldung(300, false).'</font> <i>'.meldung(316, false).'</i>';

// Filtermodus: 0=Standardfilter, 1=Eigenfilter, -1=Adminfilter
$fa=getFilters();
//fxDebug($fa,'$fa');

// Werte zuweisen
if(!$err)
{
	eval($fa['setting']);
	$maskenfeldwerte=$GLOBALS['fxpglobals']['filter'];
}
//fxDebug($maskenfeldwerte, '$maskenfeldwerte');

// Nur einfachen Filter verwenden
$filter_always_active=false;
if(strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['einzelfilter'], 0, 1)) == 'n')
	$filter_always_active=true;

### Filtername + Beschreibung ###
if($err)
{
	$flt_name=$_POST['flt_name'];
	$flt_desc=$_POST['flt_desc'];
}
else
{
	$flt_name=$fa['name'];
	$flt_desc=$fa['selects'][$flt_name]['desc'];
}
$flt_tt=meldung(3081);	// Filtername eingeben des aktuellen Filters (<b>Speichern</b>)<br />oder eines neuen Filters (<b>Speichern als</b>)
$flt_lit=get_text(1093,'text');	// Filtername
$flt_field=fieldText('flt_name', $flt_name, 797, 0, 'left', false, false, false, $flt_tt, '', 'text', 'filter', 'border:0;background:#f8f8f8;', 's5b');
$maskenfeldwerte['allg_textfeld'] ='^class_maskc|~<table cellspacing=0 cellpadding=0 border=0>';
$maskenfeldwerte['allg_textfeld'] .= '<tr><td class="lit" width="120">&nbsp;<b class="lit">'.$flt_lit.'</b>:</td><td><div style="background:#f8f8f8;margin:0 1px;margin-bottom:3px;padding:8px;border:1px solid #797979;border-radius:8px;box-shadow:4px 4px 4px rgba(0,0,0,.33) inset;" tooltip="'.$flt_tt.'">'.$flt_field.'</div></td></tr>';
$dsc_lit=get_text(38,'20');	// Beschreibung
//function fieldTextarea($fieldname, $value, $width=0, $height=0, $len=0, $mand=false, $dis=false, $nva=false, $tooltip='', $jsfunc='', $form='')
$dsc_field=fieldTextarea('flt_desc', $flt_desc, 813, 80);
$maskenfeldwerte['allg_textfeld'] .= '<tr><td class="lit" valign="top">&nbsp;'.$dsc_lit.'</b>:</td><td>'.$dsc_field.'</td></tr>';
$maskenfeldwerte['allg_textfeld'] .= '</table>';

### Personenfilter ###
// - Titelzeile mit Icon
$flt=filter_person('from', true, false, 'test');
$cls='red';
$checked='';
$fstatus_txt=$finaktiv_txt;
if($test_flt & 4)		// Filter aktiv mit Einschränkungen
{
	$cls='green';
	$checked='1';
	$fstatus_txt=$faktiv_txt;
}
else if($test_flt & 1)	// Filter aktiv ohne Einschränkungen
{
	$cls='blue';
	$checked='1';
	$fstatus_txt=$faktiv_oe_txt;
}
if($filter_always_active)
	$checked='1';

$mff='<td align="center">&nbsp;'.fieldCheckbox('fperson_inaktiv','',$checked).'&nbsp;&nbsp;</td>';
$maskenfeldwerte['perstitel'] ='^class_maskc '.$cls.'|~<table border="0"><tr>'.$mff.'<td><font class="s4b">'.$literals[0].'</font></td></tr></table>';

// - Treffer
$sql="SELECT COUNT(p.personen_id)".$flt;
$trf=db_value($sql);
$tt='';
$maskenfeldwerte['person_erledigt']='~<table border="0" cellpadding="0" cellspacing="1"><tr><td id="personen_filter_title"'.$tt.'>&nbsp;&nbsp;'.$feinstellung_txt.': '.$fstatus_txt.'<br />&nbsp;&nbsp;'.$treffer_txt.': <b>'.$trf.'</b></td></tr></table>';

// - Projekt-Team
$projektteam_id_ds=1422; // Alle

// - Vorgesetzter
$Vorgesetzter_ds=1422; // Alle

// - Abteilung
$Abt_ID_ds=0;

// - Fähigkeit
$MaFaehigkeit_ID_ds=1422; // Alle

// - Lieferant
$Person_alias_lit=meldung(2091,false);	// Vetragspartner
$Person_alias_ds=1422; // Alle
$Person_alias_arg=array('ptyp'=>2091, 'ignore_personenfilter'=>true);

// - Personentyp
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=30 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY tabwert";
$pt_arr=db_values($sql, '*');
if(fxIsArray($pt_arr))
{
	$chk='';
	foreach($pt_arr as $pv)
	{
		$name='personentyp_'.$pv['wertetabid'];
		$checked='';
		if((!$err && $maskenfeldwerte[$name]) || ($err && ($_POST[$name] || ${$name})))
			$checked='1';
		$chk .= fieldCheckbox($name,'',$checked).'&nbsp;'.$pv['tabwert'].'<br />';
	}
	$maskenfeldwerte['personen_typ']='~'.$chk;
}
else
	$Personen_Typ_nv=true;

### Zeitfilter ###
// - Titelzeile mit Icon
$flt=filter_zeit('von', 'bis', true, 'test');
$cls='red';
$checked='';
$fstatus_txt=$finaktiv_txt;
if($test_flt & 32)		// Filter aktiv mit Einschränkungen
{
	$cls='green';
	$checked='1';
	$fstatus_txt=$faktiv_txt;
}
else if($test_flt & 8)	// Filter aktiv ohne Einschränkungen
{
	$cls='blue';
	$checked='1';
	$fstatus_txt=$faktiv_oe_txt;
}
if($filter_always_active)
	$checked='1';

$mff='<td align="center">&nbsp;'.fieldCheckbox('fzeit_inaktiv','',$checked).'&nbsp;&nbsp;</td>';	//<INPUT TYPE="checkbox" NAME="fperson_inaktiv"'.$checked.' VALUE="1" STYLE="width:15; height:15"></TD><TD>';
$maskenfeldwerte['datum'] ='^class_maskc '.$cls.'|~<table border="0"><tr>'.$mff.'<td><font class="s4b">'.$literals[1].'</font></td></tr></table>';

// - Treffer
$tt='';
$maskenfeldwerte['datumsangabe']='~<table border="0" cellpadding="0" cellspacing="1"><tr><td id="pers_filter_title"'.$tt.'>&nbsp;&nbsp;'.$feinstellung_txt.': '.$fstatus_txt.'<br />&nbsp;&nbsp;'.meldung(314, false).'</td></tr></table>';

### Projektfilter ###
// - Titelzeile mit Icon
$flt=filter_projekt('from', true, false, 'test');
$cls='red';
$checked='';
$fstatus_txt=$finaktiv_txt;
if($test_flt & 256)		// Filter aktiv mit Einschränkungen
{
	$cls='green';
	$checked='1';
	$fstatus_txt=$faktiv_txt;
}
else if($test_flt & 64)	// Filter aktiv ohne Einschränkungen
{
	$cls='blue';
	$checked='1';
	$fstatus_txt=$faktiv_oe_txt;
}
if($filter_always_active)
	$checked='1';

$mff='<td align="center">&nbsp;'.fieldCheckbox('fprojekt_inaktiv','',$checked).'&nbsp;&nbsp;</td>';
$maskenfeldwerte['projektanzeige']='^class_maskc '.$cls.'|~<table border="0"><tr>'.$mff.'<td><font class="s4b">'.$literals[2].'</font></td></tr></table>';

// - Treffer
$sql="SELECT COUNT(p.projekt_id)".$flt;
$trf=db_value($sql);
$tt='';
$maskenfeldwerte['projektauswahl']='~<table border="0" cellpadding="0" cellspacing="1"><tr><td id="projekt_filter_title"'.$tt.'>&nbsp;&nbsp;'.$feinstellung_txt.': '.$fstatus_txt.'<br />&nbsp;&nbsp;'.$treffer_txt.': <b>'.$trf.'</b></td></tr></table>';

// - Mutterprojekt
$Mutterprojekt_ID_ds=1422; // Alle

// - Auftraggeber
$Kunde_arg=array('pr_check'=>true);
$Kunde_ds=1422; // Alle

// - Projektmanager
$Projektmanager_ds=1422; // Alle

// - Projektleiter
$Projektleiter_ds=1422; // Alle

// - Kategorie
$Kategorie_ID_ds=1422; // Alle

// - Projekt-Status
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=137 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position, tabwert";
$ps_arr=db_values($sql, '*');
if(is_array($ps_arr))
{
	$chk='';
	foreach($ps_arr as $pv)
	{
		$name='projektstatus_'.$pv['wertetabid'];
		$checked='';
		if((!$err && $maskenfeldwerte[$name]) || ($err && ($_POST[$name] || ${$name})))
			$checked='1';
		$chk .= fieldCheckbox($name,'',$checked).'&nbsp;'.$pv['tabwert'].'<br />';
	}
	$maskenfeldwerte['projekt_status']='~'.$chk;
}
else
	$Projekt_Status_nv=true;

// - Priorität
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=47 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position, tabwert";
$pr_arr=db_values($sql, '*');
if(is_array($pr_arr))
{
	$chk='';
	foreach($pr_arr as $pv)
	{
		$name='prioritaet_'.$pv['wertetabid'];
		$checked='';
		if((!$err && $maskenfeldwerte[$name]) || ($err && ($_POST[$name] || ${$name})))
			$checked='1';
		$chk .= fieldCheckbox($name,'',$checked).'&nbsp;'.$pv['tabwert'].'<br />';
	}
	$maskenfeldwerte['prioritaet']='~'.$chk;
}
else
	$Prioritaet_nv=true;

### Buttons ###
// Filtermodus: 0=Standardfilter, 1=Eigenfilter, -1=Adminfilter
// ...Speichern
if($fa['mode'] >= 0)
{
	$btn=fieldSubmit('Button_Speichern', get_text(1360,'15'));
	$GLOBALS['fxpvars']['bbuffer'][0]=$btn;
}
// ...Speichern als [Standard]
if($fa['mode'])
{
	$btn=fieldSubmit('Button_Speichern_als', get_text(2015,'15').' [Standard]');
	$GLOBALS['fxpvars']['bbuffer'][1]=$btn;
}
// ...Löschen
if($fa['mode'] >= 1)
{
	$btn=fieldSubmit('Button_Loeschen', get_text(282,'15'));
	$GLOBALS['fxpvars']['bbuffer'][2]=$btn;
}
?>