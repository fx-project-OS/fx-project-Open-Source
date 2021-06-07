<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : madaten_pre.inc                                              //
// Version     : 21.1                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * PF 7: Employee's Contract Data
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Variablen
$year=(int)substr($GLOBALS['datetime'],0,4);
$ek=0.0;
$vk=0.0;
$ua=0.0;
$uu=0.0;
$pt=0;

$mid=-1;
$aid=-1;

// Einheiten
$std=meldung(484,false);	// Std.
$tag=meldung(917,false);	// Tag(e)
$uhr=meldung(3017,false);	// Uhr.

// Analisieren ob ausgewählte Person schon in MADATEN eingetragen ist
if($Personen_ID)
{
	if(!$verr)
	{
		$sql="SELECT * FROM madaten WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$maskenfeldwerte=db_values($sql);

		$maskenfeldwerte['arbeitsbeginn']=$maskenfeldwerte['soll_beginn'];
		$maskenfeldwerte['anmerkungintern']=$maskenfeldwerte['anmerkung'];
		$maskenfeldwerte['ek']=$ek;
		$maskenfeldwerte['vk']=$vk;
		$maskenfeldwerte['urlaubsanspruch']=$ua;
		$maskenfeldwerte['resturlaub']=$uu;

		$mid=(int)$maskenfeldwerte['madaten_id'];
	}

	// Personen-ID
	if($GLOBALS['with_client_id'])
		$maskenfeldwerte['personen_id']=$GLOBALS['fxpglobals']['client'].'_'.$Personen_ID;
	else
		$maskenfeldwerte['personen_id']=$Personen_ID;

	// Name + Bild
	if(fxIsArray($prsarr))
	{
		$pt=$prsarr['personentyp'];
		// ...Name
		$maskenfeldwerte['pname']='~<b>'.$prsarr['pname'].'</b>';
		if(strlen(trim($prsarr['vorname'])))
			$maskenfeldwerte['pname'] .= ',&nbsp;'.trim($prsarr['vorname']);
		if(strlen(trim($prsarr['personen_initialen'])))
			$maskenfeldwerte['pname'] .= '&nbsp;&nbsp;<i class="lightergrey">['.trim($prsarr['personen_initialen']).']</i>';
		$maskenfeldwerte['pname'] .= fieldHidden('pt',$pt);

		// ...Bild
		$pmw=148;
		$pmh=126;
		$personen_bildarray=getPersonIcons($Personen_ID, $pmh, 0, $GLOBALS['fxpglobals']['client'], $prsarr['personentyp'], true);
		$iso='';
		if($personen_bildarray['def'])
			$iso='opacity:0.1;';
		$maskenfeldwerte['bild']='~<img src="'.$personen_bildarray['dsp'].$personen_bildarray['mtime'].'"'.$personen_bildarray['size_dsp'].' align="right" style="position:absolute;right:12px;max-height:'.$pmh.'px;max-width:'.$pmw.'px;width:auto;height:auto;border:1px solid white;border-right-color:black;border-bottom-color:black;border-radius:8px;box-shadow:2px 2px 4px rgba(0,0,0, 0.5);'.$iso.'">';
	}

	// Firmenandresse
	$Firm_Adressselect="SELECT a.adressen_id, a.ort, a.strasse FROM adressen a, personen p WHERE p.personentyp=".FXP_PT_CLIENT." AND p.personen_id=a.personen_id AND p.mandanten_id=a.mandanten_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$Firm_Adressselectfeld='ort';
	$Firm_Adressselectwert='adressen_id';
	$Firm_Adressselectfunc='if(strlen(trim($selectwerte["strasse"]))) $selectwerte["ort"] .= ", ".$selectwerte["strasse"];'; 

	// EK/VK
	$sql="SELECT artikel_id, ek, vk FROM artikel WHERE personen_id=".$Personen_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL)";
	$eva=db_values($sql);
	if(fxIsArray($eva))
	{
		$aid=(int)$eva['artikel_id'];
		$ek=$eva['ek'];
		$vk=$eva['vk'];
		if(!$verr)
		{
			$maskenfeldwerte['ek']=$ek;
			$maskenfeldwerte['vk']=$vk;
		}
	}

	// Urlaubsanspruch
	if($pt == FXP_PT_EMPLOYEE)
	{
		$ul=get_resturlaub($Personen_ID, '', substr('0000'.$year, -4).'1231240000');
fxDebug($ul, '$ul', 3);
		if(fxIsArray($ul))
		{
			$ua=$ul['akt']['urlaub_pro_jahr'];
			$uu=$ul['akt']['urlaubsuebertrag_ganzjahr'];
			if(!$verr)
			{
				$maskenfeldwerte['urlaubsanspruch']=$ua;
				$maskenfeldwerte['resturlaub']=$uu;
			}
		}
	}

	// Interne Anmerkung
	if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) && strlen($maskenfeldwerte['anmerkungintern']))
	{
		$mw=db_value("SELECT anzeigegroesse1 FROM maskenfelder WHERE nummer=6 AND eingabefeld=110 AND anzeigeart=7");
		$maskenfeldwerte['anmerkungintern']='~<div style="width:'.(abs($mw)+4).'px;">'.$maskenfeldwerte['anmerkungintern'].'</div>';
	}
}

// Literal für "Name" ändern zu "Name, Vorname", sowie MaDaten_ID als "hidden" Variable mitgeben
$Pname_lit=get_text(14,'10').', '.get_text(15,'10');
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
	$Pname_lit .= fieldHidden('madaten_id',$mid);

// Literal für "Sa" und "So" einfärben, da Wochenende
$Samstag_lit='<span class="lightergrey">'.get_text(228,'10').':</span>';
$Samstag_nc=true;
$Sonntag_lit='<span class="lightergrey">'.get_text(237,'10').':</span>';
$Sonntag_nc=true;

// Literal für "EK" + "VK" ändern: "Std." hinzufügen
$EK_lit=get_text(805,'5').'/'.$std;
$VK_lit=get_text(806,'5').'/'.$std;
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
{
	$EK_lit .= fieldHidden('aek',$ek).fieldHidden('artikel_id',$aid);
	$VK_lit .= fieldHidden('avk',$vk);
}

// Arbeitsstunden/Tag + Arbeitszeit
$arbstdtag_lit=meldung(1888,false);	// Arbeitsstunden/Tag
$arbzeit_lit=meldung(1177,false);	// Arbeitszeit
$maskenfeldwerte['zeitraum']='~<div style="width:404px;"><div style="float:left;"><span class="lit">'.$arbstdtag_lit.':</span> <span id="arbstdtag"></span> '.$std.'</div><div style="float:right"><span class="lit">'.$arbzeit_lit.':</span> <span id="arbzeit"></span></div></div>';

// Einheitsfelder
// ..."Std." bei: [Arbeitsstunden/Woche] + [Pause]
$maskenfeldwerte['nv_text1']='&nbsp;'.$std;
$maskenfeldwerte['nv_text4']=$maskenfeldwerte['nv_text1'];
// ..."Tag(e)" bei: [Arbeitstage], [Urlaub/Jahr] + [Resturlaub]
$maskenfeldwerte['nv_text2']='&nbsp;'.$tag;
$maskenfeldwerte['nv_text5']=$maskenfeldwerte['nv_text2'];
$maskenfeldwerte['nv_text6']=$maskenfeldwerte['nv_text2'];
// ..."Uhr" bei: [Arbeitstagbeginn]
$maskenfeldwerte['nv_text3']='&nbsp;'.$uhr;

// Externe Mitarbeitern haben keinen Urlaubsanspruch, keine Gleittage und müssen keine Zeiterfassung machen
if($pt != FXP_PT_EMPLOYEE)
{
	$Urlaubsanspruch_nv=true;
	$nv_text5_nv=true;
	$Resturlaub_nv=true;
	$nv_text6_nv=true;
	$hr2_nv=true;

	$maskenfeldwerte['gleitzeit']='';
	$Gleitzeit_dis=true;
	$maskenfeldwerte['zeiterfassung']='';
	$Zeiterfassung_dis=true;
	$maskenfeldwerte['zeitbeschraenkung']='';
	$Zeitbeschraenkung_dis=true;
}
else if(!strlen(trim($maskenfeldwerte['zeitbeschraenkung'])))
	$maskenfeldwerte['zeitbeschraenkung']='1';

// Urlaubs-Anpassungen für den Ändern-Modus
$Urlaubsanspruch_lit=get_text(32,'text').' <span class="blue">'.($year).'</span>';
$Resturlaub_lit=meldung(1575,false).' <span class="blue">'.($year-1).'</span>';
if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
{
	$Urlaubsanspruch_lit .= fieldHidden('ua',$ua);
	$Resturlaub_lit .= fieldHidden('uu',$uu);
}

// Checkboxen
foreach($cbarr as $cbname)
{
	$cblname=strtolower($cbname);
	if(!$maskenfeldwerte[$cblname])
		$maskenfeldwerte[$cblname]='';
}

// Button ausblenden?
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;
?>